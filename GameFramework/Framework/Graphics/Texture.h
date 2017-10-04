#ifndef TEXTURE_H_
#define TEXTURE_H_

namespace Framework {
namespace Graphics {

class Texture
{
public:
    Texture() ;
    virtual ~Texture();

    bool CreateFromFile(const wchar_t* filename);
    void Draw(const int x, const int y);
    void Destroy();
};

} // namespace Graphics 
} // namespace Framework

#endif // TEXTURE_H_
