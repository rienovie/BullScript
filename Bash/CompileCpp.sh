#!usr/bin/bash

# Made with Basher
# AI trusts Emacs!?

echo "Bull CPP Compile"

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

echo $'Bull CPP Compile success!'

if [ $# -gt 0 ] && [ $1 = "run" ]; then
	cd ..
	testFile=$(pwd)/Basm/test.basm
	./build/bull_cpp $testFile
fi

if [ $? -ne 0 ]; then
	echo $'Error attempting to Bull_Cpp Compile ".../Basm/test.basm"!\n'
	exit 1
else
	echo $'Successfully Compiled with Bull_Cpp!\n'
	exit 0
fi
