#include "pch.h"
#include "OpenToonzPlugin.h"
#include "fcFoundation.h"
#include "otPlugin.h"


std::map<std::string, otModule*> g_plugins;


fcCLinkage fcExport otModule* otLoad(const char *path)
{
    auto i = g_plugins.find(path);
    if (i != g_plugins.end()) {
        return i->second;
    }

    otModule *plugin = new otModule();
    if (!plugin->load(path)) {
        delete plugin;
        return nullptr;
    }
    g_plugins[path] = plugin;
    return plugin;
}

fcCLinkage fcExport void otUnload(otModule *plugin)
{
    // do nothing
}


fcCLinkage fcExport int otGetNumPlugins(otModule *mod)
{
    if (!mod) { return 0; }
    return mod->getNumPlugins();
}

fcCLinkage fcExport otPlugin* otGetPlugin(otModule *mod, int i)
{
    if (!mod) { return nullptr; }
    return mod->getPlugin(i);
}


otCLinkage otExport void otGetPluginInfo(otPlugin *plugin, otPluginInfo *dst)
{
    if (!plugin || !dst) { return; }
    *dst = plugin->getPluginInfo();
}

fcCLinkage fcExport int otGetNumParams(otPlugin *plugin)
{
    if (!plugin) { return 0; }
    return plugin->getNumParams();
}

fcCLinkage fcExport void otGetParamInfo(otPlugin *plugin, otParamInfo *pinfo)
{
    if (!plugin || !pinfo) { return; }
    memcpy(pinfo, plugin->getParamInfo(), sizeof(otParamInfo)*plugin->getNumParams());
}

fcCLinkage fcExport void otApplyFx(otPlugin *plugin, otParamData *pdata, void *pixels, int width, int height)
{
    if (!plugin) { return; }
    plugin->applyFx(pdata, pixels, width, height);
}
