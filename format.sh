#!/bin/bash

TARGET="./phosphor"
INCLUDE="$TARGET/include"
SRC="$TARGET/src"

for FILE in $INCLUDE/*.hpp; do
    clang-format -i $FILE
done

for FILE in $SRC/*.cpp; do
    clang-format -i $FILE
done