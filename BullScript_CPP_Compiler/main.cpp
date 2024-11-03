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

    std::vector<std::string> vArgs;
    for(int i = 0; i < argc; i++) {
        vArgs.push_back(util::argvToString(argv[i]));
    }

    bool bTesting = true;
    if (argc < 2) {
        if(bTesting) {
            vArgs.clear();
            vArgs.push_back(util::argvToString(argv[0]));
            vArgs.push_back("Basm/test.basm");
            // TODO: handle third arg
        } else {
	        std::string sFullCommand = "";
	        for(int i = 0; i < argc; i++) {
	            sFullCommand.append(argv[i]);
	            sFullCommand.push_back(' ');
	        }
	        Log->e("Must specify source and output.\n\nbull_cpp {source} {output}\n\nCommand given:\n",sFullCommand,"\n");
        }
    }
    if(argc < 3) {
        // TODO: make third command be the name of the output
    }

    util::qPrint(vArgs.at(1));
    if(vArgs.at(1).at(0) != '/') {
        vArgs.at(1) = std::string(std::filesystem::current_path()) + "/" + vArgs.at(1);
    }
    basm::compileFromFile(vArgs.at(1));

    return 0;
}

