#include "pch.h"
#include "Foundation.h"

#ifdef utjWindows
    #include <windows.h>
#else
    #include <dlfcn.h>
#endif

namespace utj {

void DebugLogImpl(const char* fmt, ...)
{
#ifdef utjWindows

    char buf[2048];
    va_list vl;
    va_start(vl, fmt);
    vsprintf(buf, fmt, vl);
    ::OutputDebugStringA(buf);
    va_end(vl);

#else // utjWindows

    char buf[2048];
    va_list vl;
    va_start(vl, fmt);
    vprintf(fmt, vl);
    va_end(vl);

#endif // utjWindows
}



#ifdef utjWindows

module_t DLLLoad(const char *path) { return ::LoadLibraryA(path); }
void DLLUnload(module_t mod) { ::FreeLibrary((HMODULE)mod); }
void* DLLGetSymbol(module_t mod, const char *name) { return ::GetProcAddress((HMODULE)mod, name); }

#else 

module_t DLLLoad(const char *path) { return ::dlopen(path, RTLD_GLOBAL); }
void DLLUnload(module_t mod) { ::dlclose(mod); }
void* DLLGetSymbol(module_t mod, const char *name) { return ::dlsym(mod, name); }

#endif

void DLLAddSearchPath(const char *path_to_add)
{
#ifdef utjWindows
    std::string path;
    path.resize(1024 * 64);

    DWORD ret = ::GetEnvironmentVariableA("PATH", &path[0], (DWORD)path.size());
    path.resize(ret);
    path += ";";
    path += path_to_add;
    ::SetEnvironmentVariableA("PATH", path.c_str());

#else 
    std::string path = getenv("LD_LIBRARY_PATH");
    path += ";";
    path += path_to_add;
    setenv("LD_LIBRARY_PATH", path.c_str(), 1);
#endif
}

const char* DLLGetDirectoryOfCurrentModule()
{
    static std::string s_path;
#ifdef utjWindows
    if (s_path.empty()) {
        char buf[MAX_PATH];
        HMODULE mod = 0;
        ::GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCSTR)&DLLGetDirectoryOfCurrentModule, &mod);
        DWORD size = ::GetModuleFileNameA(mod, buf, sizeof(buf));
        for (int i = size - 1; i >= 0; --i) {
            if (buf[i] == '\\') {
                buf[i] = '\0';
                s_path = buf;
                break;
            }
        }
    }
#else 
    // todo:
#endif
    return s_path.c_str();
}



void* AlignedAlloc(size_t size, size_t align)
{
#ifdef utjWindows
    return _aligned_malloc(size, align);
#elif defined(utjMac)
    return malloc(size);
#else
    return memalign(align, size);
#endif
}

void AlignedFree(void *p)
{
#ifdef utjWindows
    _aligned_free(p);
#else
    free(p);
#endif
}

double GetCurrentTimeSec()
{
#ifdef utjWindows
    static LARGE_INTEGER g_freq = { 0, 0 };
    if ((u64&)g_freq == 0) {
        ::QueryPerformanceFrequency(&g_freq);
    }

    LARGE_INTEGER t;
    ::QueryPerformanceCounter(&t);
    return double(t.QuadPart) / double(g_freq.QuadPart);
#else
    timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (double)ts.tv_nsec / 1000000000.0;
#endif
}

#ifdef utjWindows
void WinPrintLastError()
{
    auto code = ::GetLastError();
    char buf[1024];

    ::FormatMessageA(
        FORMAT_MESSAGE_FROM_SYSTEM,
        nullptr, ::GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        buf, sizeof(buf), nullptr);
    utjDebugLog("fcWinPrintLastError(): %s", buf);
}
#endif

int Execute(const char *command_)
{
#ifdef utjWindows
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    memset(&si, 0, sizeof(si));
    memset(&pi, 0, sizeof(pi));
    si.cb = sizeof(si);
    std::string command = command_; // CreateProcessA() require **non const** string...
    if (::CreateProcessA(nullptr, (LPSTR)command.c_str(), nullptr, nullptr, TRUE, CREATE_NO_WINDOW, nullptr, nullptr, &si, &pi) == TRUE) {
        DWORD exit_code = 0;
        ::WaitForSingleObject(pi.hThread, INFINITE);
        ::WaitForSingleObject(pi.hProcess, INFINITE);
        ::GetExitCodeProcess(pi.hProcess, &exit_code);
        ::CloseHandle(pi.hThread);
        ::CloseHandle(pi.hProcess);
        return exit_code;
    }
    else {
        WinPrintLastError();
    }
    return 1;
#else
    return std::system(command);
#endif
}


StringPair SplitDirFile(const std::string &path)
{
    auto last_sep = path.find_last_of("/");
#ifdef _WIN32
    if (last_sep == std::string::npos) {
        last_sep = path.find_last_of("\\");
    }
#endif
    if (last_sep != std::string::npos) {
        return StringPair(
            std::string(path.begin(), path.begin() + last_sep),
            std::string(path.begin() + (last_sep + 1), path.end()));
    }
    return StringPair(".", path);
}

StringPair SplitFileExt(const std::string &path)
{
    auto last_sep = path.find_last_of(".");
    if (last_sep != std::string::npos) {
        return StringPair(
            std::string(path.begin(), path.begin() + (last_sep - 1)),
            std::string(path.begin() + last_sep, path.end()));
    }
    return StringPair(path, "");
}

void Glob(const char *pattern, const std::function<void(const char*)>& f)
{
#ifdef _WIN32
    WIN32_FIND_DATA find_data;
    auto h = FindFirstFile(pattern, &find_data);
    if (h != INVALID_HANDLE_VALUE) {
        f(find_data.cFileName);
        while (FindNextFile(h, &find_data)) {
            f(find_data.cFileName);
        }
        CloseHandle(h);
    }

#else

    auto dir_and_mask = tSplitDirFile(pattern);
    auto dir = opendir(dir_and_mask.first.c_str());
    dirent *entry = nullptr;
    while ((entry = readdir(dir)) != nullptr) {
        if (!fnmatch(dir_and_mask.second.c_str(), entry->d_name, FNM_CASEFOLD | FNM_NOESCAPE | FNM_PERIOD)) {
            f((dir_and_mask.first + entry->d_name).c_str());
        }
    }
#endif
}

} // namespace utj
