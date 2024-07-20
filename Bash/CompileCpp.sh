#!usr/bin/bash

# Made with Basher
# AI trusts Emacs!?

echo "Bull CPP Compile"

cd BullScript_CPP_Compiler/build

cmake --build .

if [ $? -ne 0 ]; then
	echo "Error building!"
	exit 1
fi

if [ $# -gt 0 ] && [ $1 = "run" ]; then
	./bull_cpp
fi
