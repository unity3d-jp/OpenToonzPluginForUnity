#include <cstdio>
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
}
