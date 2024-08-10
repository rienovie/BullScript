#pragma once

#include <string>
#include <vector>
#include <map>

#include "../Util/util.hpp"

class basm {
public:
	struct brick {
		std::string sKeyword;
		std::vector<std::string> vAttributes;
		std::string sName;
		std::vector<std::string> vContents;
	};

	static std::map<std::string,std::vector<brick*>> mBricks;

	static void compileFromFile(std::string sFile);

private:
	static void buildBrick(std::string sBrickName, std::string sBrickRawContents, bool bMultiline);
	static void sanitizeRawBrickData(std::string& sBrickName, std::string& sBrickRawContents);
};
