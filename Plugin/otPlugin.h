#ifndef otPlugin_h
#define otPlugin_h

class otPlugin
{
public:
    otPlugin(toonz_plugin_probe_t *probe);
    ~otPlugin();

    const otPluginInfo& getPluginInfo() const;

    int         getNumParams() const;
    void        getParamInfo(otParamInfo *dst);

    void        applyEffect(otParamData *params, void *pixels, int width, int height);

private:
    toonz_plugin_probe_t *m_probe;
    toonz_param_group_t *m_param_info;
    otPluginInfo m_info;
};


class otModule
{
public:
    otModule();
    ~otModule();

    bool        load(const char *path);
    void        unload();

    int         getNumPlugins() const;
    otPlugin*   getPlugin(int i);

private:
    module_t m_module;
    std::vector<otPlugin> m_plugins;
};

#endif // otPlugin_h

