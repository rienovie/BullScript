#!/usr/bin/bash

# Made with Basher
# AI trusts Emacs!?

echo -ne "\n----------Bull CPP Compiler----------\n\n"

if [ ! -d "BullScript_CPP_Compiler" ]; then
	echo "Change directory to project root before running this script."
	exit 1
fi

utilSubMod=$(pwd)/BullScript_CPP_Compiler/Util/util.cpp

if [ ! -f "$utilSubMod" ]; then
	echo "Error! Util Submodule not found! Please update git submodules."
	exit 1
fi

buildDir=BullScript_CPP_Compiler/build

if [ -d "$buildDir" ]; then
	cd "$buildDir"
else
	cd BullScript_CPP_Compiler
	mkdir "build"
	cd build
	cmake ..
fi

cmake --build .

if [ $? -ne 0 ]; then
	echo "Error building!"
	exit 1
fi

echo $'Bull CPP Compile success!\n'

if [ $# -gt 0 ] && [ $1 = "run" ]; then
	cd ..
	# testFile=$(pwd)/Basm/test.basm
	# ./build/bull_cpp $testFile
	./build/bull_cpp
fi

if [ $? -ne 0 ]; then
	echo -ne "\n--------Error attempting to Bull_Cpp Compile '.../Basm/test.basm'!--------\n\n"
	exit 1
else
	echo -ne "\n--------Successfully Compiled with Bull_Cpp!--------\n\n"
	exit 0
fi
