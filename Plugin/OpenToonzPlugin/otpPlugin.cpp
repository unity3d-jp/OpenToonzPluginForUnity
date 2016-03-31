#include "pch.h"
#include "OpenToonzPlugin.h"
#include "fcFoundation.h"
#include "otpPlugin.h"

typedef int(*toonz_plugin_init_t)(toonz_host_interface_t *hostif);
typedef void(*toonz_plugin_exit_t)();
extern toonz_host_interface_t g_toonz_host_interface;


otpParam::otpParam(otpInstance *parent)
    : m_parent(parent)
{
}

otpParam::~otpParam()
{
}

otpInstance* otpParam::getInstance() const { return m_parent; }
otpParamType otpParam::getType() const { return (otpParamType)m_info.type; }
const char* otpParam::getName() const { return m_info.name; }
const char* otpParam::getNote() const { return m_info.note; }

int otpParam::getLength() const
{
    switch (getType()) {
    case otParamType_String: return m_string.empty() ? 0 : (int)m_string.size() + 1;
    case otParamType_ToneCurve: return (int)m_tonecurve.size();
    }
    return 1;
}

void otpParam::getValue(void *dst) const
{
    if (!dst) { return; }

    switch (getType()) {
    case otParamType_Double:    *(otpDoubleValue*)dst = m_value.double_v; break;
    case otParamType_Range:     *(otpRangeValue*)dst = m_value.range_v; break;
    case otParamType_Pixel:     *(otpPixelValue*)dst = m_value.pixel_v; break;
    case otParamType_Point:     *(otpPointValue*)dst = m_value.point_v; break;
    case otParamType_Enum:      *(otpEnumValue*)dst = m_value.enum_v; break;
    case otParamType_Int:       *(otpIntValue*)dst = m_value.int_v; break;
    case otParamType_Bool:      *(otpBoolValue*)dst = m_value.bool_v; break;
    case otParamType_Spectrum:  *(otpSpectrumValue*)dst = m_value.spectrum_v; break;
    case otParamType_String:    memcpy(dst, m_string.c_str(), m_string.size()); break;
    case otParamType_ToneCurve: memcpy(dst, &m_tonecurve[0], sizeof(otpToneCurveValue)*m_tonecurve.size()); break;
    }
}

void otpParam::setValue(const void *src, int len)
{
    if (!src) { return; }

    switch (getType()) {
    case otParamType_Double:    m_value.double_v = *(otpDoubleValue*)src; break;
    case otParamType_Range:     m_value.range_v = *(otpRangeValue*)src; break;
    case otParamType_Pixel:     m_value.pixel_v = *(otpPixelValue*)src; break;
    case otParamType_Point:     m_value.point_v = *(otpPointValue*)src; break;
    case otParamType_Enum:      m_value.enum_v = *(otpEnumValue*)src; break;
    case otParamType_Int:       m_value.int_v = *(otpIntValue*)src; break;
    case otParamType_Bool:      m_value.bool_v = *(otpBoolValue*)src; break;
    case otParamType_Spectrum:  m_value.spectrum_v = *(otpSpectrumValue*)src; break;
    case otParamType_String:
        m_string = (const char*)src;
        break;
    case otParamType_ToneCurve:
        m_tonecurve.assign((otpToneCurveValue*)src, (otpToneCurveValue*)src + len);
        break;
    }
}

otpParamInfo& otpParam::getRawInfo() { return m_info; }



otpPort::otpPort(otpInstance *parent, const char *name)
    : m_parent(parent)
    , m_name(name)
    , m_image()
{
}

otpInstance* otpPort::getInstance() const { return m_parent; }
const char* otpPort::getName() const { return m_name.c_str(); }
void otpPort::setImage(otpImage *img) { m_image = (ImageRGBAu8*)img; }
ImageRGBAu8* otpPort::getImage() const { return m_image; }


static otpPluginInfo otpProbeToInfo(toonz_plugin_probe_t *probe)
{
    otpPluginInfo r;
    r.name = probe->name;
    r.vendor = probe->vendor;
    r.note = probe->note;
    r.version_major = probe->ver.major;
    r.version_minor = probe->ver.minor;
    return r;
}

otpInstance::otpInstance(otpModule *module, toonz_plugin_probe_t *probe)
    : m_module(module)
    , m_probe(probe)
    , m_info(otpProbeToInfo(probe))
    , m_userdata()
    , m_frame()
    , m_dst_image()
    , m_canceled()
{
    if (m_probe->handler) {
        if (m_probe->handler->setup) {
            m_probe->handler->setup(this);
        }
        if (m_probe->handler->create) {
            m_probe->handler->create(this);
        }
    }
}

otpInstance::~otpInstance()
{
    if (m_probe->handler) {
        if (m_probe->handler->destroy) {
            m_probe->handler->destroy(this);
        }
    }
}

