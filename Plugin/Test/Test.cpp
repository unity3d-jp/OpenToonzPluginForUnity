#include <cstdio>
#include <vector>
#include "../OpenToonzPlugin/OpenToonzPlugin.h"
#pragma warning(disable:4996)


int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("usage: %s plugin_path plugin_index(default:0)\n", argv[0]);
        return 0;
    }

    const char *module_path = argv[1];
    int plugin_index = 0;
    if (argc >=3) {
        sscanf(argv[2], "%d", &plugin_index);
    }

    otpModule *module = otpLoadModule(module_path);
    if (!module) {
        printf("failed to load %s\n", module_path);
        return 1;
    }

    otpInstance *inst = otpCreateInstance(module, plugin_index);
    if (!inst) {
        printf("failed to get inst %d\n", plugin_index);
        return 1;
    }

    {
        printf("plugin info:\n");
        otpPluginInfo info;
        otpGetPluginInfo(module, plugin_index, &info);
        printf("  name: %s\n", info.name);
        printf("  vendor: %s\n", info.vendor);
        printf("  note: %s\n", info.note);
        printf("  version: %d.%d\n", info.version_major, info.version_minor);
        printf("\n");
    }

    {
        printf("port info:\n");
        int n = otpGetNumPorts(inst);
        otpPortInfo info;
        for (int i = 0; i < n; ++i) {
            otpPort *port = otpGetPort(inst, i);
            otpGetPortInfo(port, &info);
            printf("  port %d: %s\n", i, info.name);
        }
        printf("\n");
    }

    {
        printf("param info:\n");
        int n = otpGetNumParams(inst);
        otpParamInfo pinfo;
        for (int i = 0; i < n; ++i) {
            otpParam *param = otpGetParam(inst, i);
            otpGetParamInfo(param, &pinfo);
            printf("  param %d: %s\n", i, pinfo.name);
        }
        printf("\n");
    }


    otpImage *src = otpCreateImage();
    otpResizeImage(src, 1024, 512);

    otpImageData src_data;
    {
        otpGetImageData(src, &src_data);
        printf("src image: %p\n", src_data.data);

        // create image
        int n = src_data.width * src_data.height;
        const int block = 4;
        uint32_t *pixels = (uint32_t*)src_data.data;
        for (int i = 0; i < n / block; ++i) {
            uint32_t *dst = pixels + (block * i);
            memset(dst, i % 2 == 0 ? 0xff : 0x00, block * 4);
        }

        otpPort *port = otpGetPort(inst, 0);
        otpSetInput(port, src);
    }

    otpBeginRender(inst, 1024, 512);

    otpRender(inst, 0.0);
    otpImage *dst = otpGetDstImage(inst);
    otpImageData dst_data;
    {
        otpGetImageData(dst, &dst_data);
        printf("dst image: %p\n", dst_data.data);
    }

    otpEndRender(inst);


    otpDestroyImage(src);

    otpDestroyInstance(inst);
    otpUnloadModule(module);
}
