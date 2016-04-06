using System.Collections;
using UnityEngine;
using UnityEngine.Rendering;
#if UNITY_EDITOR
using UnityEditor;
#endif // UNITY_EDITOR


namespace UTJ
{
    [AddComponentMenu("UTJ/Misc/FixDeltaTime")]
    public class FixDeltaTime : MonoBehaviour
    {
        public float m_targetFrameRate = 30.0f;
        public int m_endFrame = 0;


        public IEnumerator Wait()
        {
            yield return new WaitForEndOfFrame();

            // wait until current dt reaches target dt
            float wt = Time.maximumDeltaTime;
            while (Time.realtimeSinceStartup - Time.unscaledTime < wt)
            {
                System.Threading.Thread.Sleep(1);
            }
            if(m_endFrame > 0 && Time.frameCount >= m_endFrame)
            {

#if UNITY_EDITOR
                UnityEditor.EditorApplication.isPlaying = false;
#else
                Application.Quit();
#endif
            }
        }

        public void ApplyFrameRate()
        {
            Time.maximumDeltaTime = (1.0f / m_targetFrameRate);
        }


        void OnEnable()
        {
            ApplyFrameRate();
        }

        void Update()
        {
            ApplyFrameRate();
            StartCoroutine(Wait());
        }
    }
}