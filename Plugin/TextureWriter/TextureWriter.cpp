#include "pch.h"
#include "fcFoundation.h"
#include "fcGraphicsDevice.h"

typedef fcPixelFormat twPixelFormat;
#include "TextureWriter.h"


twCLinkage twExport bool twWriteTexture(
    void *dst_tex, int dst_width, int dst_height, twPixelFormat dst_fmt,
    const void *src, int src_num, twPixelFormat src_fmt)
{
    auto *dev = fcGetGraphicsDevice();
    if (!dst_tex || !src || !dev) { return false; }

    // convert data if data format is not match
    std::vector<char> tmp_pixels;
    if (dst_fmt != src_fmt) {
        tmp_pixels.resize(src_num * fcGetPixelSize(dst_fmt));
        fcConvertPixelFormat(&tmp_pixels[0], dst_fmt, src, src_fmt, src_num);
        src = &tmp_pixels[0];
    }

    return dev->writeTexture(dst_tex, dst_width, dst_height, dst_fmt, src, src_num * fcGetPixelSize(dst_fmt));
}


twCLinkage twExport bool twReadTexture(
    void *dst, int dst_num, twPixelFormat dst_fmt,
    void *src_tex, int src_width, int src_height, twPixelFormat src_fmt)
{
    auto *dev = fcGetGraphicsDevice();
    if (!dst || !src_tex || !dev) { return false; }


    void *orig_dst = dst;
    std::vector<char> tmp_pixels;
    if (dst_fmt != src_fmt) {
        tmp_pixels.resize(dst_num * fcGetPixelSize(src_fmt));
        dst = &tmp_pixels[0];
    }

    if (!dev->readTexture(dst, dst_num * fcGetPixelSize(src_fmt), src_tex, src_width, src_height, src_fmt)) {
        return false;
    }

    // convert data if data format is not match
    if (!tmp_pixels.empty()) {
        fcConvertPixelFormat(orig_dst, dst_fmt, &tmp_pixels[0], src_fmt, (int)tmp_pixels.size());
    }

    return true;
}
