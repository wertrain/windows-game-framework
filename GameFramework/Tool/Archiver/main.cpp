#include <iostream>
#include "Includes.h"

int main(const int argc, const char*argv[])
{
    std::cout << "Usage:" << std::endl;
    std::cout << "    Archiver.exe [-a] [-e] <file> ..." << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "    -a   指定されたファイルを圧縮する" << std::endl;
    std::cout << "    -e   指定されたファイルを展開する" << std::endl;
    return 0;
}