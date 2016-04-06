using System;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Runtime.CompilerServices;
using System.Reflection;
using UnityEngine;
#if UNITY_EDITOR
using UnityEditor;
#endif

namespace UTJ
{
    public class TextureWriter
    {
        public enum twPixelFormat
        {
            Unknown = 0,

            ChannelMask = 0xF,
            TypeMask = 0xF << 4,
            Type_f16 = 0x1 << 4,
            Type_f32 = 0x2 << 4,
            Type_u8  = 0x3 << 4,
            Type_i16 = 0x4 << 4,
            Type_i32 = 0x5 << 4,

            Rf16     = Type_f16 | 1,
            RGf16    = Type_f16 | 2,
            RGBf16   = Type_f16 | 3,
            RGBAf16  = Type_f16 | 4,
            Rf32     = Type_f32 | 1,
            RGf32    = Type_f32 | 2,
            RGBf32   = Type_f32 | 3,
            RGBAf32  = Type_f32 | 4,
            Ru8      = Type_u8  | 1,
            RGu8     = Type_u8  | 2,
            RGBu8    = Type_u8  | 3,
            RGBAu8   = Type_u8  | 4,
            Ri16     = Type_i16 | 1,
            RGi16    = Type_i16 | 2,
            RGBi16   = Type_i16 | 3,
            RGBAi16  = Type_i16 | 4,
            Ri32     = Type_i32 | 1,
            RGi32    = Type_i32 | 2,
            RGBi32   = Type_i32 | 3,
            RGBAi32  = Type_i32 | 4,
        };

        public static int twGetPixelSize(twPixelFormat format)
        {
            switch (format)
            {
                case twPixelFormat.RGBAu8: return 4;
                case twPixelFormat.RGBu8: return 3;
                case twPixelFormat.RGu8: return 2;
                case twPixelFormat.Ru8: return 1;

                case twPixelFormat.RGBAf16:
                case twPixelFormat.RGBAi16: return 8;
                case twPixelFormat.RGBf16:
                case twPixelFormat.RGBi16: return 6;
                case twPixelFormat.RGf16:
                case twPixelFormat.RGi16: return 4;
                case twPixelFormat.Rf16:
                case twPixelFormat.Ri16: return 2;

                case twPixelFormat.RGBAf32:
                case twPixelFormat.RGBAi32: return 16;
                case twPixelFormat.RGBf32:
                case twPixelFormat.RGBi32: return 12;
                case twPixelFormat.RGf32:
                case twPixelFormat.RGi32: return 8;
                case twPixelFormat.Rf32:
                case twPixelFormat.Ri32: return 4;
            }
            return 0;
        }

        public static twPixelFormat twGetPixelFormat(RenderTextureFormat v)
        {
            switch (v)
            {
                case RenderTextureFormat.ARGB32:    return twPixelFormat.RGBAu8;
                case RenderTextureFormat.ARGBHalf:  return twPixelFormat.RGBAf16;
                case RenderTextureFormat.RGHalf:    return twPixelFormat.RGf16;
                case RenderTextureFormat.RHalf:     return twPixelFormat.Rf16;
                case RenderTextureFormat.ARGBFloat: return twPixelFormat.RGBAf32;
                case RenderTextureFormat.RGFloat:   return twPixelFormat.RGf32;
                case RenderTextureFormat.RFloat:    return twPixelFormat.Rf32;
                case RenderTextureFormat.ARGBInt:   return twPixelFormat.RGBAi32;
                case RenderTextureFormat.RGInt:     return twPixelFormat.RGi32;
                case RenderTextureFormat.RInt:      return twPixelFormat.Ri32;
            }
            return twPixelFormat.Unknown;
        }

        public static twPixelFormat twGetPixelFormat(TextureFormat v)
        {
            switch (v)
            {
                case TextureFormat.Alpha8:      return twPixelFormat.Ru8;
                case TextureFormat.RGB24:       return twPixelFormat.RGBu8;
                case TextureFormat.RGBA32:      return twPixelFormat.RGBAu8;
                case TextureFormat.ARGB32:      return twPixelFormat.RGBAu8;
                case TextureFormat.RGBAHalf:    return twPixelFormat.RGBAf16;
                case TextureFormat.RGHalf:      return twPixelFormat.RGf16;
                case TextureFormat.RHalf:       return twPixelFormat.Rf16;
                case TextureFormat.RGBAFloat:   return twPixelFormat.RGBAf32;
                case TextureFormat.RGFloat:     return twPixelFormat.RGf32;
                case TextureFormat.RFloat:      return twPixelFormat.Rf32;
            }
            return twPixelFormat.Unknown;
        }

