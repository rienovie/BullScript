#include "main.hpp"
#include "Util/util.hpp"
#include <filesystem>
#include <memory>
#include <string>
#include "Basm/basmCompiler.hpp"
#include "logger.hpp"

int main(int argc, char *argv[]) {
    Log = std::make_unique<logClass>();
    Log->Options.sOutputLocation = "logs/";
    Log->Options.bVerbose = true;
    Log->Options.bPrint = true;
    Log->Options.bThrowOnError = true;
    Log->Options.iMaxLogCount = 3;

    Log->initialize();

    Log->n("Bull_CPP Compiler - Pre-Alpha");
    if (argc < 2) {
        std::string sFullCommand = "";
        for(int i = 0; i < argc; i++) {
            sFullCommand.append(argv[i]);
            sFullCommand.push_back(' ');
        }
        Log->e("Must specify source and output.\n\nbull_cpp {source} {output}\nCommand given:",sFullCommand,"\n");
    }
    if(argc < 3) {
        // TODO: make third command be the name of the output
    }


    std::string arg;
    for (int i = 1; i < argc; i++) {
        arg = util::argvToString(argv[i]);
        if(i == 1) {
            if (!(util::endsWith(arg, ".basm"))) {
                Log->e("Source must be .basm file. Arg input:",arg);
            }
            if(arg.at(0) != '/') {
                arg = std::string(std::filesystem::current_path()) + arg;
            }
            basm::compileFromFile(arg);
        }
    }

    return 0;
}

