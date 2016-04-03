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

    ImageRGBAu8*            getDstImage();

    void                    beginRender(int width, int height);
    bool                    render(double frames);
    void                    endRender();

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

private:
    typedef std::unique_ptr<otpParam>       otpParamPtr;
    typedef std::unique_ptr<otpPort>        otpPortPtr;
    typedef std::unique_ptr<ImageRGBAu8>    ImagePtr;

    otpModule                   *m_module;
    toonz_plugin_probe_t        *m_probe;
    otpPluginInfo               m_info;

    std::vector<otpPortPtr>     m_ports;
    std::vector<otpParamPtr>    m_params;
    ImagePtr                    m_dst_image;

    toonz_rendering_setting_t   m_rs;
    toonz_rect_t                m_rect;

    void                        *m_userdata;
    int                         m_base_width;
    int                         m_base_height;
    double                      m_frame;
    int                         m_canceled;
};

#endif // otPlugin_h

