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

echo $'Bull CPP Compile success!\n\n'

if [ $# -gt 0 ] && [ $1 = "run" ]; then
	echo $'Attempting to compile ".../Basm/test.basm"\n'
	./bull_cpp ~/projects/BullScript/BullScript_CPP_Compiler/Basm/test.basm test
fi

if [ $? -ne 0 ]; then
	echo $'Error attempting to Bull_Cpp Compile ".../Basm/test.basm"!\n'
	exit 1
else
	echo $'Successfully Compiled with Bull_Cpp!\n'
	exit 0
fi
