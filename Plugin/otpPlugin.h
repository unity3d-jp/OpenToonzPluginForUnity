#ifndef otPlugin_h
#define otPlugin_h


class otpParam
{
public:
    otpParam();
    otpParam(const otpParamInfo& info);
    ~otpParam();

    otpParamType         getType() const;
    const char*         getName() const;
    const char*         getNote() const;

    int                 getLength() const;
    void                getValue(void *dst) const;
    void                setValue(const void *data, int len=1); // len is relevant only if type is tonecurve

    otpParamInfo&        getRawInfo();

private:
    otpParamInfo m_info;
    union
    {
        otpDoubleValue    double_v;
        otpIntValue       int_v;
        otpBoolValue      bool_v;
        otpEnumValue      enum_v;
        otpRangeValue     range_v;
        otpPixelValue     pixel_v;
        otpPointValue     point_v;
        otpSpectrumValue  spectrum_v;
    } m_value;
    std::string m_string;
    std::vector<otpToneCurveValue> m_tonecurve;
};


class otpPlugin
{
public:
    otpPlugin(toonz_plugin_probe_t *probe);
    ~otpPlugin();

    const otpPluginInfo& getPluginInfo() const;

    int                 getNumParams() const;
    otpParam*            getParam(int i);
    otpParam*            getParamByName(const char *name);

    void*               getUserData() const;
    void                setUsertData(void *v);

    otpImage*            applyFx(otpImage *src, double frames);

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
    otpPluginInfo m_info;
    std::vector<otpParam> m_params;
    void *m_userdata;
    ImageRGBAu8 *m_dst_image;
    int m_canceled;
};


class otpModule
{
public:
    otpModule();
    ~otpModule();

    bool        load(const char *path);
    void        unload();

    int         getNumPlugins() const;
    otpPlugin*   getPlugin(int i);

private:
    module_t m_module;
    std::vector<otpPlugin> m_plugins;
};

#endif // otPlugin_h

