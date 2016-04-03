#include "pch.h"
#include "Foundation.h"
using namespace utj;

#include "OpenToonzPlugin.h"
#include "otpModule.h"
#include "otpInstance.h"
#include "otpPort.h"
#include "otpParam.h"


std::map<std::string, otpModule*> g_plugins;


otpCLinkage otpExport otpImage* otpCreateImage()
{
    return new ImageRGBAu8();
}

otpCLinkage otpExport otpImage* otpCreateIntrusiveImage(void *data, int width, int height)
{
    return new ImageIntrusiveRGBAu8((RGBAu8*)data, width, height);
}

otpCLinkage otpExport void otpDestroyImage(otpImage *img)
{
    delete img;
}

otpCLinkage otpExport void otpResizeImage(otpImage *img, int width, int height)
{
    return img->resize(width, height);
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

otpCLinkage otpExport otpImage* otpGetDstImage(otpInstance *inst)
{
    if (!inst) { return nullptr; }
    return inst->getDstImage();
}

otpCLinkage otpExport void otpBeginRender(otpInstance *inst, int width, int height)
{
    if (!inst) { return; }
    inst->beginRender(width, height);
}
otpCLinkage otpExport bool otpRender(otpInstance *inst, double frame)
{
    if (!inst) { return nullptr; }
    return inst->render(frame);
}
otpCLinkage otpExport void otpEndRender(otpInstance *inst)
{
    if (!inst) { return; }
    inst->endRender();
}



#ifndef otpStaticLink
// deferred call APIs

#include "PluginAPI/IUnityGraphics.h"

otpCLinkage otpExport void  otpGuardBegin();
otpCLinkage otpExport void  otpGuardEnd();
otpCLinkage otpExport void  otpEraseDeferredCall(int id);
otpCLinkage otpExport int   otpRenderDeferred(otpInstance *inst, double frame, int id);
otpCLinkage otpExport UnityRenderingEvent   GetRenderEventFunc();

typedef std::function<void()> DeferredCall;
namespace {
    std::mutex g_deferred_calls_mutex;
    std::vector<DeferredCall> g_deferred_calls;
}

otpCLinkage otpExport void otpGuardBegin()
{
    g_deferred_calls_mutex.lock();
}

otpCLinkage otpExport void otpGuardEnd()
{
    g_deferred_calls_mutex.unlock();
}

otpCLinkage otpExport int otpAddDeferredCall(const DeferredCall& dc, int id)
{
    if (id <= 0) {
        // search empty object and return its position if found
        for (int i = 1; i < (int)g_deferred_calls.size(); ++i) {
            if (!g_deferred_calls[i]) {
                g_deferred_calls[i] = dc;
                return i;
            }
        }

        // 0th is "null" object
        if (g_deferred_calls.empty()) { g_deferred_calls.emplace_back(DeferredCall()); }

        // allocate new one
        g_deferred_calls.emplace_back(dc);
        return (int)g_deferred_calls.size() - 1;
    }
    else if (id < (int)g_deferred_calls.size()) {
        g_deferred_calls[id] = dc;
        return id;
    }
    else {
        utjDebugLog("should not be here");
        return 0;
    }
}

otpCLinkage otpExport void otpEraseDeferredCall(int id)
{
    if (id <= 0 || id >= (int)g_deferred_calls.size()) { return; }

    g_deferred_calls[id] = DeferredCall();
}

// **called from rendering thread**
otpCLinkage otpExport void otpCallDeferredCall(int id)
{
    std::unique_lock<std::mutex> l(g_deferred_calls_mutex);
    if (id <= 0 || id >= (int)g_deferred_calls.size()) { return; }

    auto& dc = g_deferred_calls[id];
    if (dc) { dc(); }
}

otpCLinkage otpExport int otpRenderDeferred(otpInstance *inst, double frame, int id)
{
    if (!inst) { return 0; }
    return otpAddDeferredCall([=]() {
        return inst->render(frame);
    }, id);
}

static void UNITY_INTERFACE_API UnityRenderEvent(int eventID)
{
    otpCallDeferredCall(eventID);
}
otpCLinkage otpExport UnityRenderingEvent GetRenderEventFunc()
{
    return UnityRenderEvent;
}
#endif // otpStaticLink
