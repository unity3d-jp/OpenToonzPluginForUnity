#include "pch.h"
#include "OpenToonzPlugin.h"
#include "fcFoundation.h"
#include "otPlugin.h"

typedef int(*toonz_plugin_init_t)(toonz_host_interface_t *hostif);
typedef void(*toonz_plugin_exit_t)();


extern toonz_host_interface_t g_toonz_host_interface;






otPlugin::otPlugin(toonz_plugin_probe_t *probe)
    : m_probe(probe)
    , m_param_info()
{
    m_info.name = m_probe->name;
    m_info.vendor = m_probe->vendor;
    m_info.note = m_probe->note;
    m_info.version_major = m_probe->ver.major;
    m_info.version_minor = m_probe->ver.minor;
}

otPlugin::~otPlugin()
{
}

const otPluginInfo& otPlugin::getPluginInfo() const
{
    return m_info;
}

int otPlugin::getNumParams() const
{
    if (!m_param_info) { return 0; }

    return m_param_info->num;
}

void otPlugin::getParamInfo(otParamInfo *dst)
{
    if (!m_param_info) { return; }

    int n = m_param_info->num;
    for (int i = 0; i < n; ++i) {
        // todo
    }
}

void otPlugin::applyEffect(otParamData *params, void *pixels, int width, int height)
{
    // todo
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

    for (auto p = probes->begin; p < probes->end; ++p) {
        m_plugins.emplace_back(otPlugin(p));
    }

    // call toonz_plugin_init
    auto init_f = (toonz_plugin_init_t)DLLGetSymbol(m_module, "toonz_plugin_init");
    if (init_f) {
        init_f(&g_toonz_host_interface);
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
