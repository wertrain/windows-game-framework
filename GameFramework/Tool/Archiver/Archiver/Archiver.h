#ifndef ARCHIVER_H_
#define ARCHIVER_H_

#include "../../../Framework/Common/Includes.h"
#include <vector>

namespace Framework {
namespace Tool {

class Archiver
{
public:
    Archiver(const u32 count = 0);
    ~Archiver();

    void Clear();
    void SetWorkBuffer(void* buffer, const u32 size);
    bool Add(const wchar_t* name, const void* data, const u32 size);
    const u32 GetData(const u32 index, void* data);
    const u32 GetDataSize(const u32 index);
    const wchar_t* GetDataName(const u32 index);
    const u32 GetDataCount();

    void WriteBinary(void* buffer);
    bool ReadBinary(const void* buffer, const size_t size);
    const u32 GetBinarySize() const ;

private:
    struct FixedArchiveHeader
    {
        s8 token[4];      // アーカイブを特定する { 'F','W','A','C' }
        u16 version;      // アーカイブバージョン
        u32 count;        // データ数
        s8 reserved[50];  // 予約領域
    };
    static_assert(sizeof(FixedArchiveHeader) == 64, "sizeof ArchiveHeader == 64");

    struct ArchiveData
    {
        u32 size;       // データサイズ
        u32 offset;     // 実データまでのオフセット
        s8 reserved[8]; // 予約領域
        wchar_t* name;  // ファイル識別名
        void* data;     // 実データ部分

        static const u32 FixedParamSize = 16; // 構造体のうち、固定長メンバのサイズ
    };
    static_assert(sizeof(ArchiveData) == 32, "sizeof DataHeader == 32");

private:
    const u32 CalcBinarySize();

private:
    std::vector<ArchiveData*> mData;
    u32 mDataSize;
};

} // namespace Tool 
} // namespace Framework

#endif // ARCHIVER_H_