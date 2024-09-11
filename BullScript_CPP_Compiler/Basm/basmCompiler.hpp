#pragma once

#include <string>
#include <vector>
#include <map>

#include "../Util/util.hpp"

class basm {
public:
	// TODO: give bricks line numbers so error messages can be more useful
	struct brick {
		std::string sKeyword;
		std::vector<std::string> vAttributes;
		std::string sName;
		std::vector<std::string> vContents;
	};

	// NOTE: will probably add more later
	enum keywordType { INSTRUCTION = 0, REGISTER = 1, SUBSTITUTION = 2, UNIQUE = 3,};

	struct asmTranslation {
		keywordType type;
		std::string x86_64;

		// NOTE: will add ARM and others later
	};
	struct asmValue {
		int x86_64;
		// NOTE: will add ARM and others later
	};

	static std::map<std::string,brick> mBricks;
	static std::map<std::string,asmTranslation> mTranslations;
	static std::map<std::string,asmValue> mValues;
	static std::unordered_set<std::string> verifiedDefined;
	

	static void compileFromFile(std::string sFile);

private:
	static void
		buildBrick(std::string sBrickName, std::string sBrickRawContents, bool bMultiline),
		sanitizeRawBrickData(std::string& sBrickName, std::string& sBrickRawContents),
		error(std::string sMessage, std::string sSolution),
		printBrick(brick toPrint),
		buildBricksFromFile(std::string sFile),
		verifyEntryAndExitBricks(bool bPrintIfFound = true),
		loadTranslations(),
		printTranslations(),
		printAllBricks(),
		branchOutFromBrick(brick branchBrick);
};
