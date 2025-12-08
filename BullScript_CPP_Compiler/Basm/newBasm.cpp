#include "newBasm.hpp"
#include "../logger.hpp"

std::unordered_map<std::string,nBasm::rawBrick> nBasm::mRawBricks {};

void nBasm::compileFromString(std::string sSource) {
  try {
    Log->workStackPush("init", "Basm compiling from string input");

    buildRawBricks(sSource);

    Log->workStackPop("init");
  } catch (...) {
    util::qPrint("Error in BASM compile.");
  }
}

void nBasm::buildRawBricks(std::string& sSource) {
  Log->workStackPush("buildRawBricks", "Building raw bricks from string input");

  std::vector<std::string> sSourceLines = util::splitStringOnChar(sSource, '\n');

  nBasm::rawBrick curBrick;

  std::string sCurLine, sBuild;
  int iLineNumber = 0;
  char curChar;

  bool bBrickStarted = false, bBrickEnded = true;

  // foreach line in source
  for(auto& line : sSourceLines) {
    iLineNumber++;
    sCurLine = line;

    // foreach char in line
    for(int i = 0; i < sCurLine.length(); i++) {
      curChar = sCurLine[i];

      if(bBrickEnded) {
        if(bBrickStarted) {

        } else {
          if(curChar == ' ' && sBuild.length() > 0) {
            if(sBuild == "create") {
              curBrick.bCreate = true;
            } else if(sBuild == "define") {
              curBrick.bCreate = false;
            } else {

            }
          }
        }
      } else {

      }
    }
  }



  Log->workStackPop("buildRawBricks");
}

void nBasm::error(util::int2d iLines, std::string sMessage, std::string sSolution) {
  Log->e("BASM Error on line(s)",iLines.x,"-",iLines.y,"!\n",sMessage,"\nPossible solution:\n",sSolution,"\n");

  // TODO: add with Log->v current values of variables

  throw NULL;

}

void nBasm::unspecifiedError(std::string sMessage, std::string sSolution) {
  Log->e("BASM Unspecified Error!\n",sMessage,"\nPossible solution:\n",sSolution,"\n");

  // TODO: add with Log->v current values of variables

  throw NULL;
}
