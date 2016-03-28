#include <cstdio>
#include <vector>
#include "../OpenToonzPlugin.h"

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("usage: %s plugin_path plugin_index=0\n", argv[0]);
        return 0;
    }

    const char *module_path = argv[1];
    int plugin_index = 0;
    if (argc >=3) {
        sscanf(argv[2], "%d", &plugin_index);
    }

    otModule *module = otLoad(module_path);
    if (!module) {
        printf("failed to load %s\n", module_path);
    }

    otPlugin *plugin = otGetPlugin(module, plugin_index);
    if (!plugin) {
        printf("failed to get plugin %d\n", plugin_index);
    }

    {
        otPluginInfo info;
        otGetPluginInfo(plugin, &info);
        printf("plugin name: %s\n", info.name);
        printf("plugin vendor: %s\n", info.vendor);
        printf("plugin note: %s\n", info.note);
        printf("plugin version: %d.%d\n", info.version_major, info.version_minor);
    }

    std::vector<otParamInfo> param_info;
    param_info.resize(otGetNumParams(plugin));
    if (!param_info.empty()) {
        otGetParamInfo(plugin, &param_info[0]);
        for (int i = 0; i < (int)param_info.size(); ++i) {
            printf("plugin param %d: %s\n", i, param_info[i].name);
        }
    }

    // todo


    otUnload(module);
}
