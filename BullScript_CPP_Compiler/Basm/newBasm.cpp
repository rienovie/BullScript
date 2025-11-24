#include "newBasm.hpp"
#include "../logger.hpp"

std::unordered_map<std::string,nBasm::rawBrick> nBasm::mRawBricks {};

void nBasm::compileFromString(std::string sSource) {
  Log->workStackPush("init", "Basm compiling from string input");

  buildRawBricks(sSource);

  Log->workStackPop("init");
}

void nBasm::buildRawBricks(std::string sSource) {
  Log->workStackPush("buildRawBricks", "Building raw bricks from string input");

  nBasm::rawBrick curBrick;

  std::string sCurLine;
  int iLineNumber = 0;

  for(auto& line : util::splitStringOnChar(sSource, '\n')) {
    iLineNumber++;
    sCurLine = line;

    for(auto& c : line) {
    }

    }



  Log->workStackPop("buildRawBricks");
}

