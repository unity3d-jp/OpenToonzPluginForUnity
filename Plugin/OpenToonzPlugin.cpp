#include "pch.h"
#include "OpenToonzPlugin.h"
#include "fcFoundation.h"
#include "otpPlugin.h"


std::map<std::string, otpModule*> g_plugins;


otpCLinkage otpExport otpImage* otpCreateImage(int width, int height)
{
    return new ImageRGBAu8(width, height);
}

otpCLinkage otpExport otpImage* otpCreateIntrusiveImage(void *data, int width, int height)
{
    return new ImageIntrusiveRGBAu8((RGBAu8*)data, width, height);
}

otpCLinkage otpExport void otpDestroyImage(otpImage *img)
{
    delete img;
}

otpCLinkage otpExport void otpGetImageData(otpImage *img, otpImageData *data)
{
    data->width = img->getWidth();
    data->height = img->getHeight();
    data->data = img->getData();
}


fcCLinkage fcExport otpModule* otpLoadModule(const char *path)
{
    auto i = g_plugins.find(path);
    if (i != g_plugins.end()) {
        return i->second;
    }

    otpModule *inst = new otpModule();
    if (!inst->load(path)) {
        delete inst;
        return nullptr;
    }
    g_plugins[path] = inst;
    return inst;
}

fcCLinkage fcExport void otpUnloadModule(otpModule *inst)
{
    // do nothing
}


fcCLinkage fcExport int otpGetNumPlugins(otpModule *mod)
{
    if (!mod) { return 0; }
    return mod->getNumPlugins();
}

otpCLinkage otpExport void otpGetPluginInfo(otpModule *mod, int i, otpPluginInfo *dst)
{
    if (!mod || !dst) { return; }
    *dst = mod->getPluginInfo(i);
}

fcCLinkage fcExport otpInstance* otpCreateInstance(otpModule *mod, int i)
{
    if (!mod) { return nullptr; }
    return mod->createPluginInstance(i);
}

otpCLinkage otpExport void otpDestroyInstance(otpInstance *inst)
{
    delete inst;
}


fcCLinkage fcExport int otpGetNumParams(otpInstance *inst)
{
    if (!inst) { return 0; }
    return inst->getNumParams();
}

otpCLinkage otpExport void otpGetParamInfo(otpInstance *inst, int i, otpParamInfo *pinfo)
{
    if (!inst || !pinfo) { return; }
    *pinfo = inst->getParam(i)->getRawInfo();
}

otpCLinkage otpExport int otpGetParamLength(otpInstance *inst, int i)
{
    if (!inst) { return 0; }
    return inst->getParam(i)->getLength();
}

otpCLinkage otpExport void otpGetParamValue(otpInstance *inst, int i, void *dst)
{
    if (!inst) { return; }
    inst->getParam(i)->getValue(dst);
}

otpCLinkage otpExport void otpSetParamValue(otpInstance *inst, int i, const void *src)
{
    if (!inst) { return; }
    inst->getParam(i)->setValue(src);
}

fcCLinkage fcExport otpImage* otpApplyFx(otpInstance *inst, otpImage *src, double frame)
{
    if (!inst) { return nullptr; }
    return inst->applyFx(src, frame);
}
