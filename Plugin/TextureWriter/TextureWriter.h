#ifndef TextureWriter_h
#define TextureWriter_h

#define twCLinkage extern "C"
#ifdef _WIN32
    #ifndef twStaticLink
        #ifdef twImpl
            #define twExport __declspec(dllexport)
        #else
            #define twExport __declspec(dllimport)
        #endif
    #else
        #define twExport
    #endif
#else
    #define twExport
#endif

#ifndef twImpl
enum twPixelFormat
{
    twPixelFormat_Unknown = 0,

    twPixelFormat_ChannelMask = 0xF,
    twPixelFormat_TypeMask = 0xF << 4,
    twPixelFormat_Type_f16 = 0x1 << 4,
    twPixelFormat_Type_f32 = 0x2 << 4,
    twPixelFormat_Type_u8  = 0x3 << 4,
    twPixelFormat_Type_i16 = 0x4 << 4,
    twPixelFormat_Type_i32 = 0x5 << 4,

    twPixelFormat_Rf16      = twPixelFormat_Type_f16 | 1,
    twPixelFormat_RGf16     = twPixelFormat_Type_f16 | 2,
    twPixelFormat_RGBf16    = twPixelFormat_Type_f16 | 3,
    twPixelFormat_RGBAf16   = twPixelFormat_Type_f16 | 4,
    twPixelFormat_Rf32      = twPixelFormat_Type_f32 | 1,
    twPixelFormat_RGf32     = twPixelFormat_Type_f32 | 2,
    twPixelFormat_RGBf32    = twPixelFormat_Type_f32 | 3,
    twPixelFormat_RGBAf32   = twPixelFormat_Type_f32 | 4,
    twPixelFormat_Ru8       = twPixelFormat_Type_u8  | 1,
    twPixelFormat_RGu8      = twPixelFormat_Type_u8  | 2,
    twPixelFormat_RGBu8     = twPixelFormat_Type_u8  | 3,
    twPixelFormat_RGBAu8    = twPixelFormat_Type_u8  | 4,
    twPixelFormat_Ri16      = twPixelFormat_Type_i16 | 1,
    twPixelFormat_RGi16     = twPixelFormat_Type_i16 | 2,
    twPixelFormat_RGBi16    = twPixelFormat_Type_i16 | 3,
    twPixelFormat_RGBAi16   = twPixelFormat_Type_i16 | 4,
    twPixelFormat_Ri32      = twPixelFormat_Type_i32 | 1,
    twPixelFormat_RGi32     = twPixelFormat_Type_i32 | 2,
    twPixelFormat_RGBi32    = twPixelFormat_Type_i32 | 3,
    twPixelFormat_RGBAi32   = twPixelFormat_Type_i32 | 4,
    twPixelFormat_I420      = 0x10 << 4,
};
#endif // twImpl


// dst_tex: texture object. ID3D11Texture* on d3d11, GLuint (texture handle) on OpenGL, etc.
twCLinkage twExport bool twWriteTexture(
    void *dst_tex, int dst_width, int dst_height, twPixelFormat dst_fmt,
    const void *src, int src_num, twPixelFormat src_fmt);

// src_tex: texture object. ID3D11Texture* on d3d11, GLuint (texture handle) on OpenGL, etc.
twCLinkage twExport bool twReadTexture(
    void *dst, int dst_num, twPixelFormat dst_fmt,
    void *src_tex, int src_width, int src_height, twPixelFormat src_fmt);

#endif // TextureWriter_h
