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
        s8 token[4];      // �A�[�J�C�u����肷�� { 'F','W','A','C' }
        u16 version;      // �A�[�J�C�u�o�[�W����
        u32 count;        // �f�[�^��
        s8 reserved[50];  // �\��̈�
    };
    static_assert(sizeof(FixedArchiveHeader) == 64, "sizeof ArchiveHeader == 64");

    struct ArchiveData
    {
        u32 size;       // �f�[�^�T�C�Y
        u32 offset;     // ���f�[�^�܂ł̃I�t�Z�b�g
        s8 reserved[8]; // �\��̈�
        wchar_t* name;  // �t�@�C�����ʖ�
        void* data;     // ���f�[�^����

        static const u32 FixedParamSize = 16; // �\���̂̂����A�Œ蒷�����o�̃T�C�Y
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