using System;
using System.IO;
using UnityEngine;
using UnityEditor;

namespace UTJ
{
    [CustomEditor(typeof(OpenToonzFx))]
    public class OpenToonzFxEditor : Editor
    {

        void OnEnable()
        {
        }


        static string MakePathRelative(string path)
        {
            Uri pathToAssets = new Uri(Application.streamingAssetsPath + "/");
            return pathToAssets.MakeRelativeUri(new Uri(path)).ToString();
        }

        bool SelectPlugin()
        {
            var t = target as OpenToonzFx;
            var path = EditorUtility.OpenFilePanel("Select OpenToonz plugin", Application.streamingAssetsPath, "plugin");
            if(path == Application.streamingAssetsPath)
            {
                return false;
            }

            OpenToonzFx.PluginPath ppath;
            if (path.IndexOf(Application.streamingAssetsPath) == -1)
            {
                ppath = new OpenToonzFx.PluginPath
                {
                    m_root = OpenToonzFx.PluginPath.Root.Absolute,
                    m_leaf = path,
                };
            }
            else
            {
                ppath = new OpenToonzFx.PluginPath
                {
                    m_root = OpenToonzFx.PluginPath.Root.StreamingAssetsPath,
                    m_leaf = MakePathRelative(path),
                };
            }

            Undo.RecordObject(target, "Changed Plugin");

            t.pluginPath = ppath;
            t.pluginIndex = 0;
            return true;
        }

