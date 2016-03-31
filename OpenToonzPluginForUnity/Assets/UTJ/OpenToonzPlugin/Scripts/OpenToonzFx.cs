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
    class OpenToonzFx : MonoBehaviour
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

        otpAPI.otpInstance m_inst;
        otpAPI.otpImage m_img_src;
        RenderTexture m_rt_tmp;


        void OnEnable()
        {
            var mod = otpAPI.otpLoadModule(m_pluginPath.GetPath());
            m_inst = otpAPI.otpCreateInstance(mod, m_pluginIndex);
        }

        void OnDisable()
        {
            otpAPI.otpDestroyInstance(m_inst);
            m_inst.ptr = IntPtr.Zero;

            otpAPI.otpDestroyImage(m_img_src);
            m_inst.ptr = IntPtr.Zero;

            if(m_rt_tmp != null)
            {
                m_rt_tmp.Release();
                m_rt_tmp = null;
            }
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
            var img_dst = otpAPI.otpApplyFx(m_inst, Time.time);
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