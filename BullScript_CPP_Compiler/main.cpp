#include "main.hpp"
#include "Util/util.hpp"
#include <string>
#include "Basm/basmCompiler.hpp"

int main(int argc, char *argv[]) {
    if (argc < 3) {
        util::qPrint("Must specify source and output.\n\nbull_cpp {source} {output}\n");
        return 1;
    }

    std::string arg;
    for (int i = 1; i < argc; i++) {
        arg = util::argvToString(argv[i]);
        if(i == 1) {
            if (!arg.ends_with(".basm")) {
                util::qPrint("Source must be .basm file. BullScript not implemented yet!");
                return 1;
            }
            basm::compileFromFile(arg);
        }
    }

    return 0;
}

