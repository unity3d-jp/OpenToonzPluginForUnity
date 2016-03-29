#ifndef OpenToonzPluginForUnity_h
#define OpenToonzPluginForUnity_h

#define otCLinkage extern "C"
#ifdef _WIN32
    #ifndef otStaticLink
        #ifdef otImpl
            #define otExport __declspec(dllexport)
        #else
            #define otExport __declspec(dllimport)
        #endif
    #else
        #define otExport
    #endif
#else
    #define otExport
#endif

#ifndef otImpl
    class otImage;
#endif // otImpl
class otModule;
class otPlugin;
class otParam;

struct otImageData
{
    int width;
    int height;
    void *data;
};

enum otParamType {
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

struct otParamInfo
{
    const char *name;
    const char *note;
    otParamType type;

    otParamInfo(
        const char *name_ = nullptr,
        const char *note_ = nullptr,
        otParamType type_ = otParamType_Unknown)
        : name(name_), note(note_), type(type_)
    {}
};


struct otIntValue
{
    int value;
};
typedef otIntValue otBoolValue;
typedef otIntValue otEnumValue;
struct otDoubleValue
{
    double value;
};
struct otStringValue
{
    const char *value;
};
struct otRangeValue
{
    double min, max;
};
struct otPixelValue
{
    int c0, c1, c2, m;
};
struct otPointValue
{
    double x, y;
};
struct otSpectrumValue
{
    double w;
    double c0, c1, c2, m;
};
struct otToneCurveValue
{
    double x, y;
    int channel;
    int interp;
};


struct otPluginInfo
{
    const char *name;
    const char *vendor;
    const char *note;
    int version_major, version_minor;
};


otCLinkage otExport otImage*    otImageCreate(int width, int height);
otCLinkage otExport otImage*    otImageCreateIntrusive(void *data, int width, int height);
otCLinkage otExport void        otImageDestroy(otImage *img);
otCLinkage otExport void        otImageGetData(otImage *img, otImageData *data);


otCLinkage otExport otModule*   otLoad(const char *path);
otCLinkage otExport void        otUnload(otModule *mod);

otCLinkage otExport int         otGetNumPlugins(otModule *mod);
otCLinkage otExport otPlugin*   otGetPlugin(otModule *mod, int i);

otCLinkage otExport void        otGetPluginInfo(otPlugin *plugin, otPluginInfo *dst);
otCLinkage otExport int         otGetNumParams(otPlugin *plugin);
otCLinkage otExport void        otGetParamInfo(otPlugin *plugin, int i, otParamInfo *pinfo);
// return count of elements if param type is string or tonecurve. otherwise 1
otCLinkage otExport int         otGetParamLength(otPlugin *plugin, int i);
otCLinkage otExport void        otGetParamValue(otPlugin *plugin, int i, void *dst);
otCLinkage otExport void        otSetParamValue(otPlugin *plugin, int i, const void *src);

// return result image. user should otImageDestroy() returned image
otCLinkage otExport otImage*    otApplyFx(otPlugin *plugin, otImage *src, double frame);

#endif // OpenToonzPluginForUnity_h
