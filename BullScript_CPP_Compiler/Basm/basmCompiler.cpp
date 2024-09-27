#include "basmCompiler.hpp"
#include <algorithm>
#include <sqlite3.h>
#include "../logger.hpp"

std::map<std::string,basm::brick> basm::mBricks {};
std::map<std::string,basm::asmTranslation> basm::mTranslations {};
std::map<std::string,basm::asmValue> basm::mValues {};
std::unordered_set<std::string> basm::verifiedDefined {};
std::unordered_set<std::string> basm::currentBranches {};
std::unordered_set<std::string> basm::currentDefines {};
std::vector<std::string> basm::vSection_data;
std::vector<std::string> basm::vSection_bss;
std::vector<std::string> basm::vSection_rodata;
std::vector<std::string> basm::vSection_text;

void basm::compileFromFile(std::string sFile) {
  loadTranslations();
  printTranslations();

  buildBricksFromFile(sFile);
  verifyEntryAndExitBricks();

  Log->n("Starting branch out...");
  branchOutFromBrick(mBricks["entry"]);
  // Log->n("Branch out completed.");

}

void basm::defineBrick(basm::brick& currentBrick) {
  Log->v(currentBrick.sName,"defining started...");

  if(currentDefines.contains(currentBrick.sName)) {
    Log->v(currentBrick.sName,"is already being defined.");
    return;
  }

  currentDefines.insert(currentBrick.sName);

  std::string sBuild = "";
  if(currentBrick.sKeyword == "create") {
    std::string aLineStart[2];
    std::string sFinalLine = "";
    bool bConst = false;
    sBuild.clear();

    aLineStart[0] = currentBrick.sName + ": ";
    aLineStart[1] = "";
    for(auto& i : currentBrick.vAttributes) {
      if(i == "const") {
        bConst = true;
      } else if(i.ends_with('b')) {
        if(aLineStart[1].length() > 0) {
          error("Attribute bit size for " + currentBrick.sName + " is defined twice.", "Only specify a single size of 8b 16b 32b 64b.");
        }
        aLineStart[1] = mTranslations.at(i).x86_64 + " ";
      } else {
        sBuild.append(i + " ");
      }
    }
    sFinalLine.append(aLineStart[0] + aLineStart[1] + sBuild);

    if(currentBrick.vContents.size() < 1) {
      vSection_bss.push_back(sFinalLine);
    } else {
      for(auto& i : currentBrick.vContents) {
        // TODO: account for "\n" here
        sFinalLine.append(i + " ");
      }
      if(bConst) {
        vSection_rodata.push_back(sFinalLine);
      } else {
        vSection_data.push_back(sFinalLine);
      }
    }

  } else if(currentBrick.sKeyword == "define") {

  } else if(currentBrick.sKeyword == "fn") {

  }

  currentDefines.erase(currentBrick.sName);

  Log->v(currentBrick.sName,"defined.");
}

void basm::branchOutFromBrick(basm::brick& branchBrick) {
  Log->v(branchBrick.sName, "branch called...");

  if(currentBranches.contains(branchBrick.sName)) {
    Log->v(branchBrick.sName, "branch already being worked on.");
    return;
  }

  currentBranches.insert(branchBrick.sName);

  if(branchBrick.sKeyword == "define"
  || branchBrick.sKeyword == "fn"
  || branchBrick.sKeyword == "create") {
    defineBrick(branchBrick);
  } else if(!verifiedDefined.contains(branchBrick.sKeyword)) {
    if(!mBricks.contains(branchBrick.sKeyword)) {
      error(branchBrick.sKeyword + " could not be found when attempting to define.", "Verify \"" + branchBrick.sKeyword + "\" is defined in the source or in the compiler database.");
    } else {
      branchOutFromBrick(mBricks.at(branchBrick.sKeyword));
    }
  }

  if(!verifiedDefined.contains(branchBrick.sName)) {
    defineBrick(branchBrick);
  }

  currentBranches.erase(branchBrick.sName);

  Log->v(branchBrick.sName,"branch completed.");

}

