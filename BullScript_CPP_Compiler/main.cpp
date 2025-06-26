#include "main.hpp"
#include "Util/util.hpp"
#include <filesystem>
#include <memory>
#include <string>
#include "Basm/basmCompiler.hpp"
#include "logger.hpp"

int main(int argc, char *argv[]) {
    // util::alt.setAlt(true);

    Log = std::make_unique<logClass>();
    Log->Options.sOutputLocation = "logs/";
    Log->Options.bVerbose = true;
    Log->Options.bPrint = true;
    Log->Options.bThrowOnError = false;
    Log->Options.iMaxLogCount = 3;

    Log->initialize();

    Log->n("Bull_CPP Compiler - Pre-Alpha");
    if(Log->Options.bPrint == false) {
        util::cPrint("cyan","Bull_CPP Compiler - Pre-Alpha");
    }

    std::vector<std::string> vArgs;
    for(int i = 0; i < argc; i++) {
        vArgs.push_back(util::argvToString(argv[i]));
    }

    bool bTesting = true;
    if (argc < 2) {
        if(bTesting) {
            vArgs.clear();
            vArgs.push_back(util::argvToString(argv[0]));

            // vArgs.push_back(util::switchOnAlt("BullScript_CPP_Compiler/Basm/test.basm", "../BullScript_CPP_Compiler/Basm/test.basm"));
            // HACK: hard coded path to test.basm but TODO: will change later
            if(std::filesystem::exists("/home/vince")) {
                vArgs.push_back("/home/vince/Repos/BullScript/BullScript_CPP_Compiler/Basm/test.basm");
            } else {
                
            }
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

    if(vArgs.at(1).at(0) != '/') {
        vArgs.at(1) = std::string(std::filesystem::current_path()) + "/" + vArgs.at(1);
    }
    basm::compileFromFile(vArgs.at(1));

    return 0;
}

