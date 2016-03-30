#ifndef OpenToonzPluginForUnity_h
#define OpenToonzPluginForUnity_h

#define otpCLinkage extern "C"
#ifdef _WIN32
    #ifndef otStaticLink
        #ifdef otImpl
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

#ifndef otImpl
    class otpImage;
#endif // otImpl
class otpModule;
class otpPlugin;
class otpParam;

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


struct otpPluginInfo
{
    const char *name;
    const char *vendor;
    const char *note;
    int version_major, version_minor;
};


otpCLinkage otpExport otpImage*     otpImageCreate(int width, int height);
otpCLinkage otpExport otpImage*     otpImageCreateIntrusive(void *data, int width, int height);
otpCLinkage otpExport void          otpImageDestroy(otpImage *img);
otpCLinkage otpExport void          otpImageGetData(otpImage *img, otpImageData *data);


otpCLinkage otpExport otpModule*    otpLoad(const char *path);
otpCLinkage otpExport void          otpUnload(otpModule *mod);

otpCLinkage otpExport int           otpGetNumPlugins(otpModule *mod);
otpCLinkage otpExport otpPlugin*    otpGetPlugin(otpModule *mod, int i);

otpCLinkage otpExport void          otpGetPluginInfo(otpPlugin *plugin, otpPluginInfo *dst);
otpCLinkage otpExport int           otpGetNumParams(otpPlugin *plugin);
otpCLinkage otpExport void          otpGetParamInfo(otpPlugin *plugin, int i, otpParamInfo *pinfo);
// return count of elements if param type is string or tonecurve. otherwise 1
otpCLinkage otpExport int           otpGetParamLength(otpPlugin *plugin, int i);
otpCLinkage otpExport void          otpGetParamValue(otpPlugin *plugin, int i, void *dst);
otpCLinkage otpExport void          otpSetParamValue(otpPlugin *plugin, int i, const void *src);

// return result image. user should otImageDestroy() returned image
otpCLinkage otpExport otpImage*     otpApplyFx(otpPlugin *plugin, otpImage *src, double frame);

#endif // OpenToonzPluginForUnity_h
