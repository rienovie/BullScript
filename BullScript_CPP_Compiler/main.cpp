#include <iostream>
#include "Util/util.hpp"
#include <string>

int main(int argc, char *argv[]) {
    std::string sLine;
    int iLine = 0;
    MACRO_ReadFileByLine("../main.cpp",sLine,{
        iLine++;
        util::qPrint(iLine,"|",sLine);
    });

    return 0;
}
