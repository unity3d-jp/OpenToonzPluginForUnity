#include "pch.h"
#include "Foundation.h"
using namespace utj;

#include "OpenToonzPlugin.h"
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
    if (!img || !data) { return; }
    data->width = img->getWidth();
    data->height = img->getHeight();
    data->data = img->getData();
}


utjCLinkage utjExport otpModule* otpLoadModule(const char *path)
{
    auto i = g_plugins.find(path);
    if (i != g_plugins.end()) {
        return i->second;
    }

    {
        auto dir_file = SplitDirFile(path);
        DLLAddSearchPath(dir_file.first.c_str());
    }

    otpModule *inst = new otpModule();
    if (!inst->load(path)) {
        delete inst;
        return nullptr;
    }
    g_plugins[path] = inst;
    return inst;
}

utjCLinkage utjExport void otpUnloadModule(otpModule *inst)
{
    // do nothing
}


utjCLinkage utjExport int otpGetNumPlugins(otpModule *mod)
{
    if (!mod) { return 0; }
    return mod->getNumPlugins();
}

otpCLinkage otpExport void otpGetPluginInfo(otpModule *mod, int i, otpPluginInfo *dst)
{
    if (!mod || !dst) { return; }
    *dst = mod->getPluginInfo(i);
}

utjCLinkage utjExport otpInstance* otpCreateInstance(otpModule *mod, int i)
{
    if (!mod) { return nullptr; }
    return mod->createPluginInstance(i);
}

otpCLinkage otpExport void otpDestroyInstance(otpInstance *inst)
{
    delete inst;
}


otpCLinkage otpExport int otpGetNumPorts(otpInstance *inst)
{
    if (!inst) { return 0; }
    return inst->getNumPorts();
}

otpCLinkage otpExport otpPort* otpGetPort(otpInstance *inst, int i)
{
    if (!inst) { return nullptr; }
    return inst->getPort(i);
}

otpCLinkage otpExport otpPort* otpGetPortByName(otpInstance *inst, const char *name)
{
    if (!inst) { return nullptr; }
    return inst->getPortByName(name);
}

otpCLinkage otpExport void otpGetPortInfo(otpPort *port, otpPortInfo *info)
{
    if (!port) { return; }
    info->name = port->getName();
}

otpCLinkage otpExport void otpSetInput(otpPort *port, otpImage *src)
{
    if (!port) { return; }
    port->setImage(src);
}


utjCLinkage utjExport int otpGetNumParams(otpInstance *inst)
{
    if (!inst) { return 0; }
    return inst->getNumParams();
}

otpCLinkage otpExport otpParam* otpGetParam(otpInstance *inst, int i)
{
    if (!inst) { return 0; }
    return inst->getParam(i);
}

otpCLinkage otpExport otpParam* otpGetParamByName(otpInstance *inst, const char *name)
{
    if (!inst) { return 0; }
    return inst->getParamByName(name);
}



otpCLinkage otpExport void otpGetParamInfo(otpParam *param, otpParamInfo *pinfo)
{
    if (!param || !pinfo) { return; }
    *pinfo = param->getRawInfo();
}

otpCLinkage otpExport int otpGetParamLength(otpParam *param)
{
    if (!param) { return 0; }
    return param->getLength();
}

otpCLinkage otpExport void otpGetParamValue(otpParam *param, void *dst)
{
    if (!param) { return; }
    param->getValue(dst);
}

otpCLinkage otpExport void otpSetParamValue(otpParam *param, const void *src)
{
    if (!param) { return; }
    param->setValue(src);
}


otpCLinkage otpExport void otpCreateDstImage(otpInstance *inst, int width, int height)
{
    inst->createDstImage(width, height);
}

utjCLinkage utjExport otpImage* otpApplyFx(otpInstance *node, double frame)
{
    if (!node) { return nullptr; }
    return node->applyFx(frame);
}
