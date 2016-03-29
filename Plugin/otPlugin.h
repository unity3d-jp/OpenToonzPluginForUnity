#ifndef otPlugin_h
#define otPlugin_h


class otParam
{
public:
    otParam();
    otParam(const otParamInfo& info);
    ~otParam();

    otParamType         getType() const;
    const char*         getName() const;
    const char*         getNote() const;

    int                 getLength() const;
    void                getValue(void *dst) const;
    void                setValue(const void *data, int len=1); // len is relevant only if type is tonecurve

    otParamInfo&        getRawInfo();

private:
    otParamInfo m_info;
    union
    {
        otDoubleValue    double_v;
        otIntValue       int_v;
        otBoolValue      bool_v;
        otEnumValue      enum_v;
        otRangeValue     range_v;
        otPixelValue     pixel_v;
        otPointValue     point_v;
        otSpectrumValue  spectrum_v;
    } m_value;
    std::string m_string;
    std::vector<otToneCurveValue> m_tonecurve;
};


class otPlugin
{
public:
    otPlugin(toonz_plugin_probe_t *probe);
    ~otPlugin();

    const otPluginInfo& getPluginInfo() const;

    int                 getNumParams() const;
    otParam*            getParam(int i);
    otParam*            getParamByName(const char *name);

    void*               getUserData() const;
    void                setUsertData(void *v);

    otImage*            applyFx(otImage *src, double frames);

    template<class Body>
    void eachParams(const Body& b)
    {
        for (auto& param : m_params) { b(param); }
    }

public:
    // internal
    void setParamInfo(toonz_param_page_t *pages, int num_pages);
    void setDstImage(ImageRGBAu8 *img);

private:
    toonz_plugin_probe_t *m_probe;
    otPluginInfo m_info;
    std::vector<otParam> m_params;
    void *m_userdata;
    ImageRGBAu8 *m_dst_image;
    int m_canceled;
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

