#include "pch.h"
#include "OpenToonzPlugin.h"
#include "fcFoundation.h"
#include "otPlugin.h"


std::map<std::string, otModule*> g_plugins;


otCLinkage otExport otImage* otImageCreate(int width, int height)
{
    return new ImageRGBAu8(width, height);
}

otCLinkage otExport otImage* otImageCreateIntrusive(void *data, int width, int height)
{
    return new ImageIntrusiveRGBAu8((RGBAu8*)data, width, height);
}

otCLinkage otExport void otImageDestroy(otImage *img)
{
    delete img;
}

otCLinkage otExport void otImageGetData(otImage *img, otImageData *data)
{
    data->width = img->getWidth();
    data->height = img->getHeight();
    data->data = img->getData();
}


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
    int n = plugin->getNumParams();

    int i = 0;
    plugin->eachParams([&](otParam& param) {
        pinfo[i++] = param.getInfo();
    });
}

fcCLinkage fcExport void otApplyFx(otPlugin *plugin, otParamData *pdata, otImage *img, double frame)
{
    if (!plugin) { return; }
    plugin->applyFx(pdata, img, frame);
}
