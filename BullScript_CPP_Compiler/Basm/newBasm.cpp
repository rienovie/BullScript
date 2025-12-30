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

// TODO: redo this function to use switch statements and states
void nBasm::buildRawBricks(std::string& sSource) {
  Log->workStackPush("buildRawBricks", "Building raw bricks from string input");

  std::vector<std::string> vSourceLines = util::splitStringOnChar(sSource, '\n', true);

  nBasm::rawBrick curBrick;

  std::string
    sCurLine,
    sBuild;
  int
    iLineNumber = 0,
    iCurIndent = 0;
  char curChar;

  bool
    bMultiCreate = false,
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
      // also used for ignoring the rest of the line
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
          } else {
            error({iLineNumber,iLineNumber}, "Unknown keyword '" + sBuild + "'" + "\n\n" + sLine + "\n\n", "Use 'create' or 'define'");
          }
        } else {
          sBuild.push_back(curChar);
        }
      } else {
        if(bMultiCreate) {
          if(curChar == '}') {
            bBrickEnded = true;
            bBrickStarted = false;
            bMultiCreate = false;
            sBuild.clear();
            bCommentLine = true;
          } else {
            curBrick.iLineNumber = iLineNumber;
            parseRawCreateLine(sLine, sBuild, curBrick);
            sBuild.clear();
            bCommentLine = true;
          }
        } else if(curBrick.bCreate) {
            if(util::contains(sBuild,'{')) {
              bMultiCreate = true;
              bCommentLine = true;
            } else {
              curBrick.iLineNumber = iLineNumber;
              sBuild.clear();
              parseRawCreateLine(sLine, sBuild, curBrick);
              bBrickEnded = true;
              bBrickStarted = false;
              sBuild.clear();
              bCommentLine = true;
            }
        // if length of name is 0, then we're in a define
        } else if(curBrick.sName.length() > 0) {
          if(curChar == '}') {
            iCurIndent--;
            if(iCurIndent < 0) {
              error({iLineNumber,iLineNumber}, "Too many '}'", "Verify '}' count");
            } else if(iCurIndent == 0) {
              bBrickEnded = true;
              bBrickStarted = false;
              mRawBricks[curBrick.sName] = curBrick;
              curBrick.clear();
              sBuild.clear();
              bCommentLine = true;
            }
          } else if(curChar == '{') {
            iCurIndent++;
          }
          sBuild.push_back(curChar);
        } else {
          parseRawDefineInitLine(sLine, sBuild, curBrick);
        }

      }
      if(sBuild.length() > 0) {
        curBrick.vContents.push_back({curBrick.iLineNumber,sBuild});
        sBuild.clear();
      }
    }
  }



  Log->workStackPop("buildRawBricks");
}

// TODO: this function
void nBasm::parseRawDefineInitLine(std::string& sLine, std::string& sBuild, nBasm::rawBrick& curBrick) {
  enum defineState {
    notDetermined,
    name,
    arguments,
    attributes,
    end
  };

  defineState curState = name;

  sBuild.clear();

  // foreach char in line
  for(int i = 0; i < sLine.length(); i++) {
    switch(curState) {
      case name:
        if(sBuild.length() > 0 && sBuild != "define") {
          curBrick.sName = sBuild;
          sBuild.clear();
          switch(sLine[i]) {
            case ' ':
              curState = notDetermined;
              break;
            case ':':
              curState = attributes;
              break;
            case '(':
              curState = arguments;
              break;
            case '{':
              curState = end;
              break;
            default:
              error({curBrick.iLineNumber,curBrick.iLineNumber}, "Compiler error, define function not started.", "Verify define line");
              break;
          }
        } else {
          sBuild.push_back(sLine[i]);
        }
        break;
      case arguments:
        switch(sLine[i]) {
          case ' ' | ',':
            if(sBuild.length() > 0) {
              curBrick.vInputs.push_back(sBuild);
              sBuild.clear();
            }
            break;
          case ')':
            if(sBuild.length() > 0) {
              curBrick.vInputs.push_back(sBuild);
              sBuild.clear();
            }
            curState = notDetermined;
            break;
          default:
            sBuild.push_back(sLine[i]);
            break;

        }
        break;
      case attributes:
        switch(sLine[i]) {
          case ' ' | ',':
            if(sBuild.length() > 0) {
              curBrick.vAtrributes.push_back(sBuild);
              sBuild.clear();
            }
            break;
          case '{':
            if(sBuild.length() > 0) {
              curBrick.vAtrributes.push_back(sBuild);
              sBuild.clear();
            }
            curState = end;
            break;
        }
        break;
      case end:
        break;
      case notDetermined:
          break;
    }
  }

  if(sBuild.length() > 0) {
    // TODO: handle later
    error({curBrick.iLineNumber,curBrick.iLineNumber}, "Compiler error, define function not completed.", "Verify define line");
  }

}

// TODO: this function
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
  if(iLines.x == iLines.y) {
    Log->e("BASM Error on line",iLines.x,"!\n",sMessage,"\nPossible solution:\n",sSolution,"\n");
  } else {
    Log->e("BASM Error on line(s)",iLines.x,"-",iLines.y,"!\n",sMessage,"\nPossible solution:\n",sSolution,"\n");
  }
  // TODO: add with Log->v current values of variables

  throw nullptr;

}

void nBasm::unspecifiedError(std::string sMessage, std::string sSolution) {
  Log->e("BASM Unspecified Error!\n",sMessage,"\nPossible solution:\n",sSolution,"\n");

  // TODO: add with Log->v current values of variables

  throw nullptr;
}
