#ifndef FILE_H_
#define FILE_H_

namespace Framework {
namespace System {

class File
{
public:
    static void* Read(const char* filename, size_t& readsize);
    static size_t GetSize(const char* filename);
};

}
}

#endif // FILE_H_
