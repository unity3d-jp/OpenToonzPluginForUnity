#ifndef otpModule_h
#define otpModule_h


class otpModule
{
public:
    otpModule();
    ~otpModule();

    bool            load(const char *path);
    void            unload();

    int             getNumPlugins() const;
    otpPluginInfo   getPluginInfo(int i);
    otpInstance*    createPluginInstance(int i);

private:
    module_t m_module;
    toonz_plugin_probe_list_t *m_probes;
};

#endif // otpModule_h
