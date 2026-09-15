#!/usr/bin/env bash
export PATH="/home/barty/.platformio/packages/toolchain-xtensa-esp32/bin:$PATH"
exec /usr/local/esp-clang/bin/clangd "$@"