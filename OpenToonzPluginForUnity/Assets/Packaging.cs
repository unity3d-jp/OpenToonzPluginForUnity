#if UNITY_EDITOR
using System;
using System.Collections;
using UnityEngine;
using UnityEngine.Rendering;
using UnityEditor;


public class Packaging
{
    [MenuItem("Assets/UTJ/OpenToonzPlugin/MakePackage")]
    public static void MakePackages()
    {
        string[] files = new string[]
        {
"Assets/UTJ",
"Assets/StreamingAssets/OpenToonzPlugin/License.txt",
        };
        AssetDatabase.ExportPackage(files, "OpenToonzPluginForUnity.unitypackage", ExportPackageOptions.Recurse);
    }

}
#endif