        public override void OnInspectorGUI()
        {
            //DrawDefaultInspector();
            //return;

            var t = target as OpenToonzFx;
            const float width = 10.0f;
            bool repaint = false;

            // "Select Plugin" button
            if (GUILayout.Button("Select Plugin", GUILayout.MinWidth(width)))
            {
                repaint = SelectPlugin();
            }

            // preview check
            {
                EditorGUI.BeginChangeCheck();
                t.m_preview = EditorGUILayout.Toggle(
                                "Preview", t.m_preview, GUILayout.MinWidth(width));
                if (EditorGUI.EndChangeCheck())
                {
                    repaint = true;
                }
            }

            var plugin_list = t.pluginList;
            if(plugin_list.Length == 0)
            {
                return;
            }

            // plugin dropdown
            {
                GUILayout.Label(t.pluginPath.GetFileName());

                EditorGUI.BeginChangeCheck();
                int v = EditorGUILayout.Popup(t.pluginIndex, plugin_list);
                if (EditorGUI.EndChangeCheck())
                {
                    Undo.RecordObject(target, "Changed Plugin Index");
                    t.pluginIndex = v;
                    repaint = true;
                }
            }

            EditorGUILayout.Space();

            // plugin name, vendor, note
            {
                GUILayout.Label(t.pluginName + " by " + t.pluginVendor);

                var note = t.pluginNote;
                if (note.Length > 0)
                {
                    GUILayout.Label(t.pluginNote);
                }
            }

            EditorGUILayout.Space();

            // params
            GUILayout.Label("Params:");
            var cparams = t.pluginParams;
            if (cparams != null)
            {
                foreach (var p in cparams)
                {
                    if(p == null) { continue; }
                    var type = p.GetType();
                    if (type == typeof(ToonzDoubleParam))
                    {
                        var vp = p as ToonzDoubleParam;
                        EditorGUI.BeginChangeCheck();
                        var v = EditorGUILayout.DoubleField(
                            new GUIContent(p.name, p.note), vp.value.value, GUILayout.MinWidth(width));
                        if (EditorGUI.EndChangeCheck())
                        {
                            Undo.RecordObject(target, "Changed Param Value");
                            vp.value.value = v;
                            repaint = true;
                        }
                    }
                    else if (type == typeof(ToonzRangeParam))
                    {
                        var vp = p as ToonzRangeParam;
                        EditorGUI.BeginChangeCheck();
                        EditorGUILayout.LabelField(p.name, p.note);
                        EditorGUILayout.BeginHorizontal();
                        var vmin = EditorGUILayout.DoubleField("", vp.value.min, GUILayout.MinWidth(width));
                        var vmax = EditorGUILayout.DoubleField("", vp.value.max, GUILayout.MinWidth(width));
                        EditorGUILayout.EndHorizontal();
                        if (EditorGUI.EndChangeCheck())
                        {
                            Undo.RecordObject(target, "Changed Param Value");
                            vp.value.min = vmin;
                            vp.value.max = vmax;
                            repaint = true;
                        }
                    }
                    else if (type == typeof(ToonzColorParam))
                    {
                        var vp = p as ToonzColorParam;
                        EditorGUI.BeginChangeCheck();
                        EditorGUILayout.LabelField(p.name, p.note);
                        EditorGUILayout.BeginHorizontal();
                        var c0 = EditorGUILayout.IntField("", vp.value.c0, GUILayout.MinWidth(width));
                        var c1 = EditorGUILayout.IntField("", vp.value.c1, GUILayout.MinWidth(width));
                        var c2 = EditorGUILayout.IntField("", vp.value.c2, GUILayout.MinWidth(width));
                        var m = EditorGUILayout.IntField("", vp.value.m, GUILayout.MinWidth(width));
                        EditorGUILayout.EndHorizontal();
                        if (EditorGUI.EndChangeCheck())
                        {
                            Undo.RecordObject(target, "Changed Param Value");
                            vp.value.c0 = c0;
                            vp.value.c1 = c1;
                            vp.value.c2 = c2;
                            vp.value.m = m;
                            repaint = true;
                        }
                    }
                    else if (type == typeof(ToonzPointParam))
                    {
                        var vp = p as ToonzPointParam;
                        EditorGUI.BeginChangeCheck();
                        EditorGUILayout.LabelField(p.name, p.note);
                        EditorGUILayout.BeginHorizontal();
                        var x = EditorGUILayout.DoubleField("", vp.value.x, GUILayout.MinWidth(width));
                        var y = EditorGUILayout.DoubleField("", vp.value.y, GUILayout.MinWidth(width));
                        EditorGUILayout.EndHorizontal();
                        if (EditorGUI.EndChangeCheck())
                        {
                            Undo.RecordObject(target, "Changed Param Value");
                            vp.value.x = x;
                            vp.value.y = y;
                            repaint = true;
                        }
                    }
                    else if (type == typeof(ToonzEnumParam))
                    {
                        var vp = p as ToonzEnumParam;
                        EditorGUI.BeginChangeCheck();
                        var v = EditorGUILayout.IntField(
                            new GUIContent(p.name, p.note), vp.value.value, GUILayout.MinWidth(width));
                        if (EditorGUI.EndChangeCheck())
                        {
                            Undo.RecordObject(target, "Changed Param Value");
                            vp.value.value = v;
                            repaint = true;
                        }
                    }
                    else if (type == typeof(ToonzIntParam))
                    {
                        var vp = p as ToonzIntParam;
                        EditorGUI.BeginChangeCheck();
                        var v = EditorGUILayout.IntField(
                            new GUIContent(p.name, p.note), vp.value.value, GUILayout.MinWidth(width));
                        if (EditorGUI.EndChangeCheck())
                        {
                            Undo.RecordObject(target, "Changed Param Value");
                            vp.value.value = v;
                            repaint = true;
                        }
                    }
                    else if (type == typeof(ToonzBoolParam))
                    {
                        var vp = p as ToonzBoolParam;
                        EditorGUI.BeginChangeCheck();
                        var v = EditorGUILayout.Toggle(
                            new GUIContent(p.name, p.note), vp.value.value != 0, GUILayout.MinWidth(width));
                        if (EditorGUI.EndChangeCheck())
                        {
                            Undo.RecordObject(target, "Changed Param Value");
                            vp.value.value = v ? 1 : 0;
                            repaint = true;
                        }
                    }
                    else if (type == typeof(ToonzStringParam))
                    {
                        var vp = p as ToonzStringParam;
                        EditorGUI.BeginChangeCheck();
                        var v = EditorGUILayout.TextField(
                            new GUIContent(p.name, p.note), vp.value, GUILayout.MinWidth(width));
                        if (EditorGUI.EndChangeCheck())
                        {
                            Undo.RecordObject(target, "Changed Param Value");
                            vp.value = v;
                            repaint = true;
                        }
                    }
                }
            }

            EditorGUILayout.Space();

            // inputs
            GUILayout.Label("Inputs:");
            var ports = t.pluginPorts;
            if(ports != null)
            {
                foreach (var port in ports)
                {
                    EditorGUI.BeginChangeCheck();
                    var newinput = EditorGUILayout.ObjectField(
                        port.name, port.input, typeof(Texture), true, GUILayout.MinWidth(width)) as Texture;
                    if (EditorGUI.EndChangeCheck())
                    {
                        Undo.RecordObject(target, "Changed Input Texture");
                        port.input = newinput;
                        repaint = true;
                    }
                }
                GUILayout.Label("  (None is treated as frame buffer)");
            }


            if(repaint && !Application.isPlaying)
            {
                UnityEditorInternal.InternalEditorUtility.RepaintAllViews();
            }
        }
    }
}
