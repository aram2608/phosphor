#!/bin/bash

echo "Building program..."

cd phosphor

cmake -B build -S .
cmake --build build