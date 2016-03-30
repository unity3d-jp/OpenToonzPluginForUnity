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
        public string m_pluginPath;
        public int m_pluginIndex;

        otpAPI.otpInstance m_inst;
        otpAPI.otpImage m_img_src;
        RenderTexture m_rt_tmp;


        void OnEnable()
        {
            var mod = otpAPI.otpLoadModule(m_pluginPath);
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

        void OnRenderImage(RenderTexture rt_src, RenderTexture rt_dst)
        {
            if(!m_inst) {
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

            // apply toonz fx
            var img_dst = otpAPI.otpApplyFx(m_inst, m_img_src, Time.time);
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

            otpAPI.otpDestroyImage(img_dst);
        }

    }
}