        public static twPixelFormat twGetPixelFormat(Texture tex)
        {
            if (tex.GetType() == typeof(RenderTexture))
            {
                return twGetPixelFormat(((RenderTexture)tex).format);
            }
            else if (tex.GetType() == typeof(Texture2D))
            {
                return twGetPixelFormat(((Texture2D)tex).format);
            }
            return twPixelFormat.Unknown;
        }


        [DllImport ("TextureWriter")] public static extern IntPtr   twMalloc(int size);
        [DllImport ("TextureWriter")] public static extern void     twFree(IntPtr ptr);

        [DllImport ("TextureWriter")] public static extern IntPtr   GetRenderEventFunc();
        [DllImport ("TextureWriter")] public static extern void     twGuardBegin();
        [DllImport ("TextureWriter")] public static extern void     twGuardEnd();
        [DllImport ("TextureWriter")] public static extern void     twEraseDeferredCall(int id);


        public static bool IsCompressed(Texture t)
        {
            var t2d = t as Texture2D;
            if(t2d != null)
            {
                var fmt = t2d.format;
                if (fmt < 0 ||
                    fmt == TextureFormat.DXT1 ||
                    fmt == TextureFormat.DXT5 ||
                    fmt >= TextureFormat.YUY2)
                {
                    return true;
                }
            }
            return false;
        }


        [DllImport("TextureWriter")]
        private static extern Bool twWriteTexture(
            IntPtr dst_tex, int dst_width, int dst_height, twPixelFormat dst_fmt,
            IntPtr src, int src_num, twPixelFormat src_fmt);

        [DllImport("TextureWriter")]
        private static extern int twWriteTextureDeferred(
            IntPtr dst_tex, int dst_width, int dst_height, twPixelFormat dst_fmt,
            IntPtr src, int src_num, twPixelFormat src_fmt, int id);

        public static bool Write(Texture dst_tex, IntPtr src, int src_num, twPixelFormat src_fmt)
        {
            if(IsCompressed(dst_tex))
            {
                Debug.LogError("TextureWriter: texture format must be uncompressed");
                return false;
            }

            return twWriteTexture(
                dst_tex.GetNativeTexturePtr(), dst_tex.width, dst_tex.height,
                twGetPixelFormat(dst_tex), src, src_num, src_fmt);
        }

        public static int WriteDeferred(Texture dst_tex, IntPtr src, int src_num, twPixelFormat src_fmt, int id)
        {
            if (IsCompressed(dst_tex))
            {
                Debug.LogError("TextureWriter: texture format must be uncompressed");
                return 0;
            }

            return twWriteTextureDeferred(
                dst_tex.GetNativeTexturePtr(), dst_tex.width, dst_tex.height,
                twGetPixelFormat(dst_tex), src, src_num, src_fmt, id);
        }


        [DllImport("TextureWriter")]
        private static extern Bool twReadTexture(
            IntPtr dst, int dst_num, twPixelFormat dst_fmt,
            IntPtr src_tex, int src_width, int src_height, twPixelFormat src_fmt);

        [DllImport("TextureWriter")]
        private static extern int twReadTextureDeferred(
            IntPtr dst, int dst_num, twPixelFormat dst_fmt,
            IntPtr src_tex, int src_width, int src_height, twPixelFormat src_fmt, int id);

        public static bool Read(IntPtr dst, int dst_num, twPixelFormat dst_fmt, Texture src_tex)
        {
            if (IsCompressed(src_tex))
            {
                Debug.LogError("TextureWriter: texture format must be uncompressed");
                return false;
            }

            return twReadTexture(
                dst, dst_num, dst_fmt,
                src_tex.GetNativeTexturePtr(), src_tex.width, src_tex.height, twGetPixelFormat(src_tex));
        }

        public static int ReadDeferred(IntPtr dst, int dst_num, twPixelFormat dst_fmt, Texture src_tex, int id)
        {
            if (IsCompressed(src_tex))
            {
                Debug.LogError("TextureWriter: texture format must be uncompressed");
                return 0;
            }

            return twReadTextureDeferred(
                dst, dst_num, dst_fmt,
                src_tex.GetNativeTexturePtr(), src_tex.width, src_tex.height, twGetPixelFormat(src_tex), id);
        }


        [DllImport("TextureWriter")] private static extern void twSync();
        [DllImport("TextureWriter")] private static extern int  twSyncDeferred(int id);
    }
}