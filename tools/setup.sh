#!/usr/bin/env sh
# Generated with OpenAI Codex. Downloads official Raylib 5.5, retaining its license.
set -eu
cd "$(dirname "$0")/.."
mkdir -p zig-pkg
if [ ! -f zig-pkg/raylib-5.5/src/Makefile ]; then
 curl -fL --retry 3 https://codeload.github.com/raysan5/raylib/tar.gz/refs/tags/5.5 -o zig-pkg/raylib.tar.gz
 tar -xzf zig-pkg/raylib.tar.gz -C zig-pkg
 rm zig-pkg/raylib.tar.gz
fi
make -C zig-pkg/raylib-5.5/src PLATFORM=PLATFORM_DESKTOP RAYLIB_LIBTYPE=STATIC -j"${JOBS:-4}"
