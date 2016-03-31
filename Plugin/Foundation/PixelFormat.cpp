#include "pch.h"
#include "Foundation.h"

#define fcEnableISPCKernel

namespace utj {

int GetPixelSize(PixelFormat format)
{
    switch (format)
    {
    case PixelFormat_RGBAu8:  return 4;
    case PixelFormat_RGBu8:   return 3;
    case PixelFormat_RGu8:    return 2;
    case PixelFormat_Ru8:     return 1;

    case PixelFormat_RGBAf16:
    case PixelFormat_RGBAi16: return 8;
    case PixelFormat_RGBf16:
    case PixelFormat_RGBi16:  return 6;
    case PixelFormat_RGf16:
    case PixelFormat_RGi16:   return 4;
    case PixelFormat_Rf16:
    case PixelFormat_Ri16:    return 2;

    case PixelFormat_RGBAf32:
    case PixelFormat_RGBAi32: return 16;
    case PixelFormat_RGBf32:
    case PixelFormat_RGBi32:  return 12;
    case PixelFormat_RGf32:
    case PixelFormat_RGi32:   return 8;
    case PixelFormat_Rf32:
    case PixelFormat_Ri32:    return 4;
    }
    return 0;
}


void ImageFlipY(void *image_, int width, int height, PixelFormat fmt)
{
    size_t pitch = width * GetPixelSize(fmt);
    Buffer buf_((size_t)pitch);
    char *image = (char*)image_;
    char *buf = &buf_[0];

    for (int y = 0; y < height / 2; ++y) {
        int iy = height - y - 1;
        memcpy(buf, image + (pitch*y), pitch);
        memcpy(image + (pitch*y), image + (pitch*iy), pitch);
        memcpy(image + (pitch*iy), buf, pitch);
    }
}


#ifdef fcEnableISPCKernel
#include "ConvertKernel_ispc.h"

void ScaleArray(uint8_t *data, size_t size, float scale)  { ispc::ScaleU8(data, (uint32_t)size, scale); }
void ScaleArray(uint16_t *data, size_t size, float scale) { ispc::ScaleI16(data, (uint32_t)size, scale); }
void ScaleArray(int32_t *data, size_t size, float scale)  { ispc::ScaleI32(data, (uint32_t)size, scale); }
void ScaleArray(half *data, size_t size, float scale)     { ispc::ScaleF16((int16_t*)data, (uint32_t)size, scale); }
void ScaleArray(float *data, size_t size, float scale)    { ispc::ScaleF32(data, (uint32_t)size, scale); }

const void* fcConvertPixelFormat_ISPC(void *dst, PixelFormat dstfmt, const void *src, PixelFormat srcfmt, size_t size_)
{
    uint32_t size = (uint32_t)size_;
    switch (srcfmt) {
    case PixelFormat_RGBAu8:
        switch (dstfmt) {
        case PixelFormat_RGBAu8: return src;
        case PixelFormat_RGBu8: ispc::RGBAu8ToRGBu8((uint8_t*)dst, (uint8_t*)src, size); break;
        case PixelFormat_RGu8: ispc::RGBAu8ToRGu8((uint8_t*)dst, (uint8_t*)src, size); break;
        case PixelFormat_Ru8: ispc::RGBAu8ToRu8((uint8_t*)dst, (uint8_t*)src, size); break;
        case PixelFormat_RGBAf16: ispc::RGBAu8ToRGBAf16((int16_t*)dst, (uint8_t*)src, size); break;
        case PixelFormat_RGBf16: ispc::RGBAu8ToRGBf16((int16_t*)dst, (uint8_t*)src, size); break;
        case PixelFormat_RGf16: ispc::RGBAu8ToRGf16((int16_t*)dst, (uint8_t*)src, size); break;
        case PixelFormat_Rf16: ispc::RGBAu8ToRf16((int16_t*)dst, (uint8_t*)src, size); break;
        case PixelFormat_RGBAf32: ispc::RGBAu8ToRGBAf32((float*)dst, (uint8_t*)src, size); break;
        case PixelFormat_RGBf32: ispc::RGBAu8ToRGBf32((float*)dst, (uint8_t*)src, size); break;
        case PixelFormat_RGf32: ispc::RGBAu8ToRGf32((float*)dst, (uint8_t*)src, size); break;
        case PixelFormat_Rf32: ispc::RGBAu8ToRf32((float*)dst, (uint8_t*)src, size); break;
        }
        break;
    case PixelFormat_RGBu8:
        switch (dstfmt) {
        case PixelFormat_RGBAu8: ispc::RGBu8ToRGBAu8((uint8_t*)dst, (uint8_t*)src, size); break;
        case PixelFormat_RGBu8: return src;
        case PixelFormat_RGu8: ispc::RGBu8ToRGu8((uint8_t*)dst, (uint8_t*)src, size); break;
        case PixelFormat_Ru8: ispc::RGBu8ToRu8((uint8_t*)dst, (uint8_t*)src, size); break;
        case PixelFormat_RGBAf16: ispc::RGBu8ToRGBAf16((int16_t*)dst, (uint8_t*)src, size); break;
        case PixelFormat_RGBf16: ispc::RGBu8ToRGBf16((int16_t*)dst, (uint8_t*)src, size); break;
        case PixelFormat_RGf16: ispc::RGBu8ToRGf16((int16_t*)dst, (uint8_t*)src, size); break;
        case PixelFormat_Rf16: ispc::RGBu8ToRf16((int16_t*)dst, (uint8_t*)src, size); break;
        case PixelFormat_RGBAf32: ispc::RGBu8ToRGBAf32((float*)dst, (uint8_t*)src, size); break;
        case PixelFormat_RGBf32: ispc::RGBu8ToRGBf32((float*)dst, (uint8_t*)src, size); break;
        case PixelFormat_RGf32: ispc::RGBu8ToRGf32((float*)dst, (uint8_t*)src, size); break;
        case PixelFormat_Rf32: ispc::RGBu8ToRf32((float*)dst, (uint8_t*)src, size); break;
        }
        break;
    case PixelFormat_RGu8:
        switch (dstfmt) {
        case PixelFormat_RGBAu8: ispc::RGu8ToRGBAu8((uint8_t*)dst, (uint8_t*)src, size); break;
        case PixelFormat_RGBu8: ispc::RGu8ToRGBu8((uint8_t*)dst, (uint8_t*)src, size); break;
        case PixelFormat_RGu8: return src;
        case PixelFormat_Ru8: ispc::RGu8ToRu8((uint8_t*)dst, (uint8_t*)src, size); break;
        case PixelFormat_RGBAf16: ispc::RGu8ToRGBAf16((int16_t*)dst, (uint8_t*)src, size); break;
        case PixelFormat_RGBf16: ispc::RGu8ToRGBf16((int16_t*)dst, (uint8_t*)src, size); break;
        case PixelFormat_RGf16: ispc::RGu8ToRGf16((int16_t*)dst, (uint8_t*)src, size); break;
        case PixelFormat_Rf16: ispc::RGu8ToRf16((int16_t*)dst, (uint8_t*)src, size); break;
        case PixelFormat_RGBAf32: ispc::RGu8ToRGBAf32((float*)dst, (uint8_t*)src, size); break;
        case PixelFormat_RGBf32: ispc::RGu8ToRGBf32((float*)dst, (uint8_t*)src, size); break;
        case PixelFormat_RGf32: ispc::RGu8ToRGf32((float*)dst, (uint8_t*)src, size); break;
        case PixelFormat_Rf32: ispc::RGu8ToRf32((float*)dst, (uint8_t*)src, size); break;
        }
        break;
    case PixelFormat_Ru8:
        switch (dstfmt) {
        case PixelFormat_RGBAu8: ispc::Ru8ToRGBAu8((uint8_t*)dst, (uint8_t*)src, size); break;
        case PixelFormat_RGBu8: ispc::Ru8ToRGBu8((uint8_t*)dst, (uint8_t*)src, size); break;
        case PixelFormat_RGu8: ispc::Ru8ToRGu8((uint8_t*)dst, (uint8_t*)src, size); break;
        case PixelFormat_Ru8: return src;
        case PixelFormat_RGBAf16: ispc::Ru8ToRGBAf16((int16_t*)dst, (uint8_t*)src, size); break;
        case PixelFormat_RGBf16: ispc::Ru8ToRGBf16((int16_t*)dst, (uint8_t*)src, size); break;
        case PixelFormat_RGf16: ispc::Ru8ToRGf16((int16_t*)dst, (uint8_t*)src, size); break;
        case PixelFormat_Rf16: ispc::Ru8ToRf16((int16_t*)dst, (uint8_t*)src, size); break;
        case PixelFormat_RGBAf32: ispc::Ru8ToRGBAf32((float*)dst, (uint8_t*)src, size); break;
        case PixelFormat_RGBf32: ispc::Ru8ToRGBf32((float*)dst, (uint8_t*)src, size); break;
        case PixelFormat_RGf32: ispc::Ru8ToRGf32((float*)dst, (uint8_t*)src, size); break;
        case PixelFormat_Rf32: ispc::Ru8ToRf32((float*)dst, (uint8_t*)src, size); break;
        }
        break;

    case PixelFormat_RGBAf16:
        switch (dstfmt) {
        case PixelFormat_RGBAu8: ispc::RGBAf16ToRGBAu8((uint8_t*)dst, (int16_t*)src, size); break;
        case PixelFormat_RGBu8: ispc::RGBAf16ToRGBu8((uint8_t*)dst, (int16_t*)src, size); break;
        case PixelFormat_RGu8: ispc::RGBAf16ToRGu8((uint8_t*)dst, (int16_t*)src, size); break;
        case PixelFormat_Ru8: ispc::RGBAf16ToRu8((uint8_t*)dst, (int16_t*)src, size); break;
        case PixelFormat_RGBAi16: ispc::RGBAf16ToRGBAi16((uint16_t*)dst, (int16_t*)src, size); break;
        case PixelFormat_RGBi16: ispc::RGBAf16ToRGBi16((uint16_t*)dst, (int16_t*)src, size); break;
        case PixelFormat_RGi16: ispc::RGBAf16ToRGi16((uint16_t*)dst, (int16_t*)src, size); break;
        case PixelFormat_Ri16: ispc::RGBAf16ToRi16((uint16_t*)dst, (int16_t*)src, size); break;
        case PixelFormat_RGBAf16: return src;
        case PixelFormat_RGBf16: ispc::RGBAf16ToRGBf16((int16_t*)dst, (int16_t*)src, size); break;
        case PixelFormat_RGf16: ispc::RGBAf16ToRGf16((int16_t*)dst, (int16_t*)src, size); break;
        case PixelFormat_Rf16: ispc::RGBAf16ToRf16((int16_t*)dst, (int16_t*)src, size); break;
        case PixelFormat_RGBAf32: ispc::RGBAf16ToRGBAf32((float*)dst, (int16_t*)src, size); break;
        case PixelFormat_RGBf32: ispc::RGBAf16ToRGBf32((float*)dst, (int16_t*)src, size); break;
        case PixelFormat_RGf32: ispc::RGBAf16ToRGf32((float*)dst, (int16_t*)src, size); break;
        case PixelFormat_Rf32: ispc::RGBAf16ToRf32((float*)dst, (int16_t*)src, size); break;
        }
        break;
    case PixelFormat_RGBf16:
        switch (dstfmt) {
        case PixelFormat_RGBAu8: ispc::RGBf16ToRGBAu8((uint8_t*)dst, (int16_t*)src, size); break;
        case PixelFormat_RGBu8: ispc::RGBf16ToRGBu8((uint8_t*)dst, (int16_t*)src, size); break;
        case PixelFormat_RGu8: ispc::RGBf16ToRGu8((uint8_t*)dst, (int16_t*)src, size); break;
        case PixelFormat_Ru8: ispc::RGBf16ToRu8((uint8_t*)dst, (int16_t*)src, size); break;
        case PixelFormat_RGBAi16: ispc::RGBf16ToRGBAi16((uint16_t*)dst, (int16_t*)src, size); break;
        case PixelFormat_RGBi16: ispc::RGBf16ToRGBi16((uint16_t*)dst, (int16_t*)src, size); break;
        case PixelFormat_RGi16: ispc::RGBf16ToRGi16((uint16_t*)dst, (int16_t*)src, size); break;
        case PixelFormat_Ri16: ispc::RGBf16ToRi16((uint16_t*)dst, (int16_t*)src, size); break;
        case PixelFormat_RGBAf16: ispc::RGBf16ToRGBAf16((int16_t*)dst, (int16_t*)src, size); break;
        case PixelFormat_RGBf16: return src;
        case PixelFormat_RGf16: ispc::RGBf16ToRGf16((int16_t*)dst, (int16_t*)src, size); break;
        case PixelFormat_Rf16: ispc::RGBf16ToRf16((int16_t*)dst, (int16_t*)src, size); break;
        case PixelFormat_RGBAf32: ispc::RGBf16ToRGBAf32((float*)dst, (int16_t*)src, size); break;
        case PixelFormat_RGBf32: ispc::RGBf16ToRGBf32((float*)dst, (int16_t*)src, size); break;
        case PixelFormat_RGf32: ispc::RGBf16ToRGf32((float*)dst, (int16_t*)src, size); break;
        case PixelFormat_Rf32: ispc::RGBf16ToRf32((float*)dst, (int16_t*)src, size); break;
        }
        break;
    case PixelFormat_RGf16:
        switch (dstfmt) {
        case PixelFormat_RGBAu8: ispc::RGf16ToRGBAu8((uint8_t*)dst, (int16_t*)src, size); break;
        case PixelFormat_RGBu8: ispc::RGf16ToRGBu8((uint8_t*)dst, (int16_t*)src, size); break;
        case PixelFormat_RGu8: ispc::RGf16ToRGu8((uint8_t*)dst, (int16_t*)src, size); break;
        case PixelFormat_Ru8: ispc::RGf16ToRu8((uint8_t*)dst, (int16_t*)src, size); break;
        case PixelFormat_RGBAi16: ispc::RGf16ToRGBAi16((uint16_t*)dst, (int16_t*)src, size); break;
        case PixelFormat_RGBi16: ispc::RGf16ToRGBi16((uint16_t*)dst, (int16_t*)src, size); break;
        case PixelFormat_RGi16: ispc::RGf16ToRGi16((uint16_t*)dst, (int16_t*)src, size); break;
        case PixelFormat_Ri16: ispc::RGf16ToRi16((uint16_t*)dst, (int16_t*)src, size); break;
        case PixelFormat_RGBAf16: ispc::RGf16ToRGBAf16((int16_t*)dst, (int16_t*)src, size); break;
        case PixelFormat_RGBf16: ispc::RGf16ToRGBf16((int16_t*)dst, (int16_t*)src, size); break;
        case PixelFormat_RGf16: return src;
        case PixelFormat_Rf16: ispc::RGf16ToRf16((int16_t*)dst, (int16_t*)src, size); break;
        case PixelFormat_RGBAf32: ispc::RGf16ToRGBAf32((float*)dst, (int16_t*)src, size); break;
        case PixelFormat_RGBf32: ispc::RGf16ToRGBf32((float*)dst, (int16_t*)src, size); break;
        case PixelFormat_RGf32: ispc::RGf16ToRGf32((float*)dst, (int16_t*)src, size); break;
        case PixelFormat_Rf32: ispc::RGf16ToRf32((float*)dst, (int16_t*)src, size); break;
        }
        break;
    case PixelFormat_Rf16:
        switch (dstfmt) {
        case PixelFormat_RGBAu8: ispc::Rf16ToRGBAu8((uint8_t*)dst, (int16_t*)src, size); break;
        case PixelFormat_RGBu8: ispc::Rf16ToRGBu8((uint8_t*)dst, (int16_t*)src, size); break;
        case PixelFormat_RGu8: ispc::Rf16ToRGu8((uint8_t*)dst, (int16_t*)src, size); break;
        case PixelFormat_Ru8: ispc::Rf16ToRu8((uint8_t*)dst, (int16_t*)src, size); break;
        case PixelFormat_RGBAi16: ispc::Rf16ToRGBAi16((uint16_t*)dst, (int16_t*)src, size); break;
        case PixelFormat_RGBi16: ispc::Rf16ToRGBi16((uint16_t*)dst, (int16_t*)src, size); break;
        case PixelFormat_RGi16: ispc::Rf16ToRGi16((uint16_t*)dst, (int16_t*)src, size); break;
        case PixelFormat_Ri16: ispc::Rf16ToRi16((uint16_t*)dst, (int16_t*)src, size); break;
        case PixelFormat_RGBAf16: ispc::Rf16ToRGBAf16((int16_t*)dst, (int16_t*)src, size); break;
        case PixelFormat_RGBf16: ispc::Rf16ToRGBf16((int16_t*)dst, (int16_t*)src, size); break;
        case PixelFormat_RGf16: ispc::Rf16ToRGf16((int16_t*)dst, (int16_t*)src, size); break;
        case PixelFormat_Rf16: return src;
        case PixelFormat_RGBAf32: ispc::Rf16ToRGBAf32((float*)dst, (int16_t*)src, size); break;
        case PixelFormat_RGBf32: ispc::Rf16ToRGBf32((float*)dst, (int16_t*)src, size); break;
        case PixelFormat_RGf32: ispc::Rf16ToRGf32((float*)dst, (int16_t*)src, size); break;
        case PixelFormat_Rf32: ispc::Rf16ToRf32((float*)dst, (int16_t*)src, size); break;
        }
        break;

    case PixelFormat_RGBAf32:
        switch (dstfmt) {
        case PixelFormat_RGBAu8: ispc::RGBAf32ToRGBAu8((uint8_t*)dst, (float*)src, size); break;
        case PixelFormat_RGBu8: ispc::RGBAf32ToRGBu8((uint8_t*)dst, (float*)src, size); break;
        case PixelFormat_RGu8: ispc::RGBAf32ToRGu8((uint8_t*)dst, (float*)src, size); break;
        case PixelFormat_Ru8: ispc::RGBAf32ToRu8((uint8_t*)dst, (float*)src, size); break;
        case PixelFormat_RGBAi16: ispc::RGBAf32ToRGBAi16((uint16_t*)dst, (float*)src, size); break;
        case PixelFormat_RGBi16: ispc::RGBAf32ToRGBi16((uint16_t*)dst, (float*)src, size); break;
        case PixelFormat_RGi16: ispc::RGBAf32ToRGi16((uint16_t*)dst, (float*)src, size); break;
        case PixelFormat_Ri16: ispc::RGBAf32ToRi16((uint16_t*)dst, (float*)src, size); break;
        case PixelFormat_RGBAf16: ispc::RGBAf32ToRGBAf16((int16_t*)dst, (float*)src, size); break;
        case PixelFormat_RGBf16: ispc::RGBAf32ToRGBf16((int16_t*)dst, (float*)src, size); break;
        case PixelFormat_RGf16: ispc::RGBAf32ToRGf16((int16_t*)dst, (float*)src, size); break;
        case PixelFormat_Rf16: ispc::RGBAf32ToRf16((int16_t*)dst, (float*)src, size); break;
        case PixelFormat_RGBAf32: return src;
        case PixelFormat_RGBf32: ispc::RGBAf32ToRGBf32((float*)dst, (float*)src, size); break;
        case PixelFormat_RGf32: ispc::RGBAf32ToRGf32((float*)dst, (float*)src, size); break;
        case PixelFormat_Rf32: ispc::RGBAf32ToRf32((float*)dst, (float*)src, size); break;
        }
        break;
    case PixelFormat_RGBf32:
        switch (dstfmt) {
        case PixelFormat_RGBAu8: ispc::RGBf32ToRGBAu8((uint8_t*)dst, (float*)src, size); break;
        case PixelFormat_RGBu8: ispc::RGBf32ToRGBu8((uint8_t*)dst, (float*)src, size); break;
        case PixelFormat_RGu8: ispc::RGBf32ToRGu8((uint8_t*)dst, (float*)src, size); break;
        case PixelFormat_Ru8: ispc::RGBf32ToRu8((uint8_t*)dst, (float*)src, size); break;
        case PixelFormat_RGBAi16: ispc::RGBf32ToRGBAi16((uint16_t*)dst, (float*)src, size); break;
        case PixelFormat_RGBi16: ispc::RGBf32ToRGBi16((uint16_t*)dst, (float*)src, size); break;
        case PixelFormat_RGi16: ispc::RGBf32ToRGi16((uint16_t*)dst, (float*)src, size); break;
        case PixelFormat_Ri16: ispc::RGBf32ToRi16((uint16_t*)dst, (float*)src, size); break;
        case PixelFormat_RGBAf16: ispc::RGBf32ToRGBAf16((int16_t*)dst, (float*)src, size); break;
        case PixelFormat_RGBf16: ispc::RGBf32ToRGBf16((int16_t*)dst, (float*)src, size); break;
        case PixelFormat_RGf16: ispc::RGBf32ToRGf16((int16_t*)dst, (float*)src, size); break;
        case PixelFormat_Rf16: ispc::RGBf32ToRf16((int16_t*)dst, (float*)src, size); break;
        case PixelFormat_RGBAf32: ispc::RGBf32ToRGBAf32((float*)dst, (float*)src, size); break;
        case PixelFormat_RGBf32: return src;
        case PixelFormat_RGf32: ispc::RGBf32ToRGf32((float*)dst, (float*)src, size); break;
        case PixelFormat_Rf32: ispc::RGBf32ToRf32((float*)dst, (float*)src, size); break;
        }
        break;
    case PixelFormat_RGf32:
        switch (dstfmt) {
        case PixelFormat_RGBAu8: ispc::RGf32ToRGBAu8((uint8_t*)dst, (float*)src, size); break;
        case PixelFormat_RGBu8: ispc::RGf32ToRGBu8((uint8_t*)dst, (float*)src, size); break;
        case PixelFormat_RGu8: ispc::RGf32ToRGu8((uint8_t*)dst, (float*)src, size); break;
        case PixelFormat_Ru8: ispc::RGf32ToRu8((uint8_t*)dst, (float*)src, size); break;
        case PixelFormat_RGBAi16: ispc::RGf32ToRGBAi16((uint16_t*)dst, (float*)src, size); break;
        case PixelFormat_RGBi16: ispc::RGf32ToRGBi16((uint16_t*)dst, (float*)src, size); break;
        case PixelFormat_RGi16: ispc::RGf32ToRGi16((uint16_t*)dst, (float*)src, size); break;
        case PixelFormat_Ri16: ispc::RGf32ToRi16((uint16_t*)dst, (float*)src, size); break;
        case PixelFormat_RGBAf16: ispc::RGf32ToRGBAf16((int16_t*)dst, (float*)src, size); break;
        case PixelFormat_RGBf16: ispc::RGf32ToRGBf16((int16_t*)dst, (float*)src, size); break;
        case PixelFormat_RGf16: ispc::RGf32ToRGf16((int16_t*)dst, (float*)src, size); break;
        case PixelFormat_Rf16: ispc::RGf32ToRf16((int16_t*)dst, (float*)src, size); break;
        case PixelFormat_RGBAf32: ispc::RGf32ToRGBAf32((float*)dst, (float*)src, size); break;
        case PixelFormat_RGBf32: ispc::RGf32ToRGBf32((float*)dst, (float*)src, size); break;
        case PixelFormat_RGf32: return src;
        case PixelFormat_Rf32: ispc::RGf32ToRf32((float*)dst, (float*)src, size); break;
        }
        break;
    case PixelFormat_Rf32:
        switch (dstfmt) {
        case PixelFormat_RGBAu8: ispc::Rf32ToRGBAu8((uint8_t*)dst, (float*)src, size); break;
        case PixelFormat_RGBu8: ispc::Rf32ToRGBu8((uint8_t*)dst, (float*)src, size); break;
        case PixelFormat_RGu8: ispc::Rf32ToRGu8((uint8_t*)dst, (float*)src, size); break;
        case PixelFormat_Ru8: ispc::Rf32ToRu8((uint8_t*)dst, (float*)src, size); break;
        case PixelFormat_RGBAi16: ispc::Rf32ToRGBAi16((uint16_t*)dst, (float*)src, size); break;
        case PixelFormat_RGBi16: ispc::Rf32ToRGBi16((uint16_t*)dst, (float*)src, size); break;
        case PixelFormat_RGi16: ispc::Rf32ToRGi16((uint16_t*)dst, (float*)src, size); break;
        case PixelFormat_Ri16: ispc::Rf32ToRi16((uint16_t*)dst, (float*)src, size); break;
        case PixelFormat_RGBAf16: ispc::Rf32ToRGBAf16((int16_t*)dst, (float*)src, size); break;
        case PixelFormat_RGBf16: ispc::Rf32ToRGBf16((int16_t*)dst, (float*)src, size); break;
        case PixelFormat_RGf16: ispc::Rf32ToRGf16((int16_t*)dst, (float*)src, size); break;
        case PixelFormat_Rf16: ispc::Rf32ToRf16((int16_t*)dst, (float*)src, size); break;
        case PixelFormat_RGBAf32: ispc::Rf32ToRGBAf32((float*)dst, (float*)src, size); break;
        case PixelFormat_RGBf32: ispc::Rf32ToRGBf32((float*)dst, (float*)src, size); break;
        case PixelFormat_RGf32: ispc::Rf32ToRGf32((float*)dst, (float*)src, size); break;
        case PixelFormat_Rf32: return src;
        }
        break;
    }
    return dst;
}

const void* ConvertPixelFormat(void *dst, PixelFormat dstfmt, const void *src, PixelFormat srcfmt, size_t size)
{
    return fcConvertPixelFormat_ISPC(dst, dstfmt, src, srcfmt, size);
}
#endif // fcEnableISPCKernel

} // namespace utj
