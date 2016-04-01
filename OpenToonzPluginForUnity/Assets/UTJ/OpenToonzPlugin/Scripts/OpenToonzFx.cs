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
    [AddComponentMenu("UTJ/OpenToonzFx")]
    [RequireComponent(typeof(Camera))]
    //[ExecuteInEditMode]
    public class OpenToonzFx : MonoBehaviour, ISerializationCallbackReceiver
    {
        [Serializable]
        public class PluginPath
        {
            public enum Root
            {
                StreamingAssetsPath,
                Absolute,
            }

            public Root m_root;
            public string m_leaf;

            public PluginPath() { }
            public PluginPath(Root root, string leaf)
            {
                m_root = root;
                m_leaf = leaf;
            }

            public string GetPath()
            {
                if(m_leaf == null) { return ""; }
                string ret = "";
                switch (m_root)
                {
                    case Root.StreamingAssetsPath:
                        ret += Application.streamingAssetsPath;
                        break;
                    case Root.Absolute:
                        break;
                }
                if (m_leaf.Length > 0)
                {
                    if(ret.Length > 0) { ret += "/"; }
                    ret += m_leaf;
                }
                return ret;
            }
        }


        public PluginPath m_pluginPath;
        public int m_pluginIndex;
        public ToonzParam[][] m_params;

        otpAPI.otpInstance m_inst;
        otpAPI.otpImage m_img_src;
        RenderTexture m_rt_tmp;
        bool m_began;


        public ToonzParam[] GetCurrentParams()
        {
            if(m_params == null) { return null; }
            if(m_pluginIndex >= m_params.Length) { return null; }
            return m_params[m_pluginIndex];
        }

        void UpdateParams()
        {
            var mod = otpAPI.otpLoadModule(m_pluginPath.GetPath());
            if (!mod) { return; }

            int nplugins = otpAPI.otpGetNumPlugins(mod);
            if(m_params == null || m_params.Length != nplugins)
            {
                Debug.Log("updated param list array");
                m_params = new ToonzParam[nplugins][];
            }

            for (int pi = 0; pi < nplugins; ++pi)
            {
                var inst = otpAPI.otpCreateInstance(mod, pi);
                int nparams = otpAPI.otpGetNumParams(inst);
                var ps = m_params[pi];
                if(ps == null || ps.Length != nparams)
                {
                    Debug.Log("updated param list");
                    ps = m_params[pi] = new ToonzParam[nparams];
                }

                var pinfo = default(otpAPI.otpParamInfo);
                for (int i = 0; i < nparams; ++i)
                {
                    var paramptr = otpAPI.otpGetParam(inst, i);
                    otpAPI.otpGetParamInfo(paramptr, ref pinfo);
                    if(ps[i] == null || ps[i].name != pinfo.name || ps[i].type != pinfo.type)
                    {
                        Debug.Log("new param");
                        ps[i] = otpAPI.CreateToonzParam(paramptr);
                    }
                }

                otpAPI.otpDestroyInstance(inst);
            }
        }

#if UNITY_EDITOR
        void OnValidate()
        {
            UpdateParams();
        }
#endif

        public void OnBeforeSerialize()
        {
            if(m_params == null) { return; }
            foreach (var pa in m_params)
            {
                foreach (var p in pa)
                    p.SerializeValue();
            }
        }

        public void OnAfterDeserialize()
        {
            if (m_params == null) { return; }
            foreach (var pa in m_params)
            {
                foreach (var p in pa)
                    p.DeserializeValue();
            }
        }

        void OnEnable()
        {
            var mod = otpAPI.otpLoadModule(m_pluginPath.GetPath());
            if(!mod)
            {
                Debug.LogWarning("OpenToonzFx: failed to load module " + m_pluginPath.GetPath());
                return;
            }
            m_inst = otpAPI.otpCreateInstance(mod, m_pluginIndex);
            UpdateParams();
        }

        void OnDisable()
        {
            if(m_rt_tmp != null)
            {
                m_rt_tmp.Release();
                m_rt_tmp = null;
            }

            if (m_began)
            {
                otpAPI.otpEndRender(m_inst);
                m_began = false;
            }

            otpAPI.otpDestroyInstance(m_inst);
            m_inst.ptr = IntPtr.Zero;

            otpAPI.otpDestroyImage(m_img_src);
            m_inst.ptr = IntPtr.Zero;
        }

        [ImageEffectOpaque]
        void OnRenderImage(RenderTexture rt_src, RenderTexture rt_dst)
        {
            if (rt_dst == null)
            {
                Debug.Log("OpenToonzFx: dst is null");
                return;
            }
            if (!m_inst) {
                Graphics.Blit(rt_src, rt_dst);
                return;
            }

            if(!m_began)
            {
                otpAPI.otpBeginRender(m_inst, rt_src.width, rt_src.height);
            }

            // copy rt_src content to memory
            if(!m_img_src)
            {
                m_img_src = otpAPI.otpCreateImage(rt_src.width, rt_src.height);
            }
            var src_data = default(otpAPI.otpImageData);
            otpAPI.otpGetImageData(m_img_src, ref src_data);
            TextureWriter.Read(src_data.data, src_data.width * src_data.height, TextureWriter.twPixelFormat.RGBAu8, rt_src);

            // set inputs
            {
                var port = otpAPI.otpGetPort(m_inst, 0);
                if(port)
                {
                    otpAPI.otpSetInput(port, m_img_src);
                }
            }

            // apply toonz fx
            var img_dst = otpAPI.otpRender(m_inst, Time.time);
            if(!img_dst)
            {
                Graphics.Blit(rt_src, rt_dst);
                return;
            }

            var dst_data = default(otpAPI.otpImageData);
            otpAPI.otpGetImageData(img_dst, ref dst_data);

            if (dst_data.width == rt_dst.width && dst_data.height == rt_dst.height)
            {
                TextureWriter.Write(rt_dst, dst_data.data, dst_data.width * dst_data.height, TextureWriter.twPixelFormat.RGBAu8);
            }
            else
            {
                // blit & resize if size of img_dst != size of rt_dst
                if (m_rt_tmp == null)
                {
                    m_rt_tmp = new RenderTexture(dst_data.width, dst_data.height, 0, RenderTextureFormat.ARGB32);
                    m_rt_tmp.filterMode = FilterMode.Bilinear;
                    m_rt_tmp.Create();
                }
                TextureWriter.Write(m_rt_tmp, dst_data.data, dst_data.width * dst_data.height, TextureWriter.twPixelFormat.RGBAu8);
                Graphics.Blit(m_rt_tmp, rt_dst);
            }
        }

    }
}