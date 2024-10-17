#pragma once

#include <string>
#include <vector>
#include <map>

#include "../Util/util.hpp"

class basm {
public:
	struct brick {
		// NOTE: originally wanted to have line numbers for errors and such, however
		// basm will be under bullscript and if you get an error in basm giving a line number
		// it would add confusion when working with bullscript
		std::string sKeyword;
		std::vector<std::string> vAttributes;
		std::string sName;
		std::vector<std::string> vContents;
	};

	// NOTE: will probably add more later
	enum keywordType { INSTRUCTION = 0, REGISTER = 1, SUBSTITUTION = 2, UNIQUE = 3,};

	enum itemType {
		INS = 0,	// instruction
		REG = 1,	// register
		SUB = 2,	// substitution
		UNI = 3,	// unique
		VAL = 4,	// value
		FN = 5,		// function
		IFN = 6,	// inline function
		VAR = 7,	// variable
		LIT = 8,	// literal
		XLIT = 9	// complex literal (must be translated to rodata)
	};

	struct asmTranslation {
		keywordType type;
		std::string x86_64;

		// NOTE: will add ARM and others later
	};
	struct asmValue {
		int x86_64;
		// NOTE: will add ARM and others later
	};

	struct itemInfo {
		itemType type;
		std::string
			name,
			translatedValue,
			prepend,
			append;
	};

	struct unitInstructions {
		itemInfo firstItem;
		bool
			bFunction,
			bMulti; // multi keyword not multilined
		unitInstructions(std::vector<itemInfo>& unit, bool multiLine = false)
		:
			firstItem(unit.at(0)),
			bFunction(unit.at(0).type == FN),
			bMulti(multiLine)
		{};
	};

	static std::map<std::string,brick> mBricks;
	static std::map<std::string,asmTranslation> mTranslations;
	static std::map<std::string,asmValue> mValues;
	static std::map<std::string,std::vector<std::string>> mTranslatedDefinitions;
	static std::unordered_set<std::string> verifiedDefined;
	static std::unordered_set<std::string> currentBranches;
	static std::unordered_set<std::string> currentDefines;
	static std::unordered_set<std::string> inlineFuncs;
	static std::vector<std::string> vSection_data;
	static std::vector<std::string> vSection_bss;
	static std::vector<std::string> vSection_rodata;
	static std::vector<std::string> vSection_text;
	

	static void compileFromFile(std::string sFile);
	static itemInfo getItemInfo(std::string sItem);

private:

	static void
		buildBrick(std::string sBrickName, std::string sBrickRawContents, bool bMultiline),
		defineBrick(brick& currentBrick),
		defineFunctionContents(brick& currentBrick),
		sanitizeRawBrickData(std::string& sBrickName, std::string& sBrickRawContents),
		error(std::string sMessage, std::string sSolution),
		printBrick(brick& toPrint),
		buildBricksFromFile(std::string sFile),
		verifyEntryAndExitBricks(),
		loadTranslations(),
		printTranslations(),
		printAllBricks(),
		branchOutFromBrick(brick& branchBrick);
	static std::vector<std::string>
		translateMultiUnit(unitInstructions uIns,std::vector<std::string> vLines),
		translateUnit(unitInstructions uIns,std::vector<itemInfo> unitToTranslate);
	static std::string getFnName(std::string sBrickName);
};