void basm::loadTranslations() {
  Log->n("Loading translations...");

  sqlite3* db;
  sqlite3_stmt* statement;
  std::string sName;

  Log->v("Attempting to open SQLite DB file...");
  int openDB = sqlite3_open("Basm/basmTranslation.db", &db);
  if(openDB != SQLITE_OK) {
    std::string err = sqlite3_errmsg(db);
    sqlite3_close(db);
    error("Error loading basmTranslation DB! Error: " + err, "Verify DB file exists with name \"basmTranslation.db\"");
  }
  Log->v("SQLite DB file:","Basm/basmTranslation.db","successfully opened.");

  Log->v("Attemping to prepare DB Main table...");
  openDB = sqlite3_prepare_v2(db,"SELECT * FROM Main;",-1,&statement,NULL);
  if(openDB != SQLITE_OK) {
    std::string err = sqlite3_errmsg(db);
    sqlite3_close(db);
    error("Error preparing basmTranslation DB Main Table! Error: " + err, "Verify prepare statement in compiler's code.");
  }
  Log->v("DB Main table successfully prepared.");

  Log->v("Stepping thru Main table...");
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
  Log->v("Main table finalized.");

  Log->v("Attempting to prepare DB Vals table...");
  openDB = sqlite3_prepare_v2(db,"SELECT * FROM Vals;",-1,&statement,NULL);
  if(openDB != SQLITE_OK) {
    std::string err = sqlite3_errmsg(db);
    sqlite3_close(db);
    error("Error preparing basmTranslation DB Values Table! Error: " + err, "Verify prepare statement in compiler's code.");
  }
  Log->v("DB Vals table successfully prepared.");

  Log->v("Stepping thru Vals table...");
  asmValue valToAdd;
  while((openDB = sqlite3_step(statement)) == SQLITE_ROW) {
    sName.clear();
    sName = std::string(reinterpret_cast<const char*>(sqlite3_column_text(statement, 0)));
    valToAdd.x86_64 = sqlite3_column_int(statement,1);
    mValues[sName] = valToAdd;
  }
  sqlite3_finalize(statement);
  Log->v("Vals table finalized.");

  Log->v("Closing DB.");
  sqlite3_close(db);
  Log->v("DB closed.");

  Log->n("Finished loading translations.");
}

void basm::printTranslations() {
  Log->v("Printing translation tables...");
  Log->v("\n");
  Log->v("Main table:");
  for(auto& i : mTranslations) {
    Log->v(i.second.type,i.first,i.second.x86_64);
  }
  Log->v("\n");
  Log->v("Value table:");
  for(auto& i : mValues) {
    Log->v(i.first,i.second.x86_64);
  }
  Log->v("\n");
  Log->v("Finished printing translation tables.");
}

void basm::verifyEntryAndExitBricks() {
  Log->v("Verifying Entry and Exit bricks exist...");
  bool bEntry = false, bExit = false;
  for(auto& b : mBricks) {
    if(b.first == "entry") {
      bEntry = true;
      Log->v("Entry function found!");
    }
    if(b.first == "exit") {
      bExit = true;
      Log->v("Exit function found!");
    }
    if(bEntry && bExit) break;
  }
  if(bEntry && bExit) {
    // TODO: need to deal with multiple definitions for entry
    // possibly just throw if defined multiple times
    Log->v("Entry and Exit bricks exist.");
    for(auto& c : mBricks["entry"].vContents) {
      if(c == "exit") return;
    }
    // will auto include the exit call if excluded in the entry function
    mBricks["entry"].vContents.push_back("exit");
    return;
  }
  else if(bEntry){
    basm::error("Exit Function not found!", "Define Exit function.");
  } else {
    basm::error("Entry Function not found!", "Define Entry function.");
  }
  basm::error("Verify Entry and Exit Bricks function critical failure! Function should have returned or thrown but did not!", "I don't man, you should never have gotten here *shrug*");
}

