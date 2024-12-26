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
	// this is the basic block the raw basm file data will be put into before further translation
	struct brick {
		// NOTE: originally wanted to have line numbers for errors and such, however
		// basm will be under bullscript and if you get an error in basm giving a line number
		// it would add confusion when working with bullscript
		std::string sKeyword;
		std::vector<std::string> vAttributes;
		std::string sName;
		std::vector<std::string> vContents;
	};

	// These are based on the db file translation types
	// NOTE: will probably add more later
	enum keywordType { INSTRUCTION = 0, REGISTER = 1, SUBSTITUTION = 2, UNIQUE = 3,};

	// Everything should be covered by this, if you find something missing let me know
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

	// from db file
	struct asmTranslation {
		keywordType type;
		std::string x86_64;

		// NOTE: will add ARM and others later
	};
	struct asmValue {
		int x86_64;
		// NOTE: will add ARM and others later
	};

	// Not all items will have all fields valued
	struct itemInfo {
		itemType type;
		std::string
			name,
			translatedValue,
			prepend,
			append;
	};

	// used to determine how to hand a unit
	// NOTE: units are basically a line but can also be multilined and sublined
	// example:
	//						unit
	//		\/------------------------------------\/
	//		subtract regA someFunction someInput, 15
	//		         ^--------------------------^
	//		                    subUnit
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
		// puts the basm data into workable variables
		buildBrick(std::string sBrickName, std::string sBrickRawContents, bool bMultiline),

		// brick must be included in final program / define also translates
		defineBrick(brick& currentBrick),
		defineFunctionContents(brick& currentBrick),

		// removes extra spaces / tabs from raw contents of a brick
		sanitizeRawBrickData(std::string& sBrickName, std::string& sBrickRawContents),

		// logs an error message and also throws with the intention of crashing
		error(std::string sMessage, std::string sSolution),

		// TODO: currently used in logging verbose, change this only to utility
		printBrick(brick& toPrint),

		// the current root function to call
		buildBricksFromFile(std::string sFile),

		// makes sure the program has an enter and exit
		verifyEntryAndExitBricks(),

		// calls the info from the sqlite db file
		// HACK: for my debugger I need to compile with a different location
		// the switch for it is in program main
		loadTranslations(),

		// list the values aquired from the db file
		printTranslations(),

		// self explanitory I think :)
		printAllBricks(),

		// program will only build with the values/functions that are actually used
		// NOTE: this will call define on any bricks or sub-bricks put thru this
		// for the main program should only need to call this on "entry"
		branchOutFromBrick(brick& branchBrick);

	static std::vector<std::string>
		splitLineToItems(std::string& sLineToSplit),
		translateMultiUnit(std::vector<std::string> vLines),
		translateUnit(std::vector<itemInfo> unitToTranslate);
	static std::string
		getFnName(std::string sBrickName),
		resolveItem(std::vector<std::string>& outputRef,itemInfo itemToResolve),
		resolveSubItems(std::vector<std::string>& outputRef,std::vector<itemInfo>& subItems);
	static itemInfo getItemInfo(std::string sItem);
	static std::vector<util::int2d> getItemGroups(std::vector<itemInfo>& translationUnit);

	// Specific Resolutions
	static void
		resolve_syscall(std::vector<std::string>& outputRef, std::vector<itemInfo>& translationUnit);
};
