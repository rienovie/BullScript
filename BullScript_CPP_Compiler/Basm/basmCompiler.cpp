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
                    if(curChar == ':') { bMultiLineBrick = true; break; }
                    if(i == iLineLength - 1) {
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
            sCurrentBrick.push_back('\n');
            if(bCompleteBrick) {
                if (!bMultiLineBrick) { sCurrentBrick.append(sBuild); }
                sanitizeRawBrickData(sCurrentBrickName, sCurrentBrick);
                buildBrick(sCurrentBrickName, sCurrentBrick, bMultiLineBrick);
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

void basm::buildBrick(std::string sBrickName, std::string sBrickRawContents, bool bMultiline) {
    util::qPrint("~",sBrickRawContents,"~");
    if(sBrickName == "create") {
        if(bMultiline) {
            
        } else {

        }
    } else {
        
    }
}

void basm::sanitizeRawBrickData(std::string& sBrickName, std::string& sBrickRawContents) {
    util::removeAllOfChar(sBrickRawContents, '\t');

    //removes empty lines
    std::string
        sBuild = "",
        sOutput;
    int iBuildLength = 0;
    for (int i = 0; i < sBrickRawContents.length(); i++) {
        sBuild.push_back(sBrickRawContents[i]);
        iBuildLength = sBuild.length();

        if(sBrickRawContents[i] == '\n') {
            if(iBuildLength < 3) {
                sBuild.clear();
                iBuildLength = 0;
                continue;
            } else {
                sOutput.append(sBuild);
                sBuild.clear();
                iBuildLength = 0;
            }

        }
    }

    if(iBuildLength > 2) sOutput.append(sBuild);

    if(sOutput[sOutput.length()-1] == '\n') sOutput.pop_back();
    sBrickRawContents = sOutput;

}
