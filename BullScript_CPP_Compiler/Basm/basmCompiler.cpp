#include "basmCompiler.hpp"

void basm::compileFromFile(std::string sFile) {
    std::string
        sLine,
        sBuild,
        sCurrentBrickName,
        sCurrentBrick;
    bool
        bBrickStarted = false,
        bCompleteBrick = false,
        bMultiLineBrick = false;
    char curChar;
    int iLineLength;

    MACRO_ReadFileByLine(sFile, sLine, {
        iLineLength = sLine.length();
        sBuild.clear();
        for(int i = 0; i < iLineLength; i++) {
            curChar = sLine[i];
            if(curChar == ';') { 
                if(bBrickStarted && !bMultiLineBrick) { bCompleteBrick = true; }
                break;
            }
            sBuild.push_back(curChar);
            if(bBrickStarted) {
                if(bMultiLineBrick) {
                    if (sBuild == "end"
                    && sLine.find(sCurrentBrickName) != sLine.npos) {
                        bCompleteBrick = true;
                        break; // leave line
                    }
                } else {
                    if(curChar == ':') { bMultiLineBrick = true; }
                    if(curChar == '\n') {
                        bCompleteBrick = true;
                        break;
                    }
                }
            } else {
                if(sBuild.length() > 1) {
                    if(curChar == ' ' || curChar == ':') {
                        sBuild.pop_back();
                        sCurrentBrickName = sBuild;
                        sBuild.clear();
                        bBrickStarted = true;
                        if(curChar == ':') { bMultiLineBrick = true; }
                    }
                }
            }
        }

        if(bBrickStarted) {
            if(bCompleteBrick) {
                if (!bMultiLineBrick) { sCurrentBrick.append(sBuild); }
                buildBrick(sCurrentBrickName, sCurrentBrick);
                sCurrentBrick.clear();
                sCurrentBrickName.clear();
                bBrickStarted = false;
                bMultiLineBrick = false;
                bCompleteBrick = false;
            } else {
                sCurrentBrick.append(sBuild);
            }
        }
    });
}

void basm::buildBrick(std::string sBrickName, std::string sBrickRawContents) {
    // TODO:
    util::qPrint("Brick Name:", sBrickName, "\nBrick Contents:", sBrickRawContents);
}
