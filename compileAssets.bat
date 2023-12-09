@echo off

cd /d %~dp0

mkdir .\shaders\bin
glslc .\shaders\triangle.frag -o .\shaders\bin\frag.spv
glslc .\shaders\triangle.vert -o .\shaders\bin\vert.spv

.\asset_compiler\build\lithAssetCompiler.exe .\assets.input .\assets.bin