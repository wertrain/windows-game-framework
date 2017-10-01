#ifndef FILE_H_
#define FILE_H_

namespace Framework {
namespace System {
namespace File {

class Binary
{
public:
    Binary();
    Binary(const wchar_t* filepath);
    bool Read(const wchar_t* filepath);
    const void* Get() const;
    size_t Size() const;
private:
    size_t mSize;
    std::unique_ptr<char> mData;
};

} // namespace File
} // namespace System
} // namespace Framework

#endif // FILE_H_
