#ifndef OpenToonzPluginForUnity_h
#define OpenToonzPluginForUnity_h

#define otpCLinkage extern "C"
#ifdef _WIN32
    #ifndef otpStaticLink
        #ifdef otpImpl
            #define otpExport __declspec(dllexport)
        #else
            #define otpExport __declspec(dllimport)
        #endif
    #else
        #define otpExport
    #endif
#else
    #define otpExport
#endif

#ifndef otpImpl
    class otpImage;
#endif // otImpl
class otpModule;
class otpInstance;
class otpParam;
class otpPort;

struct otpImageData
{
    int width;
    int height;
    void *data;
};

enum otpParamType {
    otParamType_Double,
    otParamType_Range,
    otParamType_Pixel,
    otParamType_Point,
    otParamType_Enum,
    otParamType_Int,
    otParamType_Bool,
    otParamType_Spectrum,
    otParamType_String,
    otParamType_ToneCurve,
    otParamType_Unknown,
};

struct otpPluginInfo
{
    const char *name;
    const char *vendor;
    const char *note;
    int version_major, version_minor;
};

struct otpParamInfo
{
    const char *name;
    const char *note;
    otpParamType type;

    otpParamInfo(
        const char *name_ = nullptr,
        const char *note_ = nullptr,
        otpParamType type_ = otParamType_Unknown)
        : name(name_), note(note_), type(type_)
    {}
};

struct otpPortInfo
{
    const char *name;
};


struct otpIntValue
{
    int value;
};
typedef otpIntValue otpBoolValue;
typedef otpIntValue otpEnumValue;
struct otpDoubleValue
{
    double value;
};
struct otpStringValue
{
    const char *value;
};
struct otpRangeValue
{
    double min, max;
};
struct otpPixelValue
{
    int c0, c1, c2, m;
};
struct otpPointValue
{
    double x, y;
};
struct otpSpectrumValue
{
    double w;
    double c0, c1, c2, m;
};
struct otpToneCurveValue
{
    double x, y;
    int channel;
    int interp;
};



otpCLinkage otpExport otpImage*     otpCreateImage(int width, int height);
otpCLinkage otpExport otpImage*     otpCreateIntrusiveImage(void *data, int width, int height);
otpCLinkage otpExport void          otpDestroyImage(otpImage *img);
otpCLinkage otpExport void          otpGetImageResize(otpImage *img, int width, int height);
otpCLinkage otpExport void          otpGetImageData(otpImage *img, otpImageData *data);


otpCLinkage otpExport otpModule*    otpLoadModule(const char *path);
otpCLinkage otpExport void          otpUnloadModule(otpModule *mod);

otpCLinkage otpExport int           otpGetNumPlugins(otpModule *mod);
otpCLinkage otpExport void          otpGetPluginInfo(otpModule *inst, int i, otpPluginInfo *dst);
otpCLinkage otpExport otpInstance*  otpCreateInstance(otpModule *mod, int i);
otpCLinkage otpExport void          otpDestroyInstance(otpInstance *inst);

otpCLinkage otpExport int           otpGetNumPorts(otpInstance *inst);
otpCLinkage otpExport otpPort*      otpGetPort(otpInstance *inst, int i);
otpCLinkage otpExport otpPort*      otpGetPortByName(otpInstance *inst, const char *name);
otpCLinkage otpExport void          otpGetPortInfo(otpPort *port, otpPortInfo *info);
otpCLinkage otpExport void          otpSetInput(otpPort *port, otpImage *src);

otpCLinkage otpExport int           otpGetNumParams(otpInstance *inst);
otpCLinkage otpExport otpParam*     otpGetParam(otpInstance *inst, int i);
otpCLinkage otpExport otpParam*     otpGetParamByName(otpInstance *inst, const char *name);
otpCLinkage otpExport void          otpGetParamInfo(otpParam *param, otpParamInfo *pinfo);
// return count of elements if param type is string or tonecurve. otherwise 1
otpCLinkage otpExport int           otpGetParamLength(otpParam *param);
otpCLinkage otpExport void          otpGetParamValue(otpParam *param, void *dst);
otpCLinkage otpExport void          otpSetParamValue(otpParam *param, const void *src);

otpCLinkage otpExport void          otpBeginRender(otpInstance *inst, int width, int height);
// return result image. user *should not* otpImageDestroy() returned image
otpCLinkage otpExport otpImage*     otpRender(otpInstance *inst, double frame);
otpCLinkage otpExport void          otpEndRender(otpInstance *inst);

#endif // OpenToonzPluginForUnity_h