static void otpSetupParam(toonz_param_desc_t& desc, otpParam& dst)
{
    otpParamInfo& info = dst.getRawInfo();
    info.name = desc.key;
    info.note = desc.note;
    info.type = (otpParamType)desc.traits_tag;

    switch (info.type) {
    case otParamType_Double:    dst.setValue(&desc.traits.d.def); break;
    case otParamType_Range:     dst.setValue(&desc.traits.rd.def); break;
    case otParamType_Pixel:     dst.setValue(&desc.traits.c.def); break;
    case otParamType_Point:     dst.setValue(&desc.traits.p.def); break;
    case otParamType_Enum:      dst.setValue(&desc.traits.e.def); break;
    case otParamType_Int:       dst.setValue(&desc.traits.i.def); break;
    case otParamType_Bool:      dst.setValue(&desc.traits.b.def); break;
    case otParamType_Spectrum:
        if (desc.traits.g.points > 0) {
            dst.setValue(&desc.traits.g.array[0]);
        }
        break;
    case otParamType_String:    dst.setValue(desc.traits.s.def); break;
    case otParamType_ToneCurve: /* no default value*/ break;
    }
}

void otpInstance::setParamInfo(toonz_param_page_t *pages, int num_pages)
{
    m_params.clear();
    for (int pi = 0; pi < num_pages; ++pi) {
        toonz_param_page_t& page = pages[pi];
        for (int gi = 0; gi < page.num; ++gi) {
            toonz_param_group_t& group = page.array[gi];
            for (int i = 0; i < group.num; ++i) {
                toonz_param_desc_t& desc = group.array[i];

                auto *param = new otpParam(this);
                m_params.emplace_back(otpParamPtr(param));
                otpSetupParam(desc, *param);
            }
        }
    }
}

void otpInstance::addPort(const char *name)
{
    m_ports.emplace_back(otpPortPtr(new otpPort(this, name)));
}

const otpPluginInfo& otpInstance::getPluginInfo() const
{
    return m_info;
}


int otpInstance::getNumParams() const
{
    return (int)m_params.size();
}
otpParam* otpInstance::getParam(int i)
{
    return i < m_params.size() ? m_params[i].get() : nullptr;
}
otpParam* otpInstance::getParamByName(const char *name)
{
    for (auto& p : m_params) {
        if (strcmp(p->getName(), name) == 0) {
            return p.get();
        }
    }
    return nullptr;
}

int otpInstance::getNumPorts() const
{
    return (int)m_ports.size();
}
otpPort* otpInstance::getPort(int i)
{
    return i < m_ports.size() ? m_ports[i].get() : nullptr;
}
otpPort* otpInstance::getPortByName(const char *name)
{
    for (auto& p : m_ports) {
        if (strcmp(p->getName(), name) == 0) {
            return p.get();
        }
    }
    return nullptr;
}

void* otpInstance::getUserData() const { return m_userdata; }
void otpInstance::setUsertData(void *v) { m_userdata = v; }
ImageRGBAu8* otpInstance::getDstImage() { return m_dst_image.get(); }
double otpInstance::getFrame() const { return m_frame; }

void otpInstance::createDstImage(int width, int height)
{
    if (!m_dst_image || (width != m_dst_image->getWidth() || height != m_dst_image->getHeight())) {
        m_dst_image.reset(new ImageRGBAu8(width, height));
    }
}

otpImage* otpInstance::applyFx(double frame)
{
    if (!m_dst_image) {
        printf(__FUNCTION__ "dst image is null\n");
        return nullptr;
    }
    auto dst = m_dst_image.get();

    m_frame = frame;
    m_canceled = 0;

    toonz_rendering_setting_t rs = {
        { 1, 0 },
        this,
        { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 },
        1.0,
        1.0, 1.0,
        1.0,
        32,
        1024,
        100,
        0,
        0,
        0,
        0,
        0,
        { 0.0, 0.0, 0.0, 0.0 },
        &m_canceled
    };
    m_probe->handler->do_compute(this, &rs, frame, dst);
    return dst;
}



otpModule::otpModule()
    : m_module()
    , m_probes()
{
}
otpModule::~otpModule()
{
    unload();
}

bool otpModule::load(const char *path)
{
    m_module = DLLLoad(path);
    if (!m_module) { return false; }

    // check if m_module is OpenToonz plug-in
    m_probes = (toonz_plugin_probe_list_t*)DLLGetSymbol(m_module, "toonz_plugin_info_list");
    if (m_probes == nullptr) {
        unload();
        return false;
    }

    // call toonz_plugin_init
    auto init_f = (toonz_plugin_init_t)DLLGetSymbol(m_module, "toonz_plugin_init");
    if (init_f) {
        init_f(&g_toonz_host_interface);
    }

    return true;
}

void otpModule::unload()
{
    if (m_module) {
        // call toonz_plugin_exit
        auto exit_f = (toonz_plugin_exit_t)DLLGetSymbol(m_module, "toonz_plugin_exit");
        if (exit_f) {
            exit_f();
        }

        DLLUnload(m_module);
        m_module = nullptr;
    }
}

int otpModule::getNumPlugins() const
{
    return int(((size_t)m_probes->end - (size_t)m_probes->begin) / sizeof(void*));
}

otpPluginInfo otpModule::getPluginInfo(int i)
{
    return otpProbeToInfo(m_probes->begin + i);
}

otpInstance* otpModule::createPluginInstance(int i)
{
    return new otpInstance(this, m_probes->begin + i);
}
