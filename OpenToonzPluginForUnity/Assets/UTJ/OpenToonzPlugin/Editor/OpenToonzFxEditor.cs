using System;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Runtime.CompilerServices;
using UnityEngine;
using UnityEditor;

namespace UTJ
{
    [CustomEditor(typeof(OpenToonzFx))]
    public class OpenToonzFxEditor : Editor
    {
        SerializedObject m_obj;
        SerializedProperty m_pluginPath;
        SerializedProperty m_pluginIndex;

        void OnEnable()
        {
            m_obj = new SerializedObject(target);
            m_pluginPath = m_obj.FindProperty("m_pluginPath");
            m_pluginIndex = m_obj.FindProperty("m_pluginIndex");
        }

        public override void OnInspectorGUI()
        {
            //DrawDefaultInspector();
            //return;

            var t = target as OpenToonzFx;
            const float width = 100.0f;

            m_obj.Update();
            EditorGUILayout.PropertyField(m_pluginPath, new GUIContent("Plugin Path"), true);
            EditorGUILayout.PropertyField(m_pluginIndex, new GUIContent("Plugin Index"));
            m_obj.ApplyModifiedProperties();

            EditorGUILayout.Space();

            {
                GUILayout.Label(t.pluginName + " by " + t.pluginVendor);

                var note = t.pluginNote;
                if (note.Length > 0)
                {
                    GUILayout.Label(t.pluginNote);
                }
            }

            EditorGUILayout.Space();

            GUILayout.Label("Params:");

            var cparams = t.pluginParams;
            if (cparams != null)
            {
                foreach (var p in cparams)
                {
                    var type = p.GetType();
                    if (type == typeof(ToonzDoubleParam))
                    {
                        var vp = p as ToonzDoubleParam;
                        vp.value.value = EditorGUILayout.DoubleField(
                            new GUIContent(p.name, p.note), vp.value.value, GUILayout.MinWidth(width));
                    }
                    else if (type == typeof(ToonzIntParam))
                    {
                        var vp = p as ToonzIntParam;
                        vp.value.value = EditorGUILayout.IntField(
                            new GUIContent(p.name, p.note), vp.value.value, GUILayout.MinWidth(width));
                    }
                    else if (type == typeof(ToonzBoolParam))
                    {
                        var vp = p as ToonzBoolParam;
                        vp.value.value = EditorGUILayout.IntField(
                            new GUIContent(p.name, p.note), vp.value.value, GUILayout.MinWidth(width));
                    }
                    else if (type == typeof(ToonzEnumParam))
                    {
                        var vp = p as ToonzEnumParam;
                        vp.value.value = EditorGUILayout.IntField(
                            new GUIContent(p.name, p.note), vp.value.value, GUILayout.MinWidth(width));
                    }
                    else if (type == typeof(ToonzRangeParam))
                    {
                        var vp = p as ToonzRangeParam;
                        EditorGUILayout.LabelField(p.name, p.note);
                        EditorGUILayout.BeginHorizontal();
                        vp.value.min = EditorGUILayout.DoubleField("", vp.value.min, GUILayout.MinWidth(width));
                        vp.value.max = EditorGUILayout.DoubleField("", vp.value.max, GUILayout.MinWidth(width));
                        EditorGUILayout.EndHorizontal();
                    }
                }
            }


            EditorGUILayout.Space();

            GUILayout.Label("Inputs:");
            GUILayout.Label("  (none is treated as frame buffer)");
            var ports = t.pluginPorts;
            if(ports != null)
            {
                foreach (var port in ports)
                {
                    port.input = EditorGUILayout.ObjectField(
                        port.name, port.input, typeof(Texture), true, GUILayout.MinWidth(width)) as Texture;
                }
            }
        }
    }
}
