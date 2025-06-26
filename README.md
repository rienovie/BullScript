This is me having fun trying to make a programming language while also improving my c++ skills as well as learning assembly.

If you're interested you can look thru the "Theory" directory to get an idea of what I'm trying to make the language look/feel like.

No idea if it'll actually be good to work with, but I think making this will be fun fun fun :)

If you want to mess around with it, compile using the Bash scripts. Building on Linux x86_64 cmake.
To compile just run CompileCpp.sh.
To compile & run just run with the argument "run"

 >>sh ./Bash/CompileCpp.sh run

I don't currently have a playground but plan on having one later.


Current Status:
    
    Working on basm(simplified asm) translation to asm.
    
    Todo:
        [] Input
            [x] with .basm file
            [] with string from bs conversion

        [x]SQLite3 db for basm translations and keywords

        [x] Create bricks from file
            [x]"Create"
            [x]"Define"

        [Working here]Branch out
            [x] Verify entry and exit bricks exist
            [] Translate while branching

        [] Compile Asm

        [] Before starting BullScript
            [] If statements
            [] Memory Allocation
