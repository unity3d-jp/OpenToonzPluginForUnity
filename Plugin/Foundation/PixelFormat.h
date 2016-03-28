#ifndef PixelFormat
#define PixelFormat

enum fcPixelFormat
{
    fcPixelFormat_Unknown = 0,

    fcPixelFormat_ChannelMask = 0xF,
    fcPixelFormat_TypeMask = 0xF << 4,
    fcPixelFormat_Type_f16 = 0x1 << 4,
    fcPixelFormat_Type_f32 = 0x2 << 4,
    fcPixelFormat_Type_u8  = 0x3 << 4,
    fcPixelFormat_Type_i16 = 0x4 << 4,
    fcPixelFormat_Type_i32 = 0x5 << 4,

    fcPixelFormat_Rf16      = fcPixelFormat_Type_f16 | 1,
    fcPixelFormat_RGf16     = fcPixelFormat_Type_f16 | 2,
    fcPixelFormat_RGBf16    = fcPixelFormat_Type_f16 | 3,
    fcPixelFormat_RGBAf16   = fcPixelFormat_Type_f16 | 4,
    fcPixelFormat_Rf32      = fcPixelFormat_Type_f32 | 1,
    fcPixelFormat_RGf32     = fcPixelFormat_Type_f32 | 2,
    fcPixelFormat_RGBf32    = fcPixelFormat_Type_f32 | 3,
    fcPixelFormat_RGBAf32   = fcPixelFormat_Type_f32 | 4,
    fcPixelFormat_Ru8       = fcPixelFormat_Type_u8  | 1,
    fcPixelFormat_RGu8      = fcPixelFormat_Type_u8  | 2,
    fcPixelFormat_RGBu8     = fcPixelFormat_Type_u8  | 3,
    fcPixelFormat_RGBAu8    = fcPixelFormat_Type_u8  | 4,
    fcPixelFormat_Ri16      = fcPixelFormat_Type_i16 | 1,
    fcPixelFormat_RGi16     = fcPixelFormat_Type_i16 | 2,
    fcPixelFormat_RGBi16    = fcPixelFormat_Type_i16 | 3,
    fcPixelFormat_RGBAi16   = fcPixelFormat_Type_i16 | 4,
    fcPixelFormat_Ri32      = fcPixelFormat_Type_i32 | 1,
    fcPixelFormat_RGi32     = fcPixelFormat_Type_i32 | 2,
    fcPixelFormat_RGBi32    = fcPixelFormat_Type_i32 | 3,
    fcPixelFormat_RGBAi32   = fcPixelFormat_Type_i32 | 4,
    fcPixelFormat_I420      = 0x10 << 4,
};
int fcGetPixelSize(fcPixelFormat format);

void fcImageFlipY(void *image_, int width, int height, fcPixelFormat fmt);

class half;
void fcScaleArray(uint8_t *data, size_t size, float scale);
void fcScaleArray(uint16_t *data, size_t size, float scale);
void fcScaleArray(int32_t *data, size_t size, float scale);
void fcScaleArray(half *data, size_t size, float scale);
void fcScaleArray(float *data, size_t size, float scale);
const void* fcConvertPixelFormat(void *dst, fcPixelFormat dstfmt, const void *src, fcPixelFormat srcfmt, size_t size);

#endif // PixelFormat
