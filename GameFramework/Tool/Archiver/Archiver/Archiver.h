#ifndef ARCHIVER_H_
#define ARCHIVER_H_

#include "../../../Framework/Common/Includes.h"

namespace Framework {
namespace Tool {

class Archiver
{
public:
    Archiver();
    ~Archiver();

private:
    struct FixedArchiveHeader
    {
        s8 token[4];      // アーカイブを特定する { 'F','W','A','C' }
        u16 version;      // アーカイブバージョン
        u32 count;        // データ数
        s8 reserved[50];  // 予約領域
    };
    static_assert(sizeof(FixedArchiveHeader) == 64, "sizeof ArchiveHeader == 64");

    struct DataHeader
    {
        u32 size;          // データサイズ
        s8 reserved[4];    // 予約領域
        void* data;        // 実データ部分
    };
    static_assert(sizeof(DataHeader) == 16, "sizeof DataHeader == 16");
};

} // namespace Tool 
} // namespace Framework

#endif // ARCHIVER_H_