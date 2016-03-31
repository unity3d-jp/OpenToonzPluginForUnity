#ifdef _MSC_VER
    #pragma warning(disable: 4190)
    #define _CRT_SECURE_NO_WARNINGS
#endif

#include <algorithm>
#include <string>
#include <vector>
#include <deque>
#include <list>
#include <map>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <functional>
#include <fstream>
#include <sstream>
#include <filesystem>

//#include <opencv2/core/core.hpp>
//#include <opencv2/imgproc/imgproc.hpp>
#include <toonz_plugin.h>
#include <toonz_params.h>
#include <toonz_hostif.h>

#define utjImpl

#if defined(_WIN32)
    #define utjWindows
#elif defined(__APPLE__)
    #ifdef TARGET_OS_IPHONE
        #define utjiOS
    #else
        #define utjMac
    #endif
#elif defined(__ANDROID__)
    #define utjAndroid
#elif defined(__linux__)
    #define utjLinux
#endif

#ifdef _WIN32
    #define utjSupportOpenGL
    #define utjSupportD3D9
    #define utjSupportD3D11
#else
    #define utjSupportOpenGL
#endif

#define utjEnableLogging
#ifdef utjEnableLogging
    namespace utj { void DebugLogImpl(const char* fmt, ...); }
    #define utjDebugLog(...) utj::DebugLogImpl(__FUNCTION__ ": " __VA_ARGS__)
#else
    #define utjDebugLog(...)
#endif

#define utjCLinkage extern "C"
#ifdef _WIN32
    #ifndef utjStaticLink
        #ifdef utjImpl
            #define utjExport __declspec(dllexport)
        #else
            #define utjExport __declspec(dllimport)
        #endif
    #else
        #define utjExport
    #endif
#else
    #define utjExport
#endif

#ifdef utjStaticLink
    #define otpStaticLink
    #define twStaticLink
#endif 
#ifdef utjImpl
    #define otpImpl
    #define twImpl
#endif 
#ifdef utjMaster
    #define otpMaster
    #define twMaster
#endif 

namespace utj {
    class ImageBase;
}
typedef utj::ImageBase otpImage;
