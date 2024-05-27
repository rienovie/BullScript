#!/bin/bash

if [ $# -lt 1 ]; then
    echo "Dude... I need an input..."
    exit 1
fi

INPUT=$1

nasm -felf64 "$INPUT"

if [ $? -ne 0 ]; then
    echo "Nasm error"
    exit 1
fi

BASENAME="${INPUT%.*}"
OBJFILE="${BASENAME}.o"

ld "$OBJFILE" -o "$BASENAME"

if [ $? -ne 0 ]; then
    echo "Linker error"
    exit 1
fi

OUTPUT=$?

echo "Compiled with code ${OUTPUT}."
echo "Attempting to run..."

${BASENAME}

OUTPUT=$?

echo "Program exited with code ${OUTPUT}."

exit 0
