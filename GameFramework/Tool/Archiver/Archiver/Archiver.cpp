#include "../../../Framework/Common/Defines.h"

#include "Archiver.h"

namespace Framework {
namespace Tool {

enum ARCHIVE_FILE_HEADER_VERSION
{
    V_0000
};

Archiver::Archiver(const u32 count)
    : mData()
    , mDataSize(sizeof(FixedArchiveHeader))
{
    if (count > 0)
    {
        mData.reserve(count);
    }
}

Archiver::~Archiver()
{

}

void Archiver::SetWorkBuffer(void* /*buffer*/, const u32 /*size*/)
{
    // TODO:
    // 外部からバッファを指定してそこを使うように
}

bool Archiver::Add(const void* data, const u32 size)
{
    ArchiveData* d = new ArchiveData();
    d->size = size;
    d->data = data;
    mData.push_back(d);
    
    mDataSize += (sizeof(ArchiveData) - sizeof(ArchiveData::data));
    mDataSize += size;

    return true;
}

void Archiver::WriteBinary(void* buffer)
{
    FixedArchiveHeader header;
    memset(&header, 0, sizeof(FixedArchiveHeader));
    header.token[0] = 'F';
    header.token[1] = 'W';
    header.token[2] = 'A';
    header.token[3] = 'C';
    header.count = mData.size();
    header.version = ARCHIVE_FILE_HEADER_VERSION::V_0000;

    s8* p = static_cast<s8*>(buffer);
    memcpy(p, &header, sizeof(FixedArchiveHeader));
    p += sizeof(FixedArchiveHeader);

    for each (const ArchiveData *data in mData)
    {
        memcpy(p, data, sizeof(ArchiveData));
        p += (sizeof(ArchiveData) - sizeof(ArchiveData::data));
        memcpy(p, data->data, data->size);
    }
}

bool Archiver::ReadBinary(const void* buffer, const u32 size)
{
    FixedArchiveHeader header;
    memcpy(&header, buffer, sizeof(FixedArchiveHeader));

    if (header.token[0] != 'F' || header.token[1] != 'W' || header.token[2] != 'A' || header.token[3] == 'C')
        return false;

    // バージョン依存処理
    switch (header.version)
    {
    case ARCHIVE_FILE_HEADER_VERSION::V_0000:
        break;
    default:
        return false;
    }

    mData.clear();
    mData.reserve(header.count);

    const s8* p = static_cast<const s8*>(buffer);
    p += sizeof(FixedArchiveHeader);
    for (s32 i = 0; i < header.count; ++i)
    {
        ArchiveData* d = new ArchiveData();
        memcpy(d, p, sizeof(ArchiveData));
        p += (sizeof(ArchiveData) - sizeof(ArchiveData::data));
        d->data = p;
        mData.push_back(d);
    }
    return true;
}

const u32 Archiver::GetBinarySize() const
{
    return mDataSize;
}

const u32 Archiver::CalcBinarySize()
{
    u32 size = sizeof(FixedArchiveHeader);
    for each (const ArchiveData *data in mData)
    {
        size = size + (sizeof(ArchiveData) - sizeof(ArchiveData::data));
        size = size + data->size;
    }
    return size;
}

} // namespace Tool 
} // namespace Framework