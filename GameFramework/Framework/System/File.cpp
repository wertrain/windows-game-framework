#include <fstream>
#include <iostream>
#include <string>
#include <cstdint>

#include "File.h"

namespace Framework {
namespace System {

void* File::Read(const char* filename, size_t& readsize)
{
    std::ifstream ifs(filename);
    
    if (!ifs.is_open())
    {
        readsize = 0;
        return nullptr;
    }

    ifs.seekg(0, std::ios::end);
    auto eofpos = ifs.tellg();
    ifs.clear();
    ifs.seekg(0, std::ios::beg);
    auto begpos = ifs.tellg();
    auto filesize = eofpos - begpos;
   
    void* buffer = new uint8_t[filesize];
    ifs.read((char*)buffer, filesize);
    readsize = filesize; 
    ifs.close();

    return buffer;
}

size_t File::GetSize(const char* filename)
{
    std::ifstream ifs(filename);

    ifs.seekg(0, std::ios::end);
    auto eofpos = ifs.tellg();
    ifs.clear();
    ifs.seekg(0, std::ios::beg);
    auto begpos = ifs.tellg();
    return eofpos - begpos;
}

} // namespace System
} // namespace Framework
