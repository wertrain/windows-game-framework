#ifndef FILE_H_
#define FILE_H_

NS_FW_SYS_BEGIN

/// <summary>
/// バイナリファイルリーダー
/// </summary>
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

NS_FW_SYS_END

#endif // FILE_H_
