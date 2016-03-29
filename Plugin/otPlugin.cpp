#include "pch.h"
#include "OpenToonzPlugin.h"
#include "fcFoundation.h"
#include "otPlugin.h"

typedef int(*toonz_plugin_init_t)(toonz_host_interface_t *hostif);
typedef void(*toonz_plugin_exit_t)();
extern toonz_host_interface_t g_toonz_host_interface;


otParam::otParam()
{
}

otParam::otParam(const otParamInfo& info)
    : m_info(info)
{
}

otParam::~otParam()
{
}

otParamType otParam::getType() const { return (otParamType)m_info.type; }
const char* otParam::getName() const { return m_info.name; }
const char* otParam::getNote() const { return m_info.note; }

int otParam::getLength() const
{
    switch (getType()) {
    case otParamType_String: return m_string.empty() ? 0 : m_string.size() + 1;
    case otParamType_ToneCurve: return (int)m_tonecurve.size();
    }
    return 1;
}

void otParam::getValue(void *dst) const
{
    if (!dst) { return; }

    switch (getType()) {
    case otParamType_Double:    *(otDoubleValue*)dst = m_value.double_v; break;
    case otParamType_Range:     *(otRangeValue*)dst = m_value.range_v; break;
    case otParamType_Pixel:     *(otPixelValue*)dst = m_value.pixel_v; break;
    case otParamType_Point:     *(otPointValue*)dst = m_value.point_v; break;
    case otParamType_Enum:      *(otEnumValue*)dst = m_value.enum_v; break;
    case otParamType_Int:       *(otIntValue*)dst = m_value.int_v; break;
    case otParamType_Bool:      *(otBoolValue*)dst = m_value.bool_v; break;
    case otParamType_Spectrum:  *(otSpectrumValue*)dst = m_value.spectrum_v; break;
    case otParamType_String:    memcpy(dst, m_string.c_str(), m_string.size()); break;
    case otParamType_ToneCurve: memcpy(dst, &m_tonecurve[0], sizeof(otToneCurveValue)*m_tonecurve.size()); break;
    }
}

void otParam::setValue(const void *src, int len)
{
    if (!src) { return; }

    switch (getType()) {
    case otParamType_Double:    m_value.double_v = *(otDoubleValue*)src; break;
    case otParamType_Range:     m_value.range_v = *(otRangeValue*)src; break;
    case otParamType_Pixel:     m_value.pixel_v = *(otPixelValue*)src; break;
    case otParamType_Point:     m_value.point_v = *(otPointValue*)src; break;
    case otParamType_Enum:      m_value.enum_v = *(otEnumValue*)src; break;
    case otParamType_Int:       m_value.int_v = *(otIntValue*)src; break;
    case otParamType_Bool:      m_value.bool_v = *(otBoolValue*)src; break;
    case otParamType_Spectrum:  m_value.spectrum_v = *(otSpectrumValue*)src; break;
    case otParamType_String:
        m_string = (const char*)src;
        break;
    case otParamType_ToneCurve:
        m_tonecurve.assign((otToneCurveValue*)src, (otToneCurveValue*)src + len);
        break;
    }
}

otParamInfo& otParam::getRawInfo() { return m_info; }




otPlugin::otPlugin(toonz_plugin_probe_t *probe)
    : m_probe(probe)
    , m_userdata()
    , m_dst_image()
    , m_canceled()
{
    m_info.name = m_probe->name;
    m_info.vendor = m_probe->vendor;
    m_info.note = m_probe->note;
    m_info.version_major = m_probe->ver.major;
    m_info.version_minor = m_probe->ver.minor;
    m_userdata = this;

    if (m_probe->handler && m_probe->handler->setup) {
        m_probe->handler->setup(this);
    }
}

otPlugin::~otPlugin()
{
}

static void To_otParam(toonz_param_desc_t& desc, otParam& dst)
{
    otParamInfo& info = dst.getRawInfo();
    info.name = desc.key;
    info.note = desc.note;
    info.type = (otParamType)desc.traits_tag;

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

void otPlugin::setParamInfo(toonz_param_page_t *pages, int num_pages)
{
    m_params.clear();
    for (int pi = 0; pi < num_pages; ++pi) {
        toonz_param_page_t& page = pages[pi];
        for (int gi = 0; gi < page.num; ++gi) {
            toonz_param_group_t& group = page.array[gi];
            for (int i = 0; i < group.num; ++i) {
                toonz_param_desc_t& desc = group.array[i];

                m_params.push_back(otParam());
                To_otParam(desc, m_params.back());
            }
        }
    }
}

const otPluginInfo& otPlugin::getPluginInfo() const
{
    return m_info;
}

int otPlugin::getNumParams() const
{
    return (int)m_params.size();
}

otParam* otPlugin::getParam(int i)
{
    return &m_params[i];
}
otParam* otPlugin::getParamByName(const char *name)
{
    for (auto& p : m_params) {
        if (strcmp(p.getName(), name) == 0) {
            return &p;
        }
    }
    return nullptr;
}

void* otPlugin::getUserData() const { return m_userdata; }
void otPlugin::setUsertData(void *v) { m_userdata = v; }


void otPlugin::setDstImage(ImageRGBAu8 *img)
{
    m_dst_image = img;
}

otImage* otPlugin::applyFx(otImage *src, double frame)
{
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
    m_probe->handler->do_compute(this, &rs, frame, src);

    return m_dst_image;
}



otModule::otModule()
    : m_module()
{
}
otModule::~otModule()
{
    unload();
}

bool otModule::load(const char *path)
{
    m_module = DLLLoad(path);
    if (!m_module) { return false; }

    // check if m_module is OpenToonz plug-in
    toonz_plugin_probe_list_t *probes = (toonz_plugin_probe_list_t*)DLLGetSymbol(m_module, "toonz_plugin_info_list");
    if (probes == nullptr) {
        unload();
        return false;
    }

    // call toonz_plugin_init
    auto init_f = (toonz_plugin_init_t)DLLGetSymbol(m_module, "toonz_plugin_init");
    if (init_f) {
        init_f(&g_toonz_host_interface);
    }

    for (auto p = probes->begin; p < probes->end; ++p) {
        m_plugins.emplace_back(otPlugin(p));
    }

    return true;
}

void otModule::unload()
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

int otModule::getNumPlugins() const
{
    return (int)m_plugins.size();
}

otPlugin* otModule::getPlugin(int i)
{
    if (i >= m_plugins.size()) { return nullptr; }
    return &m_plugins[i];
}