void basm::buildBricksFromFile(std::string sFile) {
  Log->n("Building bricks from file:",sFile);
  std::string
    sLine,
    sBuild,
    sCurrentBrickName,
    sCurrentBrick;
  bool bBrickStarted = false, bCompleteBrick = false, bMultiLineBrick = false;
  char curChar;
  int iLineLength;

  Log->v("Reading file by line...");
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
  Log->v("Finished reading file by line.");

  Log->n("All bricks finished building.");
}

// With verbose will print each brick without having to call this function
// This function can be used to print all after the fact.
void basm::printAllBricks() {
  for (auto &m : mBricks) {
    printBrick(m.second);
  }
}

void basm::buildBrick(std::string sBrickName, std::string sBrickRawContents, bool bMultiline) {
  Log->v("Building brick...");
  brick outputBrick;
  outputBrick.sKeyword = sBrickName;
  outputBrick.sName = "UNDEFINED";

  std::string sBuild = "";
  char currentChar;

  if (sBrickName == "create") {
    if (bMultiline) {
      Log->v("Brick is multiline, building per line...");
      for (auto &line : util::splitStringOnChar(sBrickRawContents, '\n')) {
        buildBrick("create", line, false);
      }
      Log->v("Brick multiline finished.");
      return;
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
    bool bInContent = false;

    for (int i = 0; i < sBrickRawContents.length(); i++) {
      currentChar = sBrickRawContents[i];
      if (bInContent) {
        if (currentChar == '\n') {
          if (sBuild.length() > 0) {
            outputBrick.vContents.push_back(sBuild);
          }
          sBuild.clear();
        } else {
          sBuild.push_back(currentChar);
        }
      } else if (currentChar == '\n') {
        if(outputBrick.sName != "UNDEFINED") {
          bInContent = true;
        }
      } else if (currentChar == ' ' || currentChar == '<') {
        if(sBuild.length() > 0) {
          if(outputBrick.sName == "UNDEFINED") {
            outputBrick.sName = sBuild;
          } else {
            outputBrick.vAttributes.push_back(sBuild);
          }
          sBuild.clear();
        }
      } else if (currentChar != ':') {
        sBuild.push_back(currentChar);
      }
    }

    if (sBuild.length() > 0 && !util::onlyContains(sBuild, ' ')) {
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
    if (sBuild.length() > 0 && !util::onlyContains(sBuild, ' ')) {
      outputBrick.vContents.push_back(sBuild);
      sBuild.clear();
    }
  }

  if (outputBrick.sName != "UNDEFINED") {
    mBricks[outputBrick.sName] = outputBrick;
    Log->v("Brick",outputBrick.sName,"built successfully and added to mBricks. Full brick:");
    printBrick(outputBrick);
  } else {
    Log->w("Brick with name 'UNDEFINED' has contents:","\n" + sBrickRawContents);
    Log->w("Brick was not added to mBricks.");
    Log->w("\n");
  }
}

void basm::sanitizeRawBrickData(std::string &sBrickName, std::string &sBrickRawContents) {
  Log->v("Sanitizing Raw Brick:",sBrickName,"with contents:","\n" + sBrickRawContents,"\n");
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
  Log->v("Sanitized to:","\n" + sBrickRawContents);
}

void basm::error(std::string sMessage, std::string sSolution) {
  Log->e("Basm Error!\n",sMessage,"\nPossible solution:\n",sSolution,"\n\n");
}

void basm::printBrick(basm::brick& toPrint) {
  Log->v("NAM",toPrint.sName);
  Log->v("KEY",toPrint.sKeyword);
  for (auto &i : toPrint.vAttributes) {
    Log->v("ATT", i);
  }
  for (auto &i : toPrint.vContents) {
    Log->v("CON",i);
  }
  Log->v("\n");
}
