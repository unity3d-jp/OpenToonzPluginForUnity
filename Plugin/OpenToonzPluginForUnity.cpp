#include "pch.h"
#include "OpenToonzPluginForUnity.h"
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


fcCLinkage fcExport int otGetNumParams(otPlugin *plugin)
{
    if (!plugin) { return 0; }
    return plugin->getNumParams();
}

fcCLinkage fcExport void otGetParamInfo(otPlugin *plugin, otParamInfo *pinfo)
{
    if (!plugin) { return; }
    plugin->getParamInfo(pinfo);
}

fcCLinkage fcExport void otApplyEffect(otPlugin *plugin, otParamData *pdata, const void *src_pixels, void *dst_pixels)
{
    if (!plugin) { return; }
    plugin->applyEffect(pdata, src_pixels, dst_pixels);
}
