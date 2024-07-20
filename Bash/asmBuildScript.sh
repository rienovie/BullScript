#!/bin/bash

if [ $# -lt 1 ]; then
    echo "No input file given!"
    exit 1
else
    INPUT=$1
fi

#Nasm compile step
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
echo $'Attempting to run...\n\n'

${BASENAME}

OUTPUT=$?

echo $'\n'
echo "Program exited with code ${OUTPUT}."

exit 0
