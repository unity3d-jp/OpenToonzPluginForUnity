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
        [NonSerialized] public RenderTexture rt;
        [NonSerialized] public otpAPI.otpImage image;
        [NonSerialized] public int tw_read;

        public void Release()
        {
            if(rt != null)
            {
                rt.Release();
                rt = null;
            }
            if(tw_read != 0)
            {
                TextureWriter.twEraseDeferredCall(tw_read);
                tw_read = 0;
            }
            if(image)
            {
                otpAPI.otpDestroyImage(image);
                image = default(otpAPI.otpImage);
            }
        }

        public otpAPI.otpImage GetImage()
        {
            if(!image)
            {
                image = otpAPI.otpCreateImage();
            }
            return image;
        }

        public Texture GetUncompressedInput()
        {
            Texture ret = input;

            // copy Texture2D content to RenderTexture because Texture2D is compressed in many cases
            if (input.GetType() == typeof(Texture2D))
            {
                if (rt != null && (rt.width != input.width || rt.height != input.height))
                {
                    rt.Release();
                    rt = null;
                }
                if (rt == null)
                {
                    rt = new RenderTexture(input.width, input.height, 0, RenderTextureFormat.ARGB32);
                }
                Graphics.Blit(input, rt);
                ret = rt;
            }
            return ret;
        }
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

        public virtual void Sanitize() {}
    }
    [Serializable]
    public class ToonzDoubleParam : ToonzParam
    {
        public otpAPI.otpDoubleParamTraits traits;
        public otpAPI.otpDoubleParamValue value;

        public ToonzDoubleParam(otpAPI.otpParam p)
            : base(p)
        {
            otpAPI.otpGetParamTraits(p, ref traits);
            otpAPI.otpGetParamValue(p, ref value);
        }

        public override void Sanitize()
        {
            traits.Sanitize(ref value);
        }
    }
    [Serializable]
    public class ToonzRangeParam : ToonzParam
    {
        public otpAPI.otpRangeParamTraits traits;
        public otpAPI.otpRangeParamValue value;

        public ToonzRangeParam(otpAPI.otpParam p)
            : base(p)
        {
            otpAPI.otpGetParamTraits(p, ref traits);
            otpAPI.otpGetParamValue(p, ref value);
        }

        public override void Sanitize()
        {
            traits.Sanitize(ref value);
        }
    }
    [Serializable]
    public class ToonzColorParam : ToonzParam
    {
        public otpAPI.otpColorParamTraits traits;
        public otpAPI.otpColorParamValue value;

        public ToonzColorParam(otpAPI.otpParam p)
            : base(p)
        {
            otpAPI.otpGetParamTraits(p, ref traits);
            otpAPI.otpGetParamValue(p, ref value);
        }
    }
    [Serializable]
    public class ToonzPointParam : ToonzParam
    {
        public otpAPI.otpPointParamTraits traits;
        public otpAPI.otpPointParamValue value;

        public ToonzPointParam(otpAPI.otpParam p)
            : base(p)
        {
            otpAPI.otpGetParamTraits(p, ref traits);
            otpAPI.otpGetParamValue(p, ref value);
        }

        public override void Sanitize()
        {
            traits.Sanitize(ref value);
        }
    }
    [Serializable]
    public class ToonzEnumParam : ToonzParam
    {
        public otpAPI.otpEnumParamTraits traits;
        public otpAPI.otpEnumParamValue value;
        public string[] names;

        public ToonzEnumParam(otpAPI.otpParam p)
            : base(p)
        {
            otpAPI.otpGetParamTraits(p, ref traits);
            otpAPI.otpGetParamValue(p, ref value);

            var names_ptr = new IntPtr[traits.num];
            Marshal.Copy(traits.names, names_ptr, 0, names_ptr.Length);
            names = new string[traits.num];
            for (int i = 0; i < names_ptr.Length; ++i)
            {
                names[i] = otpAPI.ToS(names_ptr[i]);
            }
        }

        public override void Sanitize()
        {
            traits.Sanitize(ref value);
        }
    }
    [Serializable]
    public class ToonzIntParam : ToonzParam
    {
        public otpAPI.otpIntParamTraits traits;
        public otpAPI.otpIntParamValue value;

        public ToonzIntParam(otpAPI.otpParam p)
            : base(p)
        {
            otpAPI.otpGetParamTraits(p, ref traits);
            otpAPI.otpGetParamValue(p, ref value);
        }

        public override void Sanitize()
        {
            traits.Sanitize(ref value);
        }
    }
    [Serializable]
    public class ToonzBoolParam : ToonzParam
    {
        public otpAPI.otpBoolParamTraits traits;
        public otpAPI.otpBoolParamValue value;

        public ToonzBoolParam(otpAPI.otpParam p)
            : base(p)
        {
            otpAPI.otpGetParamTraits(p, ref traits);
            otpAPI.otpGetParamValue(p, ref value);
        }

        public override void Sanitize()
        {
            traits.Sanitize(ref value);
        }
    }
    [Serializable]
    public class ToonzStringParam : ToonzParam
    {
        public otpAPI.otpStringParamTraits traits;
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
        public struct otpIntParamValue
        {
            public int value;
        };
        [Serializable]
        public struct otpBoolParamValue
        {
            public int value;
        };
        [Serializable]
        public struct otpEnumParamValue
        {
            public int value;
        };
        [Serializable]
        public struct otpDoubleParamValue
        {
            public double value;
        };
        [Serializable]
        public struct otpRangeParamValue
        {
            public double a, b;
        };
        [Serializable]
        public struct otpColorParamValue
        {
            public int c0, c1, c2, m;
        };
        [Serializable]
        public struct otpPointParamValue
        {
            public double x, y;
        };
        [Serializable]
        public struct otpSpectrumParamValue
        {
            public double w;
            public double c0, c1, c2, m;
        };
        [Serializable]
        public struct otpToneCurveParamValue
        {
            public double x, y;
            public int channel;
            public int interp;
        };


        [Serializable]
        public struct otpIntParamTraits
        {
            public int def, min, max;
            public void Sanitize(ref otpIntParamValue v)
            {
                if (min != max)
                {
                    v.value = Math.Max(Math.Min(v.value, max), min);
                }
            }
        };
        [Serializable]
        public struct otpEnumParamTraits
        {
            public int def, num;
            public IntPtr names;
            public void Sanitize(ref otpEnumParamValue v)
            {
                v.value = Math.Max(Math.Min(v.value, num), 0);
            }
        };
        [Serializable]
        public struct otpBoolParamTraits
        {
            public int def;
            public void Sanitize(ref otpBoolParamValue v)
            {
                v.value = Math.Max(Math.Min(v.value, 1), 0);
            }
        };
        [Serializable]
        public struct otpDoubleParamTraits
        {
            public double def, min, max;
            public void Sanitize(ref otpDoubleParamValue v)
            {
                if (min != max)
                {
                    v.value = Math.Max(Math.Min(v.value, max), min);
                }
            }
        };
        [Serializable]
        public struct otpStringParamTraits
        {
            public IntPtr def_;
            public string def { get { return ToS(def_); } }
        };
        [Serializable]
        public struct otpRangeParamTraits
        {
            public otpRangeParamValue def, minmax;
            public void Sanitize(ref otpRangeParamValue v)
            {
                if (minmax.a != minmax.b)
                {
                    v.a = Math.Max(Math.Min(v.a, minmax.b), minmax.a);
                    v.b = Math.Max(Math.Min(v.b, minmax.b), minmax.a);
                }
            }
        };
        [Serializable]
        public struct otpColorParamTraits
        {
            public otpColorParamValue def;
        };
        [Serializable]
        public struct otpPointParamTraits
        {
            public otpPointParamValue def, min, max;
            public void Sanitize(ref otpPointParamValue v)
            {
                if (min.x != max.x)
                {
                    v.x = Math.Max(Math.Min(v.x, max.x), min.x);
                    v.y = Math.Max(Math.Min(v.y, max.y), min.y);
                }
            }
        };
        [Serializable]
        public struct otpSpectrumParamTraits
        {
        };
        [Serializable]
        public struct otpToneCurveParamTraits
        {
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

        
        [DllImport("OpenToonzPlugin")] public static extern void        otpGetParamTraits(otpParam param, ref otpDoubleParamTraits v);
        [DllImport("OpenToonzPlugin")] public static extern void        otpGetParamTraits(otpParam param, ref otpRangeParamTraits v);
        [DllImport("OpenToonzPlugin")] public static extern void        otpGetParamTraits(otpParam param, ref otpColorParamTraits v);
        [DllImport("OpenToonzPlugin")] public static extern void        otpGetParamTraits(otpParam param, ref otpPointParamTraits v);
        [DllImport("OpenToonzPlugin")] public static extern void        otpGetParamTraits(otpParam param, ref otpEnumParamTraits v);
        [DllImport("OpenToonzPlugin")] public static extern void        otpGetParamTraits(otpParam param, ref otpIntParamTraits v);
        [DllImport("OpenToonzPlugin")] public static extern void        otpGetParamTraits(otpParam param, ref otpBoolParamTraits v);
        [DllImport("OpenToonzPlugin")] public static extern void        otpGetParamTraits(otpParam param, ref otpStringParamTraits v);

        [DllImport("OpenToonzPlugin")] public static extern void        otpGetParamValue(otpParam param, ref otpDoubleParamValue v);
        [DllImport("OpenToonzPlugin")] public static extern void        otpGetParamValue(otpParam param, ref otpRangeParamValue v);
        [DllImport("OpenToonzPlugin")] public static extern void        otpGetParamValue(otpParam param, ref otpColorParamValue v);
        [DllImport("OpenToonzPlugin")] public static extern void        otpGetParamValue(otpParam param, ref otpPointParamValue v);
        [DllImport("OpenToonzPlugin")] public static extern void        otpGetParamValue(otpParam param, ref otpEnumParamValue v);
        [DllImport("OpenToonzPlugin")] public static extern void        otpGetParamValue(otpParam param, ref otpIntParamValue v);
        [DllImport("OpenToonzPlugin")] public static extern void        otpGetParamValue(otpParam param, ref otpBoolParamValue v);
        [DllImport("OpenToonzPlugin")] public static extern void        otpGetParamValue(otpParam param, ref byte v);

        [DllImport("OpenToonzPlugin")] public static extern void        otpSetParamValue(otpParam param, ref otpDoubleParamValue v);
        [DllImport("OpenToonzPlugin")] public static extern void        otpSetParamValue(otpParam param, ref otpRangeParamValue v);
        [DllImport("OpenToonzPlugin")] public static extern void        otpSetParamValue(otpParam param, ref otpColorParamValue v);
        [DllImport("OpenToonzPlugin")] public static extern void        otpSetParamValue(otpParam param, ref otpPointParamValue v);
        [DllImport("OpenToonzPlugin")] public static extern void        otpSetParamValue(otpParam param, ref otpEnumParamValue v);
        [DllImport("OpenToonzPlugin")] public static extern void        otpSetParamValue(otpParam param, ref otpIntParamValue v);
        [DllImport("OpenToonzPlugin")] public static extern void        otpSetParamValue(otpParam param, ref otpBoolParamValue v);
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