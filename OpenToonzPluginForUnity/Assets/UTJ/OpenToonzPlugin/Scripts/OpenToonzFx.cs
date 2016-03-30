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
        otpAPI.otpImage m_img;
        RenderTexture m_tmpRT;


        void OnEnable()
        {
            var mod = otpAPI.otpLoadModule(m_pluginPath);
            m_inst = otpAPI.otpCreateInstance(mod, m_pluginIndex);
        }

        void OnDisable()
        {
            otpAPI.otpDestroyInstance(m_inst);
            m_inst.ptr = IntPtr.Zero;

            otpAPI.otpDestroyImage(m_img);
            m_inst.ptr = IntPtr.Zero;

            if(m_tmpRT != null)
            {
                m_tmpRT.Release();
                m_tmpRT = null;
            }
        }

        void OnRenderImage(RenderTexture src, RenderTexture dst)
        {
            if(!m_inst) {
                Graphics.Blit(src, dst);
                return;
            }

            if(!m_img)
            {
                m_img = otpAPI.otpCreateImage(src.width, src.height);
            }
            var result = otpAPI.otpApplyFx(m_inst, m_img, Time.time);

            // todo

            otpAPI.otpDestroyImage(result);
        }

    }
}