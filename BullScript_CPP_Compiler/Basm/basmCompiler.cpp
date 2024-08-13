#include "basmCompiler.hpp"
#include <algorithm>

std::map<std::string,std::vector<basm::brick>> basm::mBricks {};

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

    for(auto& m : mBricks) {
        for(auto& b : m.second) {
            printBrick(b);
        }
    }
}

void basm::buildBrick(std::string sBrickName, std::string sBrickRawContents, bool bMultiline) {
    brick outputBrick;
    outputBrick.sKeyword = sBrickName;
    outputBrick.sName = "UNDEFINED";

    std::string sBuild = "";
    char currentChar;

    if(sBrickName == "create") {
        if(bMultiline) {
            for(auto& line : util::splitStringOnChar(sBrickRawContents, '\n')) {
                buildBrick("create", line, false);
            }
        } else {
            bool bEqualOnLine = util::contains(sBrickRawContents,'=');
            bool bInsideParen = false;
            for(int i = sBrickRawContents.length()-1; i > -1; i--) {
                currentChar = sBrickRawContents[i];
                if(bInsideParen) {
                    if(i == 0) {
                        std::string sErrorOut;
                        if(currentChar == '\"') {
                            sErrorOut.append("Found 0 index \" on a line while trying to build brick ");
                        } else {
                            sErrorOut.append("Error parsing with a \" while trying to build brick ");
                        }
                        sErrorOut.append(sBrickName);
                        sErrorOut.append("\n\nBrick Contents:\n");
                        sErrorOut.append(sBrickRawContents);

                        error(sErrorOut,"Verify Brick " + sBrickName + " has proper quotation");
                    } else if(currentChar == '"' && sBrickRawContents[i-1] != '\\') {
                        bInsideParen = false;
                    }
                    sBuild.push_back(currentChar);
                } else if(currentChar == '\"') {
                    bInsideParen = true;
                    sBuild.push_back(currentChar);
                } else if((currentChar == ' ' || currentChar == '=') && sBuild.length() > 0) {
                    if(bEqualOnLine && i >= sBrickRawContents.find('=')) {
                        std::reverse(sBuild.begin(), sBuild.end());
                        outputBrick.vContents.push_back(sBuild);
                        sBuild.clear();
                    } else if(outputBrick.sName == "UNDEFINED") {
                        std::reverse(sBuild.begin(), sBuild.end());
                        outputBrick.sName = sBuild;
                        sBuild.clear();
                    } else {
                        std::reverse(sBuild.begin(), sBuild.end());
                        outputBrick.vAttributes.push_back(sBuild);
                        sBuild.clear();
                    }
                } else {
                    if(currentChar != ' ' && currentChar != '=') sBuild.push_back(currentChar);
                    if(i == 0) {
                        std::reverse(sBuild.begin(), sBuild.end());
                        outputBrick.vAttributes.push_back(sBuild);
                    }
                }
            }
        }
    } else if(sBrickName == "define") {
        std::vector<std::string> vElements;
        bool bInContent = false;

        for(int i = 0; i < sBrickRawContents.length(); i++) {
            currentChar = sBrickRawContents[i];
            if(bInContent) {
                if(currentChar == '\n') {
                    if(sBuild.length() > 0) outputBrick.vContents.push_back(sBuild);
                    sBuild.clear();
                } else if(currentChar != ':') sBuild.push_back(currentChar);

            } else if((currentChar == ' ' || currentChar == '<') && sBuild.length() > 0) {
                if(outputBrick.sName == "UNDEFINED") outputBrick.sName = sBuild;
                else {
                    outputBrick.vAttributes.push_back(sBuild);
                    bInContent = true;
                }
                sBuild.clear();
            } else if(currentChar != ' ' && currentChar != '<') sBuild.push_back(currentChar);
        }

        if(sBuild.length() > 0) {
            outputBrick.vContents.push_back(sBuild);
            sBuild.clear();
        }
    } else {
        for(int i = 0; i < sBrickRawContents.length(); i++) {
            currentChar = sBrickRawContents[i];
            if(outputBrick.sName == "UNDEFINED" && currentChar == ':') {
                outputBrick.sName = sBuild;
                sBuild.clear();
            } else if(currentChar == '\n' && sBuild.length() > 0){
                outputBrick.vContents.push_back(sBuild);
                sBuild.clear();
            } else if(currentChar != '\n') {
                sBuild.push_back(currentChar);
            }
        }
        if(sBuild.length() > 0) {
            outputBrick.vContents.push_back(sBuild);
            sBuild.clear();
        }
    }

    if(outputBrick.sName != "UNDEFINED") mBricks[outputBrick.sName].push_back(outputBrick);

}

void basm::sanitizeRawBrickData(std::string& sBrickName, std::string& sBrickRawContents) {
    util::removeAllOfChar(sBrickRawContents, '\t');

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

void basm::error(std::string sMessage, std::string sSolution) {
    util::qPrint("Basm Error!\n", sMessage);
    util::qPrint("\n\nPossible solution:\n", sSolution);
    throw;
}

void basm::printBrick(basm::brick toPrint) {
    util::qPrint("Keyword:", toPrint.sKeyword);
    util::qPrint("Brick Name:", toPrint.sName);
    for(auto& i : toPrint.vAttributes) {
        util::qPrint("Attribute:",i);
    }
    for(auto& i : toPrint.vContents) {
        util::qPrint("Content:",i);
    }
    util::qPrint("\n");
}

