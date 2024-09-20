#pragma once

#include "Util/util.hpp"
#include <filesystem>
#include <fstream>

struct logOptsStruct {
	bool
		bVerbose = false,
		bPrint = true,
		bThrowOnError = false;
	std::string sOutputLocation = "logs/";
};

class logClass {
public:

	static logOptsStruct Options;

	logClass() {
		sOutputFile = util::getCurrentDateTime();
		if(!std::filesystem::exists(Options.sOutputLocation)) {
			std::filesystem::create_directory(Options.sOutputLocation);
		}
		outFile.open(Options.sOutputLocation + sOutputFile);
	}

	~logClass() {
		outFile.close();
	}

	// Log Type "Normal"
	// Will always log
	template <typename... Args>
	static void n(Args... inputArgs) {
		if(Options.bPrint) { util::qPrint("N:",inputArgs...); }
		writeToFile("N:",inputArgs...);
	}

	// Log Type "Verbose"
	// Will only log with verbose option
	template <typename... Args>
	static void v(Args... inputArgs) {
		if(!Options.bVerbose) { return; }
		if(Options.bPrint) { util::qPrint("V:",inputArgs...); }
		writeToFile("V:",inputArgs...);
	}

	// Log Type "Warning"
	// Will log with warning tag
	template <typename... Args>
	static void w(Args... inputArgs) {
		if(Options.bPrint) { util::qPrint("W:",inputArgs...); }
		writeToFile("W:",inputArgs...);
	}

	// Log Type "Error"
	// Will log with error tag
	// Will throw with throw option
	template <typename... Args>
	static void e(Args... inputArgs) {
		if(Options.bPrint) { util::qPrint("E:",inputArgs...); }
		writeToFile("E:",inputArgs...);
		if(Options.bThrowOnError) {
			throw;
		}
	}
	
private:
	static std::string sOutputFile;
	static std::ofstream outFile;

	template <typename T,typename... Args>
	static void writeToFile(T arg, Args... args) {
		outFile << arg;
		outFile << " ";
		writeToFile(args...);
	}

	template <typename T>
	static void writeToFile(T arg) {
		outFile << arg;
		outFile << "\n";
	}
};

static logClass* Log = nullptr;

