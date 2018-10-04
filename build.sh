#!/bin/sh -x

clang -g -Isrc/liquid -Ideps/include -fPIC -shared -lGLFW -lGLEW -framework OpenGL -o lib/libliquid.dylib src/liquid/unity_build.c
clang -g -Isrc/liquid -Llib -lliquid -o obj/sandbox src/sandbox/main.c