#include "logger.hpp"
#include "Util/util.hpp"
#include <filesystem>

std::string logClass::sOutputFile;
std::ofstream logClass::outFile;
logOptsStruct logClass::Options;
bool logClass::bInitialized = false;

void logClass::initialize() {
    util::qPrint("Attempting to initialize logger.");
    int iFileCount = 0;
    for(auto& item : std::filesystem::directory_iterator {Options.sOutputLocation}) {
        if(item.is_regular_file()) {
            ++iFileCount;
        }
	}
	if(iFileCount > Options.iMaxLogCount - 1) {
        std::filesystem::path oldestFile = "";
        std::string itemFilename = "";

        // Doesn't feel very efficient but meh
        for(int i = 0; i < (iFileCount - Options.iMaxLogCount) + 1; i++) {
            for(auto& item : std::filesystem::directory_iterator {Options.sOutputLocation}) {
                if(item.is_regular_file()) {
                    if(oldestFile.empty() || compareFileNames(oldestFile, item.path())) {
                        oldestFile = item.path();
                    }
                }
            }
            std::filesystem::remove(oldestFile);
            oldestFile.clear();
        }
    }

	outFile.open(Options.sOutputLocation + sOutputFile);

    bInitialized = true;
    Log->n("Logger initialized.");
}

bool logClass::compareFileNames(std::string sCurrent, std::string sNew) {
    return std::lexicographical_compare(sNew.begin(),sNew.end(),sCurrent.begin(),sCurrent.end());
}

