#include "pch.h"
#include "OpenToonzPluginForUnity.h"
#include "fcFoundation.h"
#include "otPlugin.h"


std::map<std::string, otPlugin*> g_plugins;


otPlugin* otLoadPlugin(const char *path)
{
    auto i = g_plugins.find(path);
    if (i != g_plugins.end()) {
        return i->second;
    }

    otPlugin *plugin = new otPlugin();
    if (!plugin->load(path)) {
        delete plugin;
        return nullptr;
    }
    g_plugins[path] = plugin;
    return plugin;
}
void otUnloadPlugin(otPlugin *plugin)
{
    // do nothing
}

int otGetNumParams(otPlugin *plugin)
{
    if (!plugin) { return 0; }
    return plugin->getNumParams();
}
void otGetParamInfo(otPlugin *plugin, otParamInfo *pinfo)
{
    if (!plugin) { return; }
    plugin->getParamInfo(pinfo);
}
void otApplyEffect(otPlugin *plugin, otParamData *pdata, const void *src_pixels, void *dst_pixels)
{
    if (!plugin) { return; }
    plugin->applyEffect(pdata, src_pixels, dst_pixels);
}
