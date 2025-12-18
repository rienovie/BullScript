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

  std::vector<std::string> vSourceLines = util::splitStringOnChar(sSource, '\n', true);

  nBasm::rawBrick curBrick;

  std::string
    sCurLine,
    sMultiInstruction,
    sBuild;
  int iLineNumber = 0;
  char curChar;

  bool
    // NOTE: another way I could handle "multi" would be multiple passes through the code and just inserting the instruction on each line prior to compilation
    // I don't want to do this because there will be multiple layers (BullLang, BullScript) compiling down to BASM and each layer could add multiple steps, so each pass thru this code, would become a greater compliation time
    // By handling this in one pass, it makes the compiler code more complex, but would be a stronger foundation to build upon
    bMultiInstruction = false,
    bBrickStarted = false,
    bCommentLine = false,
    bBrickEnded = true;

  // foreach line in source
  for(auto& sLine : vSourceLines) {
    iLineNumber++;
    sCurLine = sLine;
    bCommentLine = false;

    // foreach char in line
    for(int i = 0; i < sCurLine.length(); i++) {
      if(bCommentLine) { continue; }

      curChar = sCurLine[i];

      if(curChar == ';') {
        sBuild.clear();
        bCommentLine = true;
        continue;
      }

      if(bBrickEnded) {
        if(curChar == ' ' && sBuild.length() > 0) {
          if(sBuild == "create") {
            curBrick.bCreate = true;
            bBrickStarted = true;
            bBrickEnded = false;
            sBuild.clear();
          } else if(sBuild == "define") {
            curBrick.bCreate = false;
            bBrickStarted = true;
            bBrickEnded = false;
            sBuild.clear();
          } else if(sBuild == "multi") {
            bMultiInstruction = true;
            bBrickStarted = true;
            bBrickEnded = false;
            sBuild.clear();
          } else {
            error({iLineNumber,iLineNumber}, "Unknown keyword '" + sBuild + "'", "Use 'create' - 'define' - 'multi' ");
          }
        } else {
          sBuild.push_back(curChar);
        }
      } else {
        if(bMultiInstruction) {
          if(sMultiInstruction.length() > 0) {
            if(curChar == '}') {
              bBrickEnded = true;
              bBrickStarted = false;
              bMultiInstruction = false;
              sMultiInstruction.clear();
              sBuild.clear();
              bCommentLine = true;
            } else {
              curBrick.iLineNumber = iLineNumber;
              parseRawCreateLine(sLine, sBuild, curBrick);
              sBuild.clear();
              bCommentLine = true;
            }
          } else if((curChar == ' ' || curChar == '{') && sBuild.length() > 0) {
            curBrick.bCreate = (sBuild == "create");
            sMultiInstruction = sBuild;
            sBuild.clear();
          } else {
            sBuild.push_back(curChar);
          }
        }

      }
    }
  }



  Log->workStackPop("buildRawBricks");
}

void nBasm::parseRawCreateLine(std::string& sLine, std::string& sBuild, nBasm::rawBrick& curBrick) {
  sBuild.clear();
  bool bInValueSection = false;

  // foreach char in line
  for(int i = 0; i < sLine.length(); i++) {
    if(sLine[i] == ' ' || sLine[i] == ',') {
      if(sBuild.length() > 0) {
        if(bInValueSection) {
          curBrick.vContents.push_back({curBrick.iLineNumber,sBuild});
        } else {
          curBrick.vAtrributes.push_back(sBuild);
        }
        sBuild.clear();
      }
    } else if(sLine[i] == ':' && sBuild.length() > 0) {

    } else {
      sBuild.push_back(sLine[i]);
    }
  }

  // Handle last value if no space at end of line
  if(sBuild.length() > 0) {
    if(bInValueSection) {
      curBrick.vContents.push_back({curBrick.iLineNumber,sBuild});
      sBuild.clear();
    } else {
      curBrick.vAtrributes.push_back(sBuild);
      sBuild.clear();
    }
  }

  mRawBricks[curBrick.sName] = curBrick;

}

void nBasm::rawBrick::clear() {
  this->iLineNumber = 0;
  this->bCreate = false;
  this->sName.clear();
  this->vInputs.clear();
  this->vAtrributes.clear();
  this->vContents.clear();
}

void nBasm::error(util::int2d iLines, std::string sMessage, std::string sSolution) {
  Log->e("BASM Error on line(s)",iLines.x,"-",iLines.y,"!\n",sMessage,"\nPossible solution:\n",sSolution,"\n");

  // TODO: add with Log->v current values of variables

  throw nullptr;

}

void nBasm::unspecifiedError(std::string sMessage, std::string sSolution) {
  Log->e("BASM Unspecified Error!\n",sMessage,"\nPossible solution:\n",sSolution,"\n");

  // TODO: add with Log->v current values of variables

  throw nullptr;
}
