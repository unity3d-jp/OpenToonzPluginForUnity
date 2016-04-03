using System;
using System.IO;
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
    [Serializable]
    public class ToonzPort
    {
        public string name;
        public Texture input;
        [NonSerialized] public otpAPI.otpImage image;
        [NonSerialized] public int tw_read;
    }

    [Serializable]
    public abstract class ToonzParam
    {
        public string name;
        public string note;
        public otpAPI.otpParamType type;

        public ToonzParam(otpAPI.otpParam p)
        {
            var info = default(otpAPI.otpParamInfo);
            otpAPI.otpGetParamInfo(p, ref info);

            name = info.name;
            note = info.note;
            type = info.type;
        }
    }

    [Serializable]
    public class ToonzDoubleParam : ToonzParam
    {
        public otpAPI.otpDoubleValue value;

        public ToonzDoubleParam(otpAPI.otpParam p)
            : base(p)
        {
            otpAPI.otpGetParamValue(p, ref value);
        }
    }

    [Serializable]
    public class ToonzRangeParam : ToonzParam
    {
        public otpAPI.otpRangeValue value;

        public ToonzRangeParam(otpAPI.otpParam p)
            : base(p)
        {
            otpAPI.otpGetParamValue(p, ref value);
        }
    }

    [Serializable]
    public class ToonzColorParam : ToonzParam
    {
        public otpAPI.otpColorValue value;

        public ToonzColorParam(otpAPI.otpParam p)
            : base(p)
        {
            otpAPI.otpGetParamValue(p, ref value);
        }
    }

    [Serializable]
    public class ToonzPointParam : ToonzParam
    {
        public otpAPI.otpPointValue value;

        public ToonzPointParam(otpAPI.otpParam p)
            : base(p)
        {
            otpAPI.otpGetParamValue(p, ref value);
        }
    }

    [Serializable]
    public class ToonzEnumParam : ToonzParam
    {
        public otpAPI.otpEnumValue value;

        public ToonzEnumParam(otpAPI.otpParam p)
            : base(p)
        {
            otpAPI.otpGetParamValue(p, ref value);
        }
    }

    [Serializable]
    public class ToonzIntParam : ToonzParam
    {
        public otpAPI.otpIntValue value;

        public ToonzIntParam(otpAPI.otpParam p)
            : base(p)
        {
            otpAPI.otpGetParamValue(p, ref value);
        }
    }

    [Serializable]
    public class ToonzBoolParam : ToonzParam
    {
        public otpAPI.otpBoolValue value;

        public ToonzBoolParam(otpAPI.otpParam p)
            : base(p)
        {
            otpAPI.otpGetParamValue(p, ref value);
        }
    }

    [Serializable]
    public class ToonzStringParam : ToonzParam
    {
        public string value;

        public ToonzStringParam(otpAPI.otpParam p)
            : base(p)
        {
            int nlen = otpAPI.otpGetParamLength(p);
            if(nlen > 0)
            {
                byte[] tmp = new byte[nlen];
                otpAPI.otpGetParamValue(p, ref tmp[0]);
                value = System.Text.Encoding.ASCII.GetString(tmp);
            }
        }
    }


    public static class otpAPI
    {
        public enum otpParamType
        {
            Double,
            Range,
            Color,
            Point,
            Enum,
            Int,
            Bool,
            Spectrum,
            String,
            ToneCurve,
        };

        public static string ToS(IntPtr ptr)
        {
            return ptr == IntPtr.Zero ? "" : Marshal.PtrToStringAnsi(ptr);
        }

        public struct otpImage
        {
            public IntPtr ptr;
            public static implicit operator bool(otpImage v) { return v.ptr != IntPtr.Zero; }
        }
        public struct otpModule
        {
            public IntPtr ptr;
            public static implicit operator bool(otpModule v) { return v.ptr != IntPtr.Zero; }
        }
        public struct otpInstance
        {
            public IntPtr ptr;
            public static implicit operator bool(otpInstance v) { return v.ptr != IntPtr.Zero; }
        }
        public struct otpParam
        {
            public IntPtr ptr;
            public static implicit operator bool(otpParam v) { return v.ptr != IntPtr.Zero; }
        }
        public struct otpPort
        {
            public IntPtr ptr;
            public static implicit operator bool(otpPort v) { return v.ptr != IntPtr.Zero; }
        }

        public struct otpImageData
        {
            public int width;
            public int height;
            public IntPtr data;
        }

        public struct otpPluginInfo
        {
            public IntPtr name_;
            public IntPtr vendor_;
            public IntPtr note_;
            public int version_major, version_minor;

            public string name { get { return ToS(name_); } }
            public string vendor { get { return ToS(vendor_); } }
            public string note { get { return ToS(note_); } }
        }

        public struct otpParamInfo
        {
            public IntPtr name_;
            public IntPtr note_;
            public otpParamType type;

            public string name { get { return ToS(name_); } }
            public string note { get { return ToS(note_); } }
        }

        public struct otpPortInfo
        {
            public IntPtr name_;

            public string name { get { return ToS(name_); } }
        }

        [Serializable]
        public struct otpIntValue
        {
            public int value;
        };
        [Serializable]
        public struct otpBoolValue
        {
            public int value;
        };
        [Serializable]
        public struct otpEnumValue
        {
            public int value;
        };
        [Serializable]
        public struct otpDoubleValue
        {
            public double value;
        };
        [Serializable]
        public struct otpRangeValue
        {
            public double min, max;
        };
        [Serializable]
        public struct otpColorValue
        {
            public int c0, c1, c2, m;
        };
        [Serializable]
        public struct otpPointValue
        {
            public double x, y;
        };
        [Serializable]
        public struct otpSpectrumValue
        {
            public double w;
            public double c0, c1, c2, m;
        };
        [Serializable]
        public struct otpToneCurveValue
        {
            public double x, y;
            public int channel;
            public int interp;
        };


        [DllImport("OpenToonzPlugin")] public static extern otpImage    otpCreateImage();
        [DllImport("OpenToonzPlugin")] public static extern otpImage    otpCreateIntrusiveImage(IntPtr data, int width, int height);
        [DllImport("OpenToonzPlugin")] public static extern void        otpDestroyImage(otpImage img);
        [DllImport("OpenToonzPlugin")] public static extern void        otpResizeImage(otpImage img, int width, int height);
        [DllImport("OpenToonzPlugin")] public static extern void        otpGetImageData(otpImage img, ref otpImageData data);


        [DllImport("OpenToonzPlugin")] public static extern otpModule   otpLoadModule(string path);
        [DllImport("OpenToonzPlugin")] public static extern void        otpUnloadModule(otpModule mod);

        [DllImport("OpenToonzPlugin")] public static extern int         otpGetNumPlugins(otpModule mod);
        [DllImport("OpenToonzPlugin")] public static extern void        otpGetPluginInfo(otpModule mod, int i, ref otpPluginInfo dst);
        [DllImport("OpenToonzPlugin")] public static extern otpInstance otpCreateInstance(otpModule mod, int i);
        [DllImport("OpenToonzPlugin")] public static extern void        otpDestroyInstance(otpInstance inst);

        [DllImport("OpenToonzPlugin")] public static extern int         otpGetNumPorts(otpInstance inst);
        [DllImport("OpenToonzPlugin")] public static extern otpPort     otpGetPort(otpInstance inst, int i);
        [DllImport("OpenToonzPlugin")] public static extern otpPort     otpGetPortByName(otpInstance inst, string name);
        [DllImport("OpenToonzPlugin")] public static extern void        otpGetPortInfo(otpPort port, ref otpPortInfo info);
        [DllImport("OpenToonzPlugin")] public static extern void        otpSetInput(otpPort port, otpImage src);

        [DllImport("OpenToonzPlugin")] public static extern int         otpGetNumParams(otpInstance inst);
        [DllImport("OpenToonzPlugin")] public static extern otpParam    otpGetParam(otpInstance inst, int i);
        [DllImport("OpenToonzPlugin")] public static extern otpParam    otpGetParamByName(otpInstance inst, string name);
        [DllImport("OpenToonzPlugin")] public static extern void        otpGetParamInfo(otpParam param, ref otpParamInfo pinfo);
        // return count of elements if param type is string or tonecurve. otherwise 1
        [DllImport("OpenToonzPlugin")] public static extern int         otpGetParamLength(otpParam param);
        [DllImport("OpenToonzPlugin")] public static extern void        otpGetParamValue(otpParam param, IntPtr dst);
        [DllImport("OpenToonzPlugin")] public static extern void        otpSetParamValue(otpParam param, IntPtr src);

        // return result image. user *should not* otpImageDestroy() returned image
        [DllImport("OpenToonzPlugin")] public static extern otpImage    otpGetDstImage(otpInstance inst);

        [DllImport("OpenToonzPlugin")] public static extern void        otpBeginRender(otpInstance inst, int width, int height);
        [DllImport("OpenToonzPlugin")] public static extern Bool        otpRender(otpInstance inst, double frame);
        [DllImport("OpenToonzPlugin")] public static extern void        otpEndRender(otpInstance inst);


        [DllImport("OpenToonzPlugin")] public static extern IntPtr      GetRenderEventFunc();
        [DllImport("OpenToonzPlugin")] public static extern void        otpGuardBegin();
        [DllImport("OpenToonzPlugin")] public static extern void        otpGuardEnd();
        [DllImport("OpenToonzPlugin")] public static extern void        otpEraseDeferredCall(int id);
        [DllImport("OpenToonzPlugin")] public static extern int         otpRenderDeferred(otpInstance inst, double frame, int id);


        [DllImport("OpenToonzPlugin")] public static extern void        otpGetParamValue(otpParam param, ref otpDoubleValue v);
        [DllImport("OpenToonzPlugin")] public static extern void        otpGetParamValue(otpParam param, ref otpRangeValue v);
        [DllImport("OpenToonzPlugin")] public static extern void        otpGetParamValue(otpParam param, ref otpColorValue v);
        [DllImport("OpenToonzPlugin")] public static extern void        otpGetParamValue(otpParam param, ref otpPointValue v);
        [DllImport("OpenToonzPlugin")] public static extern void        otpGetParamValue(otpParam param, ref otpEnumValue v);
        [DllImport("OpenToonzPlugin")] public static extern void        otpGetParamValue(otpParam param, ref otpIntValue v);
        [DllImport("OpenToonzPlugin")] public static extern void        otpGetParamValue(otpParam param, ref otpBoolValue v);
        [DllImport("OpenToonzPlugin")] public static extern void        otpGetParamValue(otpParam param, ref byte v);

        [DllImport("OpenToonzPlugin")] public static extern void        otpSetParamValue(otpParam param, ref otpDoubleValue v);
        [DllImport("OpenToonzPlugin")] public static extern void        otpSetParamValue(otpParam param, ref otpRangeValue v);
        [DllImport("OpenToonzPlugin")] public static extern void        otpSetParamValue(otpParam param, ref otpColorValue v);
        [DllImport("OpenToonzPlugin")] public static extern void        otpSetParamValue(otpParam param, ref otpPointValue v);
        [DllImport("OpenToonzPlugin")] public static extern void        otpSetParamValue(otpParam param, ref otpEnumValue v);
        [DllImport("OpenToonzPlugin")] public static extern void        otpSetParamValue(otpParam param, ref otpIntValue v);
        [DllImport("OpenToonzPlugin")] public static extern void        otpSetParamValue(otpParam param, ref otpBoolValue v);
        [DllImport("OpenToonzPlugin")] public static extern void        otpSetParamValue(otpParam param, string v);

        public static ToonzParam CreateToonzParam(otpParam param)
        {
            var info = default(otpParamInfo);
            otpGetParamInfo(param, ref info);

            ToonzParam ret = null;
            switch (info.type)
            {
                case otpParamType.Double: ret = new ToonzDoubleParam(param); break;
                case otpParamType.Range: ret = new ToonzRangeParam(param); break;
                case otpParamType.Color: ret = new ToonzColorParam(param); break;
                case otpParamType.Point: ret = new ToonzPointParam(param); break;
                case otpParamType.Enum: ret = new ToonzEnumParam(param); break;
                case otpParamType.Int: ret = new ToonzIntParam(param); break;
                case otpParamType.Bool: ret = new ToonzBoolParam(param); break;
                case otpParamType.String: ret = new ToonzStringParam(param); break;
            }
            return ret;
        }

        public static void SetParamValue(otpParam param, ToonzParam v)
        {
            var t = v.GetType();
            if (t == typeof(ToonzDoubleParam))
            {
                otpSetParamValue(param, ref ((ToonzDoubleParam)v).value);
            }
            else if (t == typeof(ToonzRangeParam))
            {
                otpSetParamValue(param, ref ((ToonzRangeParam)v).value);
            }
            else if (t == typeof(ToonzColorParam))
            {
                otpSetParamValue(param, ref ((ToonzColorParam)v).value);
            }
            else if (t == typeof(ToonzPointParam))
            {
                otpSetParamValue(param, ref ((ToonzPointParam)v).value);
            }
            else if (t == typeof(ToonzEnumParam))
            {
                otpSetParamValue(param, ref ((ToonzEnumParam)v).value);
            }
            else if (t == typeof(ToonzIntParam))
            {
                otpSetParamValue(param, ref ((ToonzIntParam)v).value);
            }
            else if (t == typeof(ToonzBoolParam))
            {
                otpSetParamValue(param, ref ((ToonzBoolParam)v).value);
            }
            else if (t == typeof(ToonzStringParam))
            {
                otpSetParamValue(param, ((ToonzStringParam)v).value);
            }
        }

        public static string[] GetPluginList(otpModule mod)
        {
            int nplugins = otpGetNumPlugins(mod);
            var ret = new string[nplugins];

            otpPluginInfo info = default(otpPluginInfo);
            for (int i = 0; i < nplugins; ++i)
            {
                otpGetPluginInfo(mod, i, ref info);
                ret[i] = info.name;
            }
            return ret;
        }
    }
}