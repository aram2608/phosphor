#!/bin/bash

VCPKG_ROOT="${VCPKG_ROOT:-$HOME/dev/tools/vcpkg}"
export VCPKG_ROOT

cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake
cmake --build build