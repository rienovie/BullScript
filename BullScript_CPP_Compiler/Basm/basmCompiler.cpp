#include "basmCompiler.hpp"
#include <algorithm>
#include <sqlite3.h>

std::map<std::string,std::vector<basm::brick>> basm::mBricks {};
std::map<std::string,basm::asmTranslation> basm::mTranslations {};
std::map<std::string,basm::asmValue> basm::mValues {};

void basm::compileFromFile(std::string sFile) {
  loadTranslations();
  buildBricksFromFile(sFile);
  verifyEntryAndExitBricks();
  printTranslations();
}

void basm::loadTranslations() {
  sqlite3* db;
  sqlite3_stmt* statement;
  std::string sName;

  int openDB = sqlite3_open("../Basm/basmTranslation.db", &db);
  if(openDB != SQLITE_OK) {
    std::string err = sqlite3_errmsg(db);
    sqlite3_close(db);
    error("Error loading basmTranslation DB! Error: " + err, "Verify DB file exists with name \"basmTranslation.db\"");
  }
  openDB = sqlite3_prepare_v2(db,"SELECT * FROM Main;",-1,&statement,NULL);
  if(openDB != SQLITE_OK) {
    std::string err = sqlite3_errmsg(db);
    sqlite3_close(db);
    error("Error preparing basmTranslation DB Main Table! Error: " + err, "Verify prepare statement in compiler's code.");
  }

  asmTranslation transToAdd;
  while((openDB = sqlite3_step(statement)) == SQLITE_ROW) {
    transToAdd.x86_64.clear();
    sName.clear();
    transToAdd.type = static_cast<keywordType>(sqlite3_column_int(statement, 1));
    if(transToAdd.type != UNIQUE) {
      transToAdd.x86_64 = std::string(reinterpret_cast<const char*>(sqlite3_column_text(statement, 2)));
    }
    sName = std::string(reinterpret_cast<const char*>(sqlite3_column_text(statement, 0)));
    mTranslations[sName] = transToAdd;
  }
  sqlite3_finalize(statement);

  openDB = sqlite3_prepare_v2(db,"SELECT * FROM Vals;",-1,&statement,NULL);
  if(openDB != SQLITE_OK) {
    std::string err = sqlite3_errmsg(db);
    sqlite3_close(db);
    error("Error preparing basmTranslation DB Values Table! Error: " + err, "Verify prepare statement in compiler's code.");
  }

  asmValue valToAdd;
  while((openDB = sqlite3_step(statement)) == SQLITE_ROW) {
    sName.clear();
    sName = std::string(reinterpret_cast<const char*>(sqlite3_column_text(statement, 0)));
    valToAdd.x86_64 = sqlite3_column_int(statement,1);
    mValues[sName] = valToAdd;
  }
  sqlite3_finalize(statement);

  sqlite3_close(db);
}

void basm::printTranslations() {
  util::qPrint("\nMain table:");
  for(auto& i : mTranslations) {
    util::qPrint(i.second.type,i.first,i.second.x86_64);
  }
  util::qPrint("\nValue table:");
  for(auto& i : mValues) {
    util::qPrint(i.first,i.second.x86_64);
  }
  util::qPrint("\n");
}

void basm::verifyEntryAndExitBricks() {
  bool bEntry = false, bExit = false;
  for(auto& b : mBricks) {
    if(b.first == "entry") {
      bEntry = true;
      util::qPrint("Entry function found!");
    }
    if(b.first == "exit") {
      bExit = true;
      util::qPrint("Exit Function found!");
    }
    if(bEntry && bExit) break;
  }
  if(bEntry && bExit) return;
  else if(bEntry){
    basm::error("Exit Function not found!", "Define Exit function.");
  } else {
    basm::error("Entry Function not found!", "Define Entry function.");
  }
  basm::error("Verify Entry and Exit Bricks function critical failure! Function should have returned or thrown but did not!", "I don't man, you should never have gotten here *shrug*");
}

