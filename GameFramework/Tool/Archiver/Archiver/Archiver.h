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

    void SetWorkBuffer(void* buffer, const u32 size);
    bool Add(const void* data, const u32 size);
    void WriteBinary(void* buffer);
    bool ReadBinary(const void* buffer, const u32 size);
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
        u32 size;          // データサイズ
        s8 reserved[4];    // 予約領域
        const void* data;  // 実データ部分
    };
    static_assert(sizeof(ArchiveData) == 16, "sizeof DataHeader == 16");

private:
    const u32 CalcBinarySize();

private:
    std::vector<ArchiveData*> mData;
    u32 mDataSize;
};

} // namespace Tool 
} // namespace Framework

#endif // ARCHIVER_H_