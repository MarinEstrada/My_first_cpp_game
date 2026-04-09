#!/bin/bash
# Build the game for the web using Emscripten.
# Prerequisites: run `source /path/to/emsdk/emsdk_env.sh` first.

set -e

mkdir -p docs

emcc My_first_cpp_game/emscripten_platform.cpp \
    -s USE_SDL=2 \
    -s WASM=1 \
    -s ALLOW_MEMORY_GROWTH=1 \
    -O2 \
    -o docs/index.html

echo "Build complete. Output is in docs/"
