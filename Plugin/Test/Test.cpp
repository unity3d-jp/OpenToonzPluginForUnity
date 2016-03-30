#include <cstdio>
#include <vector>
#include "../OpenToonzPlugin.h"
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

    otpModule *module = otpLoad(module_path);
    if (!module) {
        printf("failed to load %s\n", module_path);
        return 1;
    }

    otpPlugin *plugin = otpGetPlugin(module, plugin_index);
    if (!plugin) {
        printf("failed to get plugin %d\n", plugin_index);
        return 1;
    }

    {
        otpPluginInfo info;
        otpGetPluginInfo(plugin, &info);
        printf("plugin name: %s\n", info.name);
        printf("plugin vendor: %s\n", info.vendor);
        printf("plugin note: %s\n", info.note);
        printf("plugin version: %d.%d\n", info.version_major, info.version_minor);
        printf("\n");
    }

    {
        int n = otpGetNumParams(plugin);
        otpParamInfo pinfo;
        for (int i = 0; i < n; ++i) {
            otpGetParamInfo(plugin, i, &pinfo);
            printf("plugin param %d: %s\n", i, pinfo.name);
        }
    }

    otpImage *src = otpImageCreate(128, 128);
    otpImage *dst = otpApplyFx(plugin, src, 0.0);

    otpImageDestroy(dst);
    otpImageDestroy(src);
    otpUnload(module);
}
