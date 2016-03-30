using System;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Runtime.CompilerServices;
using UnityEngine;
#if UNITY_EDITOR
using UnityEditor;
#endif

namespace UTJ
{
    public static class otpAPI
    {
        public struct otpModule { public IntPtr ptr; }
        public struct otpInstance { public IntPtr ptr; }
        public struct otpParam { public IntPtr ptr; }
        public struct otpImage { public IntPtr ptr; }

        public struct otpPluginInfo
        {
            public IntPtr name;
            public IntPtr vendor;
            public IntPtr note;
            public int version_major, version_minor;
        };

        public enum otParamType
        {
            Double,
            Range,
            Pixel,
            Point,
            Enum,
            Int,
            Bool,
            Spectrum,
            String,
            ToneCurve,
        };

        public struct otpImageData
        {

        }

        public struct otpParamInfo
        {
            public IntPtr name;
            public IntPtr note;
            public otParamType type;
        };

        public struct otpParamData
        {
            public double double_value; // todo
        }


        [DllImport("OpenToonzPlugin")] public static extern otpImage    otpCreateImage(int width, int height);
        [DllImport("OpenToonzPlugin")] public static extern otpImage    otpCreateIntrusiveImage(IntPtr data, int width, int height);
        [DllImport("OpenToonzPlugin")] public static extern void        otpDestroyImage(otpImage img);
        [DllImport("OpenToonzPlugin")] public static extern void        otpGetImageData(otpImage img, ref otpImageData data);


        [DllImport("OpenToonzPlugin")] public static extern otpModule   otpLoadModule(string path);
        [DllImport("OpenToonzPlugin")] public static extern void        otpUnloadModule(otpModule mod);

        [DllImport("OpenToonzPlugin")] public static extern int         otpGetNumPlugins(otpModule mod);
        [DllImport("OpenToonzPlugin")] public static extern void        otpGetPluginInfo(otpModule mod, int i, ref otpPluginInfo dst);
        [DllImport("OpenToonzPlugin")] public static extern otpInstance otpCreateInstance(otpModule mod, int i);
        [DllImport("OpenToonzPlugin")] public static extern void        otpDestroyInstance(otpInstance inst);

        [DllImport("OpenToonzPlugin")] public static extern int         otpGetNumParams(otpInstance inst);
        [DllImport("OpenToonzPlugin")] public static extern otpParam    otpGetParam(otpInstance inst, int i);
        [DllImport("OpenToonzPlugin")] public static extern otpParam    otpGetParamByName(otpInstance inst, string name);

        [DllImport("OpenToonzPlugin")] public static extern void        otpGetParamInfo(otpParam param, ref otpParamInfo pinfo);
        // return count of elements if param type is string or tonecurve. otherwise 1
        [DllImport("OpenToonzPlugin")] public static extern int         otpGetParamLength(otpParam param);
        [DllImport("OpenToonzPlugin")] public static extern void        otpGetParamValue(otpParam param, IntPtr dst);
        [DllImport("OpenToonzPlugin")] public static extern void        otpSetParamValue(otpParam param, IntPtr src);

        // return result image. user should otImageDestroy() returned image
        [DllImport("OpenToonzPlugin")] public static extern otpImage    otpApplyFx(otpInstance inst, otpImage src, double frame);
    }
}