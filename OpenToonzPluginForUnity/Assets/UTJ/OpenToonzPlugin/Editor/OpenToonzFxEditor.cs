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
            var path = EditorUtility.OpenFilePanel("Select OpenToonz plugin", Application.streamingAssetsPath + "OpenToonzPlugin", "plugin");
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
            const float width = 20.0f;
            const float label_width_1c = 16.0f;
            const float label_width_3c = 30.0f;
            bool repaint = false;

            // "Select Plugin" button
            if (GUILayout.Button("Select Plugin", GUILayout.MinWidth(width)))
            {
                repaint = SelectPlugin();
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
                var options = GUILayout.MinWidth(width);

                foreach (var p in cparams)
                {
                    if(p == null) { continue; }
                    var type = p.GetType();

                    // double
                    if (type == typeof(ToonzDoubleParam))
                    {
                        var vp = p as ToonzDoubleParam;
                        EditorGUI.BeginChangeCheck();

                        var v = EditorGUILayout.DoubleField(
                            new GUIContent(p.name, p.note), vp.value.value, options);

                        if (EditorGUI.EndChangeCheck())
                        {
                            Undo.RecordObject(target, "Changed Param Value");
                            vp.value.value = v;
                            repaint = true;
                        }
                    }
                    // range
                    else if (type == typeof(ToonzRangeParam))
                    {
                        var vp = p as ToonzRangeParam;
                        EditorGUI.BeginChangeCheck();

                        EditorGUIUtility.labelWidth = 1.0f;
                        EditorGUILayout.BeginHorizontal();
                        EditorGUILayout.LabelField(new GUIContent(p.name, p.note), options);
                        EditorGUIUtility.labelWidth = label_width_3c;
                        var vmin = EditorGUILayout.DoubleField("Min", vp.value.min, options);
                        var vmax = EditorGUILayout.DoubleField("Max", vp.value.max, options);
                        EditorGUILayout.EndHorizontal();
                        EditorGUIUtility.labelWidth = 0.0f;

                        if (EditorGUI.EndChangeCheck())
                        {
                            Undo.RecordObject(target, "Changed Param Value");
                            vp.value.min = vmin;
                            vp.value.max = vmax;
                            repaint = true;
                        }
                    }
                    // color
                    else if (type == typeof(ToonzColorParam))
                    {
                        var vp = p as ToonzColorParam;
                        EditorGUI.BeginChangeCheck();

                        EditorGUILayout.LabelField(new GUIContent(p.name, p.note), options);
                        EditorGUIUtility.labelWidth = label_width_1c;
                        EditorGUILayout.BeginHorizontal();
                        var c0 = EditorGUILayout.IntField("R", vp.value.c0, options);
                        var c1 = EditorGUILayout.IntField("G", vp.value.c1, options);
                        var c2 = EditorGUILayout.IntField("B", vp.value.c2, options);
                        var m = EditorGUILayout.IntField("A", vp.value.m, options);
                        EditorGUILayout.EndHorizontal();
                        EditorGUIUtility.labelWidth = 0.0f;

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
                    // point
                    else if (type == typeof(ToonzPointParam))
                    {
                        var vp = p as ToonzPointParam;
                        EditorGUI.BeginChangeCheck();

                        EditorGUIUtility.labelWidth = 1.0f;
                        EditorGUILayout.BeginHorizontal();
                        EditorGUILayout.LabelField(new GUIContent(p.name, p.note), options);
                        EditorGUIUtility.labelWidth = label_width_1c;
                        var x = EditorGUILayout.DoubleField("X", vp.value.x, options);
                        var y = EditorGUILayout.DoubleField("Y", vp.value.y, options);
                        EditorGUILayout.EndHorizontal();
                        EditorGUIUtility.labelWidth = 0.0f;

                        if (EditorGUI.EndChangeCheck())
                        {
                            Undo.RecordObject(target, "Changed Param Value");
                            vp.value.x = x;
                            vp.value.y = y;
                            repaint = true;
                        }
                    }
                    // enum
                    else if (type == typeof(ToonzEnumParam))
                    {
                        var vp = p as ToonzEnumParam;
                        EditorGUI.BeginChangeCheck();

                        var v = EditorGUILayout.IntField(
                            new GUIContent(p.name, p.note), vp.value.value, options);

                        if (EditorGUI.EndChangeCheck())
                        {
                            Undo.RecordObject(target, "Changed Param Value");
                            vp.value.value = v;
                            repaint = true;
                        }
                    }
                    // int
                    else if (type == typeof(ToonzIntParam))
                    {
                        var vp = p as ToonzIntParam;
                        EditorGUI.BeginChangeCheck();

                        var v = EditorGUILayout.IntField(
                            new GUIContent(p.name, p.note), vp.value.value, options);

                        if (EditorGUI.EndChangeCheck())
                        {
                            Undo.RecordObject(target, "Changed Param Value");
                            vp.value.value = v;
                            repaint = true;
                        }
                    }
                    // bool
                    else if (type == typeof(ToonzBoolParam))
                    {
                        var vp = p as ToonzBoolParam;
                        EditorGUI.BeginChangeCheck();

                        var v = EditorGUILayout.Toggle(
                            new GUIContent(p.name, p.note), vp.value.value != 0, options);

                        if (EditorGUI.EndChangeCheck())
                        {
                            Undo.RecordObject(target, "Changed Param Value");
                            vp.value.value = v ? 1 : 0;
                            repaint = true;
                        }
                    }
                    // string
                    else if (type == typeof(ToonzStringParam))
                    {
                        var vp = p as ToonzStringParam;

                        EditorGUI.BeginChangeCheck();
                        var v = EditorGUILayout.TextField(
                            new GUIContent(p.name, p.note), vp.value, options);

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
