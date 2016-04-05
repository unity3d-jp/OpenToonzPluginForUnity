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
    otParamType_Color,
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

struct otpPortInfo
{
    const char *name;
};


struct otpIntParamValue
{
    int value;
};
typedef otpIntParamValue otpBoolParamValue;
typedef otpIntParamValue otpEnumParamValue;
struct otpDoubleParamValue
{
    double value;
};
struct otpRangeParamValue
{
    double a, b;
};
struct otpColorParamValue
{
    int c0, c1, c2, m;
};
struct otpPointParamValue
{
    double x, y;
};
struct otpStringParamValue
{
    char *value;
};
struct otpSpectrumParamValue
{
    double w;
    double c0, c1, c2, m;
};
struct otpToneCurveParamValue
{
    double x, y;
    int channel;
    int interp;
};


struct otpIntParamTraits
{
    typedef int value_type;
    int def, min, max;
};
struct otpEnumParamTraits
{
    typedef int value_type;
    int def;
    int num;
    const char **names;
};
struct otpBoolParamTraits
{
    typedef int value_type;
    int def;
};
struct otpDoubleParamTraits
{
    typedef double value_type;
    double def, min, max;
};
struct otpRangeParamTraits
{
    typedef otpRangeParamValue value_type;
    otpRangeParamValue def, minmax;
};
struct otpColorParamTraits
{
    typedef otpColorParamValue value_type;
    otpColorParamValue def;
};
struct otpPointParamTraits
{
    typedef otpPointParamValue value_type;
    otpPointParamValue def, min, max;
};
struct otpStringParamTraits
{
    const char *value;
};
struct otpSpectrumParamTraits
{
    typedef otpSpectrumParamValue value_type;
    // todo
};
struct otpToneCurveParamTraits
{
    typedef otpToneCurveParamValue value_type;
};


struct otpParamInfo
{
    const char *name;
    const char *note;
    otpParamType type;
};



otpCLinkage otpExport otpImage*     otpCreateImage();
otpCLinkage otpExport otpImage*     otpCreateIntrusiveImage(void *data, int width, int height);
otpCLinkage otpExport void          otpDestroyImage(otpImage *img);
otpCLinkage otpExport void          otpResizeImage(otpImage *img, int width, int height);
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
otpCLinkage otpExport void          otpGetPortInfo(otpPort *port, otpPortInfo *dst);
otpCLinkage otpExport void          otpSetInput(otpPort *port, otpImage *src);

otpCLinkage otpExport int           otpGetNumParams(otpInstance *inst);
otpCLinkage otpExport otpParam*     otpGetParam(otpInstance *inst, int i);
otpCLinkage otpExport otpParam*     otpGetParamByName(otpInstance *inst, const char *name);
otpCLinkage otpExport void          otpGetParamInfo(otpParam *param, otpParamInfo *dst);
otpCLinkage otpExport void          otpGetParamTraits(otpParam *param, void *dst);
// return count of elements if param type is string or tonecurve. otherwise 1
otpCLinkage otpExport int           otpGetParamLength(otpParam *param);
otpCLinkage otpExport void          otpGetParamValue(otpParam *param, void *dst);
otpCLinkage otpExport void          otpSetParamValue(otpParam *param, const void *src);

// return result image. user *should not* otpImageDestroy() returned image
otpCLinkage otpExport otpImage*     otpGetDstImage(otpInstance *inst);

otpCLinkage otpExport void          otpBeginRender(otpInstance *inst, int width, int height);
otpCLinkage otpExport bool          otpRender(otpInstance *inst, double frame);
otpCLinkage otpExport void          otpEndRender(otpInstance *inst);

#endif // OpenToonzPluginForUnity_h
