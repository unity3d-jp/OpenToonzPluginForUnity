#ifndef otPlugin_h
#define otPlugin_h

class otpModule;
class otpInstance;
class otpPort;
class otpParam;


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

#endif // otPlugin_h

