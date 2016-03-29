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

class otModule;
class otPlugin;
#ifndef otImpl
    class otImage;
#endif // otImpl

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
    int value[4];
};
struct otPointValue
{
    double x, y;
};
struct otSpectrumValue
{
    double w;
    double color[4];
};
struct otToneCurveValue
{
    double x, y;
    int channel;
    int interp;
};

union otParamValue
{
    otDoubleValue    double_v;
    otIntValue       int_v;
    otBoolValue      bool_v;
    otEnumValue      enum_v;
    otRangeValue     range_v;
    otPixelValue     pixel_v;
    otPointValue     point_v;
    otSpectrumValue  spectrum_v;
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
otCLinkage otExport void        otGetParamInfo(otPlugin *plugin, otParamInfo *pinfo);
otCLinkage otExport otImage*    otApplyFx(otPlugin *plugin, otParamValue *pdata, otImage *src, double frame);

#endif // OpenToonzPluginForUnity_h
