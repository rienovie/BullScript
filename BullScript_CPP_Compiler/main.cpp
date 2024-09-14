#include "main.hpp"
#include "Util/util.hpp"
#include <filesystem>
#include <string>
#include "Basm/basmCompiler.hpp"

int main(int argc, char *argv[]) {
    util::qPrint("\n\nBull_CPP Compiler - Pre-Alpha\n");
    if (argc < 2) {
        std::string sFullCommand = "";
        for(int i = 0; i < argc; i++) {
            sFullCommand.append(argv[i]);
            sFullCommand.push_back(' ');
        }
        util::qPrint("Must specify source and output.\n\nbull_cpp {source} {output}\nCommand given:", sFullCommand, "\n");
        return 1;
    }
    if(argc < 3) {
        // TODO: make third command be the name of the output
    }


    std::string arg;
    for (int i = 1; i < argc; i++) {
        arg = util::argvToString(argv[i]);
        if(i == 1) {
            if (!(util::endsWith(arg, ".basm"))) {
                util::qPrint("Source must be .basm file. Arg input: ", arg);
                return 1;
            }
            if(arg.at(0) != '/') {
                arg = std::string(std::filesystem::current_path()) + arg;
            }
            basm::compileFromFile(arg);
        }
    }

    return 0;
}

