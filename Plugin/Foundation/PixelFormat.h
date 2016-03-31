#ifndef utj_PixelFormat_h
#define utj_PixelFormat_h

namespace utj {

enum PixelFormat
{
    PixelFormat_Unknown = 0,

    PixelFormat_ChannelMask = 0xF,
    PixelFormat_TypeMask = 0xF << 4,
    PixelFormat_Type_f16 = 0x1 << 4,
    PixelFormat_Type_f32 = 0x2 << 4,
    PixelFormat_Type_u8  = 0x3 << 4,
    PixelFormat_Type_i16 = 0x4 << 4,
    PixelFormat_Type_i32 = 0x5 << 4,

    PixelFormat_Rf16      = PixelFormat_Type_f16 | 1,
    PixelFormat_RGf16     = PixelFormat_Type_f16 | 2,
    PixelFormat_RGBf16    = PixelFormat_Type_f16 | 3,
    PixelFormat_RGBAf16   = PixelFormat_Type_f16 | 4,
    PixelFormat_Rf32      = PixelFormat_Type_f32 | 1,
    PixelFormat_RGf32     = PixelFormat_Type_f32 | 2,
    PixelFormat_RGBf32    = PixelFormat_Type_f32 | 3,
    PixelFormat_RGBAf32   = PixelFormat_Type_f32 | 4,
    PixelFormat_Ru8       = PixelFormat_Type_u8  | 1,
    PixelFormat_RGu8      = PixelFormat_Type_u8  | 2,
    PixelFormat_RGBu8     = PixelFormat_Type_u8  | 3,
    PixelFormat_RGBAu8    = PixelFormat_Type_u8  | 4,
    PixelFormat_Ri16      = PixelFormat_Type_i16 | 1,
    PixelFormat_RGi16     = PixelFormat_Type_i16 | 2,
    PixelFormat_RGBi16    = PixelFormat_Type_i16 | 3,
    PixelFormat_RGBAi16   = PixelFormat_Type_i16 | 4,
    PixelFormat_Ri32      = PixelFormat_Type_i32 | 1,
    PixelFormat_RGi32     = PixelFormat_Type_i32 | 2,
    PixelFormat_RGBi32    = PixelFormat_Type_i32 | 3,
    PixelFormat_RGBAi32   = PixelFormat_Type_i32 | 4,
    PixelFormat_I420      = 0x10 << 4,
};
int GetPixelSize(PixelFormat format);

void ImageFlipY(void *image_, int width, int height, PixelFormat fmt);

class half;
void ScaleArray(uint8_t *data, size_t size, float scale);
void ScaleArray(uint16_t *data, size_t size, float scale);
void ScaleArray(int32_t *data, size_t size, float scale);
void ScaleArray(half *data, size_t size, float scale);
void ScaleArray(float *data, size_t size, float scale);
const void* ConvertPixelFormat(void *dst, PixelFormat dstfmt, const void *src, PixelFormat srcfmt, size_t size);

} // namespace utj
#endif // utj_PixelFormat_h
