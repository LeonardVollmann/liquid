#!/bin/sh -x

clang -g -Isrc/engine -Ideps/include -fPIC -shared -lGLFW -lGLEW -framework OpenGL -o lib/libengine.dylib src/engine/unity_build.c
clang -g -Llib -lengine -o obj/sandbox src/sandbox/main.c