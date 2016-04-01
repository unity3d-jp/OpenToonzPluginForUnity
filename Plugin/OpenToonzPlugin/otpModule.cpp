#include "pch.h"
#include "Foundation.h"
using namespace utj;

#include "OpenToonzPlugin.h"
#include "otpModule.h"
#include "otpInstance.h"
#include "otpPort.h"
#include "otpParam.h"


typedef int(*toonz_plugin_init_t)(toonz_host_interface_t *hostif);
typedef void(*toonz_plugin_exit_t)();
extern toonz_host_interface_t g_toonz_host_interface;

otpPluginInfo otpProbeToInfo(toonz_plugin_probe_t *probe)
{
    otpPluginInfo r;
    r.name = probe->name;
    r.vendor = probe->vendor;
    r.note = probe->note;
    r.version_major = probe->ver.major;
    r.version_minor = probe->ver.minor;
    return r;
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
    return (int)std::distance(m_probes->begin, m_probes->end);
}

otpPluginInfo otpModule::getPluginInfo(int i)
{
    return otpProbeToInfo(m_probes->begin + i);
}

otpInstance* otpModule::createPluginInstance(int i)
{
    return new otpInstance(this, m_probes->begin + i);
}
