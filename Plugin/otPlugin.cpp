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
    , m_data()
{
}

otParam::~otParam()
{
}

otParamType otParam::getType() const { return (otParamType)m_info.type; }
const char* otParam::getName() const { return m_info.name; }
const char* otParam::getNote() const { return m_info.note; }
const void* otParam::getValue() const { return &m_data; }

int otParam::getLength() const
{
    switch (getType()) {
    case otParamType_String: return (int)m_string.size();
    case otParamType_ToneCurve: return (int)m_tonecurve.size();
    }
    return 1;
}

void otParam::copyValue(void *dst) const
{
    if (!dst) { return; }

    switch (getType()) {
    case otParamType_Double:    *(otDoubleValue*)dst = m_data.double_v; break;
    case otParamType_Range:     *(otRangeValue*)dst = m_data.range_v; break;
    case otParamType_Pixel:     *(otPixelValue*)dst = m_data.pixel_v; break;
    case otParamType_Point:     *(otPointValue*)dst = m_data.point_v; break;
    case otParamType_Enum:      *(otEnumValue*)dst = m_data.enum_v; break;
    case otParamType_Int:       *(otIntValue*)dst = m_data.int_v; break;
    case otParamType_Bool:      *(otBoolValue*)dst = m_data.bool_v; break;
    case otParamType_Spectrum:  *(otSpectrumValue*)dst = m_data.spectrum_v; break;
    case otParamType_String:    memcpy(dst, m_string.c_str(), m_string.size()); break;
    case otParamType_ToneCurve: memcpy(dst, &m_tonecurve[0], sizeof(otToneCurveValue)*m_tonecurve.size()); break;
    }
}

void otParam::setValue(const void *src, int len)
{
    if (!src) { return; }

    switch (getType()) {
    case otParamType_Double:    m_data.double_v = *(otDoubleValue*)src; break;
    case otParamType_Range:     m_data.range_v = *(otRangeValue*)src; break;
    case otParamType_Pixel:     m_data.pixel_v = *(otPixelValue*)src; break;
    case otParamType_Point:     m_data.point_v = *(otPointValue*)src; break;
    case otParamType_Enum:      m_data.enum_v = *(otEnumValue*)src; break;
    case otParamType_Int:       m_data.int_v = *(otIntValue*)src; break;
    case otParamType_Bool:      m_data.bool_v = *(otBoolValue*)src; break;
    case otParamType_Spectrum:  m_data.spectrum_v = *(otSpectrumValue*)src; break;
    case otParamType_String:
        m_string = (const char*)src;
        break;
    case otParamType_ToneCurve:
        m_tonecurve.assign((otToneCurveValue*)src, (otToneCurveValue*)src + len);
        break;
    }
}

otParamInfo& otParam::getRawInfo() { return m_info; }
otParamValue& otParam::getRawValue() { return m_data; }



otContext::otContext()
    : dst()
    , is_canceled(0)

{
    rs = {
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
        &is_canceled
    };
}


otPlugin::otPlugin(toonz_plugin_probe_t *probe)
    : m_probe(probe)
    , m_userdata()
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
    otParamValue& val = dst.getRawValue();
    info.name = desc.key;
    info.note = desc.note;
    info.type = (otParamType)desc.traits_tag;

    ;

    switch (info.type) {
    case otParamType_Double:    dst.setValue(&desc.traits.d.def); break;
    case otParamType_Range:     dst.setValue(&desc.traits.rd.def); break;
    case otParamType_Pixel:     dst.setValue(&desc.traits.c.def); break;
    case otParamType_Point:     dst.setValue(&desc.traits.p.def); break;
    case otParamType_Enum:      dst.setValue(&desc.traits.e.def); break;
    case otParamType_Int:       dst.setValue(&desc.traits.i.def); break;
    case otParamType_Bool:      dst.setValue(&desc.traits.b.def); break;
    case otParamType_Spectrum:  dst.setValue(&desc.traits.g.def); break;
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

otImage* otPlugin::applyFx(otParamValue *params, otImage *src, double frame)
{
    otContext ctx;
    m_probe->handler->do_compute(this, &ctx.rs, frame, src);

    return ctx.dst;
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
