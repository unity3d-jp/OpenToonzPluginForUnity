#include "pch.h"
#include "OpenToonzPluginForUnity.h"
#include "fcFoundation.h"
#include "otPlugin.h"

typedef int(*toonz_plugin_init_t)(toonz_host_interface_t *hostif);
typedef void(*toonz_plugin_exit_t)();


static toonz_host_interface_t g_host_interface; // todo: implement this



otPlugin::otPlugin(toonz_plugin_probe_t *probe)
    : m_probe(probe)
    , m_param_info()
{
}

otPlugin::~otPlugin()
{
}

const char* otPlugin::getName() const   { return m_probe->name; }
const char* otPlugin::getVender() const { return m_probe->vendor; }
const char* otPlugin::getNote() const   { return m_probe->note; }
int         otPlugin::getVersion() const{ return m_probe->plugin_ver.major; }

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

void otPlugin::applyEffect(otParamData *params, const void *src_pixels, void *dst_pixels)
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
        init_f(&g_host_interface);
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

int         otModule::getNumPlugins() const { return (int)m_plugins.size(); }
otPlugin*   otModule::getPlugin(int i)      { return &m_plugins[i]; }
