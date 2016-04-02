#if UNITY_EDITOR
using System;
using System.Collections;
using UnityEngine;
using UnityEngine.Rendering;
using UnityEditor;


public class Packaging
{
    [MenuItem("Assets/UTJ/make package - OpenToonzPlugin")]
    public static void MakePackages()
    {
        string[] files = new string[]
        {
"Assets/UTJ",
"Assets/StreamingAssets/OpenToonzPlugin/License.txt",
        };
        AssetDatabase.ExportPackage(files, "OpenToonzPlugin.unitypackage", ExportPackageOptions.Recurse);
    }

}
#endif
