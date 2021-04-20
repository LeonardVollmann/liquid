#!/bin/sh

echo "Building liquid library"
(set -x; clang -g -Isrc/liquid -Ideps/include -fPIC -shared -lGLFW -lGLEW -framework OpenGL -o lib/libliquid.dylib src/liquid/unity_build.c)
#echo "Building asset_compile"
#(set -x; clang -g -Isrc/liquid -Llib -lliquid -o obj/asset_compile src/asset_compile/asset_compile.c)
echo "Building sandbox"
(set -x; clang -g -Isrc/liquid -Llib -lliquid -o obj/sandbox src/sandbox/main.c)