#ifndef otPlugin_h
#define otPlugin_h

class otpModule;
class otpInstance;
class otpParam;


class otpParam
{
public:
    otpParam(otpInstance *parent = nullptr);
    ~otpParam();

    otpInstance*        getInstance() const;

    otpParamType        getType() const;
    const char*         getName() const;
    const char*         getNote() const;

    int                 getLength() const;
    void                getValue(void *dst) const;
    void                setValue(const void *data, int len=1); // len is relevant only if type is tonecurve

    otpParamInfo&       getRawInfo();

private:
    otpInstance *m_parent;
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


class otpPort
{
public:
    otpPort(otpInstance *parent, const char *name);

    otpInstance*    getInstance() const;
    const char*     getName() const;
    void            setImage(otpImage *img);
    ImageRGBAu8*    getImage() const;

private:
    otpInstance *m_parent;
    std::string m_name;
    ImageRGBAu8 *m_image;
};


class otpInstance
{
public:
    otpInstance(otpModule *module, toonz_plugin_probe_t *probe);
    ~otpInstance();

    const otpPluginInfo&    getPluginInfo() const;

    int                     getNumParams() const;
    otpParam*               getParam(int i);
    otpParam*               getParamByName(const char *name);

    int                     getNumPorts() const;
    otpPort*                getPort(int i);
    otpPort*                getPortByName(const char *name);

    void*                   getUserData() const;
    void                    setUsertData(void *v);

    void                    createDstImage(int width, int height);
    otpImage*               applyFx(double frames);

    // Body: [](otpParam *param){} -> void
    template<class Body>
    void eachParams(const Body& b)
    {
        for (auto& param : m_params) { b(param.get()); }
    }

public:
    // internal
    void            setParamInfo(toonz_param_page_t *pages, int num_pages);
    void            addPort(const char *name);
    double          getFrame() const;
    ImageRGBAu8*    getDstImage();

private:
    typedef std::unique_ptr<otpParam> otpParamPtr;
    typedef std::unique_ptr<otpPort> otpPortPtr;

    otpModule *m_module;
    toonz_plugin_probe_t *m_probe;
    otpPluginInfo m_info;

    std::vector<otpParamPtr> m_params;
    void *m_userdata;

    double m_frame;
    std::vector<otpPortPtr> m_ports;
    std::unique_ptr<ImageRGBAu8> m_dst_image;
    int m_canceled;
};


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

#endif // otPlugin_h

