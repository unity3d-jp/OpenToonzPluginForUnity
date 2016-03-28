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
    public static class otAPI
    {
        public struct otModule { public IntPtr ptr; }
        public struct otPlugin { public IntPtr ptr; }

        public struct otPluginInfo
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

        public struct otParamInfo
        {
            public IntPtr name;
            public IntPtr note;
            public otParamType type;
        };

        public struct otParamData
        {
            public double double_value; // todo
        }



        [DllImport("OpenToonzPlugin")] public static extern otModule    otLoad();
        [DllImport("OpenToonzPlugin")] public static extern void        otUnload(otModule mod);

        [DllImport("OpenToonzPlugin")] public static extern int         otGetNumPlugins(otModule mod);
        [DllImport("OpenToonzPlugin")] public static extern otPlugin    otGetPlugin(otModule mod, int i);

        [DllImport("OpenToonzPlugin")] public static extern void        otGetPluginInfo(otPlugin plugin, ref otPluginInfo info);
        [DllImport("OpenToonzPlugin")] public static extern int         otGetNumParams(otPlugin plugin);
        [DllImport("OpenToonzPlugin")] public static extern void        otGetParamInfo(otPlugin plugin, ref otParamInfo pinfo);
        [DllImport("OpenToonzPlugin")] public static extern void        otApplyFx(otPlugin plugin, ref otParamData pdata, IntPtr pixels, int width, int height);
    }
}