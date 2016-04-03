@echo off

cd Plugin\External
7z\7za.exe x -aos *.7z

cd ..\..\OpenToonzPluginForUnity\Assets\StreamingAssets\OpenToonzPlugin
..\..\..\..\Plugin\External\7z\7za.exe x -aos ..\..\..\..\Packages\DWANGO_Plugins.7z
