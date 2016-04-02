using System;
using System.IO;
using System.Runtime.Serialization.Formatters.Binary;
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


        [SerializeField] PluginPath m_pluginPath;
        [SerializeField] int m_pluginIndex;
        ToonzParam[] m_params;

        [SerializeField] byte[] m_serialized;

        otpAPI.otpPluginInfo m_plugin_info = default(otpAPI.otpPluginInfo);
        otpAPI.otpInstance m_inst;
        otpAPI.otpImage m_img_src;
        RenderTexture m_rt_tmp;
        bool m_began;


        public PluginPath pluginPath { get { return m_pluginPath; } }
        public int pluginIndex { get { return m_pluginIndex; } }
        public ToonzParam[] pluginParams { get { return m_params; } }

        public string pluginName { get { return m_plugin_info.name; } }
        public string pluginNote { get { return m_plugin_info.note; } }
        public string pluginVendor { get { return m_plugin_info.vendor; } }


        void UpdateParamList()
        {
            var mod = otpAPI.otpLoadModule(m_pluginPath.GetPath());
            if (!mod) { return; }

            int nplugins = otpAPI.otpGetNumPlugins(mod);
            m_pluginIndex = m_pluginIndex < nplugins ? m_pluginIndex : nplugins - 1;
            otpAPI.otpGetPluginInfo(mod, m_pluginIndex, ref m_plugin_info);

            int pi = m_pluginIndex;
            {
                var inst = otpAPI.otpCreateInstance(mod, pi);
                int nparams = otpAPI.otpGetNumParams(inst);
                if(m_params == null || m_params.Length != nparams)
                {
                    Debug.Log("updated param list");
                    m_params = new ToonzParam[nparams];
                }

                var pinfo = default(otpAPI.otpParamInfo);
                for (int i = 0; i < nparams; ++i)
                {
                    var paramptr = otpAPI.otpGetParam(inst, i);
                    otpAPI.otpGetParamInfo(paramptr, ref pinfo);
                    if(m_params[i] == null || m_params[i].name != pinfo.name || m_params[i].type != pinfo.type)
                    {
                        Debug.Log("new param");
                        m_params[i] = otpAPI.CreateToonzParam(paramptr);
                    }
                }

                otpAPI.otpDestroyInstance(inst);
            }
        }

        void ApplyParams()
        {
            if(m_params==null || !m_inst) { return; }

            int nparams = m_params.Length;
            for (int i = 0; i < nparams; ++i)
            {
                var paramptr = otpAPI.otpGetParam(m_inst, i);
                otpAPI.SetParamValue(paramptr, m_params[i]);
            }
        }

#if UNITY_EDITOR
        void OnValidate()
        {
            m_pluginIndex = m_pluginIndex < 0 ? 0 : m_pluginIndex;
            UpdateParamList();
        }
#endif

        public void OnBeforeSerialize()
        {
            if (m_params != null)
            {
                var stream = new MemoryStream();
                var formatter = new BinaryFormatter();
                formatter.Serialize(stream, m_params);
                stream.Flush();
                m_serialized = stream.GetBuffer();
            }
        }

        public void OnAfterDeserialize()
        {
            if(m_serialized != null && m_serialized.Length > 0)
            {
                var stream = new MemoryStream(m_serialized);
                var formatter = new BinaryFormatter();
                m_params = (ToonzParam[])formatter.Deserialize(stream);
                m_serialized = null;
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
            UpdateParamList();
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

            if (!m_began)
            {
                otpAPI.otpBeginRender(m_inst, rt_src.width, rt_src.height);
            }

            // copy rt_src content to memory
            if (!m_img_src)
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
            ApplyParams();
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