#ifndef otPlugin_h
#define otPlugin_h

class otPlugin
{
public:
    otPlugin();
    ~otPlugin();

    bool        load(const char *path);

    const char* getName() const;
    const char* getVender() const;
    const char* getNote() const;
    int         getVersion() const;

    int         getNumParams() const;
    void        getParamInfo(otParamInfo *dst);

    void        applyEffect(otParamData *params, const void *src_pixels, void *dst_pixels);

private:
    module_t m_module;
    toonz_plugin_probe_t *m_probe;
    toonz_param_group_t *m_param_info;
};

#endif // otPlugin_h