void basm::buildBricksFromFile(std::string sFile) {
  std::string
    sLine,
    sBuild,
    sCurrentBrickName,
    sCurrentBrick;
  bool bBrickStarted = false, bCompleteBrick = false, bMultiLineBrick = false;
  char curChar;
  int iLineLength;

  MACRO_ReadFileByLine(sFile, sLine, {
    iLineLength = sLine.length();
    sBuild.clear();
    for (int i = 0; i < iLineLength; i++) {
      curChar = sLine[i];
      if (curChar == ';') {
        if (bBrickStarted && !bMultiLineBrick) {
          bCompleteBrick = true;
        }
        break;
      }
      sBuild.push_back(curChar);
      if (bBrickStarted) {
        if (bMultiLineBrick) {
          if (sBuild == "end" && sLine.find(sCurrentBrickName) != sLine.npos) {
            bCompleteBrick = true;
            break; // leave line
          }
        } else {
          if (curChar == ':') {
            bMultiLineBrick = true;
            break;
          }
          if (i == iLineLength - 1) {
            bCompleteBrick = true;
            break;
          }
        }
      } else {
        if (sBuild.length() > 1) {
          if (curChar == ' ' || curChar == ':') {
            sBuild.pop_back();
            sCurrentBrickName = sBuild;
            sBuild.clear();
            bBrickStarted = true;
            if (curChar == ':') {
              bMultiLineBrick = true;
            }
          }
        }
      }
    }

    if (bBrickStarted) {
      sCurrentBrick.push_back('\n');
      if (bCompleteBrick) {
        if (!bMultiLineBrick) {
          sCurrentBrick.append(sBuild);
        }
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

  for (auto &m : mBricks) {
    for (auto &b : m.second) {
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

  if (sBrickName == "create") {
    if (bMultiline) {
      for (auto &line : util::splitStringOnChar(sBrickRawContents, '\n')) {
        buildBrick("create", line, false);
      }
    } else {
      bool bEqualOnLine = util::contains(sBrickRawContents, '=');
      bool bInsideParen = false;
      for (int i = sBrickRawContents.length() - 1; i > -1; i--) {
        currentChar = sBrickRawContents[i];
        if (bInsideParen) {
          if (i == 0) {
            std::string sErrorOut;
            if (currentChar == '\"') {
              sErrorOut.append(
                  "Found 0 index \" on a line while trying to build brick ");
            } else {
              sErrorOut.append(
                  "Error parsing with a \" while trying to build brick ");
            }
            sErrorOut.append(sBrickName);
            sErrorOut.append("\n\nBrick Contents:\n");
            sErrorOut.append(sBrickRawContents);

            error(sErrorOut,
                  "Verify Brick " + sBrickName + " has proper quotation");
          } else if (currentChar == '"' && sBrickRawContents[i - 1] != '\\') {
            bInsideParen = false;
          }
          sBuild.push_back(currentChar);
        } else if (currentChar == '\"') {
          bInsideParen = true;
          sBuild.push_back(currentChar);
        } else if ((currentChar == ' ' || currentChar == '=') && sBuild.length() > 0) {
          if (bEqualOnLine && i >= sBrickRawContents.find('=')) {
            std::reverse(sBuild.begin(), sBuild.end());
            outputBrick.vContents.push_back(sBuild);
            sBuild.clear();
          } else if (outputBrick.sName == "UNDEFINED") {
            std::reverse(sBuild.begin(), sBuild.end());
            outputBrick.sName = sBuild;
            sBuild.clear();
          } else {
            std::reverse(sBuild.begin(), sBuild.end());
            outputBrick.vAttributes.push_back(sBuild);
            sBuild.clear();
          }
        } else {
          if (currentChar != ' ' && currentChar != '=')
            sBuild.push_back(currentChar);
          if (i == 0) {
            std::reverse(sBuild.begin(), sBuild.end());
            outputBrick.vAttributes.push_back(sBuild);
          }
        }
      }
    }
  } else if (sBrickName == "define") {
    std::vector<std::string> vElements;
    bool bInContent = false;

    for (int i = 0; i < sBrickRawContents.length(); i++) {
      currentChar = sBrickRawContents[i];
      if (bInContent) {
        if (currentChar == '\n') {
          if (sBuild.length() > 0)
            outputBrick.vContents.push_back(sBuild);
          sBuild.clear();
        } else if (currentChar != ':')
          sBuild.push_back(currentChar);

      } else if ((currentChar == ' ' || currentChar == '<') && sBuild.length() > 0) {
        if (outputBrick.sName == "UNDEFINED")
          outputBrick.sName = sBuild;
        else {
          outputBrick.vAttributes.push_back(sBuild);
          bInContent = true;
        }
        sBuild.clear();
      } else if (currentChar != ' ' && currentChar != '<')
        sBuild.push_back(currentChar);
    }

    if (sBuild.length() > 0) {
      outputBrick.vContents.push_back(sBuild);
      sBuild.clear();
    }
  } else {
    bool bFirstLine = true;
    for (int i = 0; i < sBrickRawContents.length(); i++) {
      currentChar = sBrickRawContents[i];
      if (bFirstLine && currentChar == '\n')
        bFirstLine = false;
      if (outputBrick.sName == "UNDEFINED" &&
          (currentChar == ':' || currentChar == ' ') && (sBuild.length() > 0)) {
        outputBrick.sName = sBuild;
        sBuild.clear();
      } else if (bFirstLine) { // only checks for att if first line
        if (sBuild.length() > 0 && currentChar == ':') {
          outputBrick.vAttributes.push_back(sBuild);
          sBuild.clear();
        } else if (currentChar == ' ' && sBuild.length() > 0) {
          outputBrick.vAttributes.push_back(sBuild);
          sBuild.clear();
        } else if (currentChar != '<' && currentChar != ':' &&
                   currentChar != ' ') {
          sBuild.push_back(currentChar);
        }
      } else if (currentChar == '\n' && sBuild.length() > 0) {
        outputBrick.vContents.push_back(sBuild);
        sBuild.clear();
      } else if (currentChar != '\n') {
        sBuild.push_back(currentChar);
      }
    }
    if (sBuild.length() > 0) {
      outputBrick.vContents.push_back(sBuild);
      sBuild.clear();
    }
  }

  if (outputBrick.sName != "UNDEFINED")
    mBricks[outputBrick.sName].push_back(outputBrick);
}

void basm::sanitizeRawBrickData(std::string &sBrickName, std::string &sBrickRawContents) {
  util::removeAllOfChar(sBrickRawContents, '\t');

  std::string sBuild = "", sOutput;
  int iBuildLength = 0;
  for (int i = 0; i < sBrickRawContents.length(); i++) {
    sBuild.push_back(sBrickRawContents[i]);
    iBuildLength = sBuild.length();

    if (sBrickRawContents[i] == '\n') {
      if (iBuildLength < 3) {
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

  if (iBuildLength > 2)
    sOutput.append(sBuild);

  if (sOutput[sOutput.length() - 1] == '\n')
    sOutput.pop_back();
  sBrickRawContents = sOutput;
}

void basm::error(std::string sMessage, std::string sSolution) {
  util::qPrint("Basm Error!\n", sMessage);
  util::qPrint("\nPossible solution:\n", sSolution, "\n\n");
  throw;
}

void basm::printBrick(basm::brick toPrint) {
  util::qPrint("NAM", toPrint.sName);
  util::qPrint("KEY", toPrint.sKeyword);
  for (auto &i : toPrint.vAttributes) {
    util::qPrint("ATT", i);
  }
  for (auto &i : toPrint.vContents) {
    util::qPrint("CON", i);
  }
  util::qPrint("\n");
}
