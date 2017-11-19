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
    Clear();
}

void Archiver::Clear()
{
    for each (const ArchiveData *data in mData)
    {
        delete [] data->name;
        delete [] data->data;
        delete data;
    }
    mData.clear();
}

void Archiver::SetWorkBuffer(void* /*buffer*/, const u32 /*size*/)
{
    // TODO:
    // 外部からバッファを指定してそこを使うように
}

bool Archiver::Add(const wchar_t* name, const void* data, const u32 size)
{
    ArchiveData* d = new ArchiveData();
    memset(d, 0, ArchiveData::FixedParamSize);
    d->size = size;
    d->data = new s8[size];
    memcpy(d->data, data, size);
    u32 namesize = static_cast<u32>(wcslen(name) + 1) * sizeof(wchar_t); // null 文字込み
    d->name = new wchar_t[namesize];
    memset(d->name, 0, namesize);
    wcscpy_s(d->name, namesize, name);
    d->offset = ArchiveData::FixedParamSize + namesize;
    mData.push_back(d);
    
    mDataSize += ArchiveData::FixedParamSize + namesize;
    mDataSize += size;

    return true;
}

const u32 Archiver::GetData(const u32 index, void* data)
{
    if (mData.size() <= index) return 0;

    ArchiveData* d = mData[index];
    memcpy(data, d->data, d->size);
    return d->size;
}

const u32 Archiver::GetDataSize(const u32 index)
{
    if (mData.size() <= index) return 0;

    ArchiveData* d = mData[index];
    return d->size;
}

const wchar_t* Archiver::GetDataName(const u32 index)
{
    if (mData.size() <= index) return 0;

    ArchiveData* d = mData[index];
    return d->name;
}

const u32 Archiver::GetDataCount()
{
    return static_cast<u32>(mData.size());
}

void Archiver::WriteBinary(void* buffer)
{
    FixedArchiveHeader header;
    memset(&header, 0, sizeof(FixedArchiveHeader));
    header.token[0] = 'F';
    header.token[1] = 'W';
    header.token[2] = 'A';
    header.token[3] = 'C';
    header.count = static_cast<u32>(mData.size());
    header.version = ARCHIVE_FILE_HEADER_VERSION::V_0000;

    s8* p = static_cast<s8*>(buffer);
    memcpy(p, &header, sizeof(FixedArchiveHeader));
    p += sizeof(FixedArchiveHeader);

    for each (const ArchiveData *data in mData)
    {
        memcpy(p, data, ArchiveData::FixedParamSize);
        p += ArchiveData::FixedParamSize;
        size_t namesize = (wcslen(data->name) + 1) * sizeof(wchar_t); // null 文字込み
        memcpy(p, data->name, namesize);
        p += namesize;
        memcpy(p, data->data, data->size);
        p += data->size;
    }
}

bool Archiver::ReadBinary(const void* buffer, const u32 size)
{
    FixedArchiveHeader header;
    memcpy(&header, buffer, sizeof(FixedArchiveHeader));

    if (header.token[0] != 'F' || header.token[1] != 'W' || header.token[2] != 'A' || header.token[3] != 'C')
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
    for (u32 i = 0; i < header.count; ++i)
    {
        ArchiveData* d = new ArchiveData();
        memcpy(d, p, ArchiveData::FixedParamSize);
        p += ArchiveData::FixedParamSize;
        u32 namesize = d->offset - ArchiveData::FixedParamSize;
        d->name = new wchar_t[namesize];
        memcpy(d->name, p, namesize);
        p += namesize;
        d->data = new s8[d->size];
        memcpy(d->data, p, d->size);
        p += d->size;
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