#include "pch.h"
#include "OpenToonzPlugin.h"
#include "fcFoundation.h"
#include "otpPlugin.h"


std::map<std::string, otpModule*> g_plugins;


otpCLinkage otpExport otpImage* otpImageCreate(int width, int height)
{
    return new ImageRGBAu8(width, height);
}

otpCLinkage otpExport otpImage* otpImageCreateIntrusive(void *data, int width, int height)
{
    return new ImageIntrusiveRGBAu8((RGBAu8*)data, width, height);
}

otpCLinkage otpExport void otpImageDestroy(otpImage *img)
{
    delete img;
}

otpCLinkage otpExport void otpImageGetData(otpImage *img, otpImageData *data)
{
    data->width = img->getWidth();
    data->height = img->getHeight();
    data->data = img->getData();
}


fcCLinkage fcExport otpModule* otpLoad(const char *path)
{
    auto i = g_plugins.find(path);
    if (i != g_plugins.end()) {
        return i->second;
    }

    otpModule *plugin = new otpModule();
    if (!plugin->load(path)) {
        delete plugin;
        return nullptr;
    }
    g_plugins[path] = plugin;
    return plugin;
}

fcCLinkage fcExport void otpUnload(otpModule *plugin)
{
    // do nothing
}


fcCLinkage fcExport int otpGetNumPlugins(otpModule *mod)
{
    if (!mod) { return 0; }
    return mod->getNumPlugins();
}

fcCLinkage fcExport otpPlugin* otpGetPlugin(otpModule *mod, int i)
{
    if (!mod) { return nullptr; }
    return mod->getPlugin(i);
}


otpCLinkage otpExport void otpGetPluginInfo(otpPlugin *plugin, otpPluginInfo *dst)
{
    if (!plugin || !dst) { return; }
    *dst = plugin->getPluginInfo();
}

fcCLinkage fcExport int otpGetNumParams(otpPlugin *plugin)
{
    if (!plugin) { return 0; }
    return plugin->getNumParams();
}

otpCLinkage otpExport void otpGetParamInfo(otpPlugin *plugin, int i, otpParamInfo *pinfo)
{
    if (!plugin || !pinfo) { return; }
    *pinfo = plugin->getParam(i)->getRawInfo();
}

otpCLinkage otpExport int otpGetParamLength(otpPlugin *plugin, int i)
{
    if (!plugin) { return 0; }
    return plugin->getParam(i)->getLength();
}

otpCLinkage otpExport void otpGetParamValue(otpPlugin *plugin, int i, void *dst)
{
    if (!plugin) { return; }
    plugin->getParam(i)->getValue(dst);
}

otpCLinkage otpExport void otpSetParamValue(otpPlugin *plugin, int i, const void *src)
{
    if (!plugin) { return; }
    plugin->getParam(i)->setValue(src);
}

fcCLinkage fcExport otpImage* otpApplyFx(otpPlugin *plugin, otpImage *src, double frame)
{
    if (!plugin) { return nullptr; }
    return plugin->applyFx(src, frame);
}
