#pragma once

#include <string>
#include <utility>
#include <vector>
#include <unordered_map>

#include "../Util/util.hpp"

class nBasm {
public:
	
	struct rawBrick {
		// first line of brick
		int iLineNumber;
		// if false then it's a define
		bool bCreate;
		std::string sName;
		std::vector<std::string> vInputs;
		std::vector<std::string> vAtrributes;
		// int is the line number
		std::vector<std::pair<int, std::string>> vContents;
	};

	static void compileFromString(std::string sSource);

private:
	static std::unordered_map<std::string,rawBrick> mRawBricks;

	static void
		buildRawBricks(std::string& sSource),
		// NOTE: prioritize using this function over unspecifiedError
		error(util::int2d iLines, std::string sMessage, std::string sSolution),
		// NOTE: avoid using this function, maybe will be removed if not used
		unspecifiedError(std::string sMessage, std::string sSolution);
};
