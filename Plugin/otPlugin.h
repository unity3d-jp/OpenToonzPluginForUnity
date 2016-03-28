#ifndef otPlugin_h
#define otPlugin_h

class otPlugin
{
public:
    otPlugin(toonz_plugin_probe_t *probe);
    ~otPlugin();

    const char* getName() const;
    const char* getVender() const;
    const char* getNote() const;
    int         getVersion() const;

    int         getNumParams() const;
    void        getParamInfo(otParamInfo *dst);

    void        applyEffect(otParamData *params, const void *src_pixels, void *dst_pixels);

private:
    toonz_plugin_probe_t *m_probe;
    toonz_param_group_t *m_param_info;
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

