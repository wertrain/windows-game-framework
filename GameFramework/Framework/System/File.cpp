#include <fstream>
#include <iostream>
#include <cstdint>
#include <memory>

#include "File.h"

namespace Framework {
namespace System {
namespace File {

Binary::Binary()
    : mSize(0)
    , mData(nullptr)
{

}

Binary::Binary(const wchar_t* filepath)
{
    Read(filepath);
}

bool Binary::Read(const wchar_t* filepath)
{
    std::ifstream binfile(filepath, std::ios::in | std::ios::binary);

    if (binfile.is_open())
    {
        int fsize = static_cast<int>(binfile.seekg(0, std::ios::end).tellg());
        binfile.seekg(0, std::ios::beg);
        std::unique_ptr<char> code(new char[fsize]);
        binfile.read(code.get(), fsize);
        mSize = fsize;
        mData = std::move(code);
        return true;
    }
    return false;
}

const void* Binary::Get() const
{
    return mData.get();
}

size_t Binary::Size() const
{
    return mSize;
}

} // namespace File
} // namespace System
} // namespace Framework
