#pragma once

#include <stack>
#include <string>
#include <unordered_set>
#include <vector>
#include <map>
#include "../Util/util.hpp"


// TODO: need to reorder which functions/vars are public/private
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
		SLIT = 8,	// string literal
		XLIT = 9,	// complex string literal (must be translated to rodata)
		VLIT = 10,	// value literal
		GRP = 11	// group (inside parentheses)
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
		bool bFunction;
		unitInstructions(std::vector<itemInfo>& unit) :
			firstItem(unit.at(0)),
			bFunction(unit.at(0).type == FN)
		{};
	};

	static std::map<std::string,brick> mBricks;
	static std::map<std::string,asmTranslation> mTranslations;
	static std::map<std::string,asmValue> mValues;
	static std::map<std::string,std::vector<std::string>> mTranslatedDefinitions;
	static std::map<std::string,std::string>
		mSLITs,
		mXLITs;

	static std::unordered_set<std::string> verifiedDefined;
	static std::unordered_set<std::string> currentBranches;
	static std::unordered_set<std::string> currentDefines;
	static std::unordered_set<std::string> inlineFuncs;
	static std::vector<std::string> vSection_data;
	static std::vector<std::string> vSection_bss;
	static std::vector<std::string> vSection_rodata;
	static std::vector<std::string> vSection_text;
	
	static std::stack<std::string> workStack;

	static void compileFromFile(std::string sFile);

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
	static std::string
		getFnName(std::string sBrickName),
		resolveItem(std::vector<std::string>& outputRef,itemInfo itemToResolve);
	static itemInfo
		getItemInfo(std::string sItem),
		resolveSubUnits(std::vector<std::string>& outputRef,std::vector<itemInfo>& subUnits);
	static std::vector<util::int2d> getItemGroups(std::vector<itemInfo>& translationUnit);

	// Specific Resolutions
	static void
		resolve_syscall(std::vector<std::string>& outputRef, std::vector<itemInfo>& translationUnit);
};
