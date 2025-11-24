#pragma once

#include <string>
#include <utility>
#include <vector>
#include <unordered_map>

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

	static void buildRawBricks(std::string sSource);
};
