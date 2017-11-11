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
        s8 token[4];      // �A�[�J�C�u����肷�� { 'F','W','A','C' }
        u16 version;      // �A�[�J�C�u�o�[�W����
        u32 count;        // �f�[�^��
        s8 reserved[50];  // �\��̈�
    };
    static_assert(sizeof(FixedArchiveHeader) == 64, "sizeof ArchiveHeader == 64");

    struct DataHeader
    {
        u32 size;          // �f�[�^�T�C�Y
        s8 reserved[4];    // �\��̈�
        void* data;        // ���f�[�^����
    };
    static_assert(sizeof(DataHeader) == 16, "sizeof DataHeader == 16");
};

} // namespace Tool 
} // namespace Framework

#endif // ARCHIVER_H_