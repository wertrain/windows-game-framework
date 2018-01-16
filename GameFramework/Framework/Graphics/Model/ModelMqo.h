#ifndef GRAPHICS_MODEL_MODELMQO_H_
#define GRAPHICS_MODEL_MODELMQO_H_

#include "ModelBase.h"
#include <string>
#include <vector>

NS_FW_GFX_BEGIN

class MqoFile
{
public:
    struct Scene
    {
        f32 pos[4];
        f32 lookat[4];
        f32 head;
        f32 pich;
        f32 ortho;
        f32 zoom2;
        f32 amb[4];
    };

    struct Material
    {
        std::string name;
        s32 shader;
        f32 col[4];
        f32 dif;
        f32 amb;
        f32 emi;
        f32 spc;
        f32 power;
        std::string tex;
    };

    struct Object
    {
        struct Vertex
        {
            f32 pos[4];
        };
        struct Face
        {
            s32 num;
            s32* V;
            s32  M;
            f32* UV;
        };

        std::string name;
        s32 depth;
        s32 folding;
        f32 scale[4];
        f32 rotation[4];
        f32 translation[4];
        s32 visible;
        s32 locking;
        s32 shading;
        f32 facet;
        f32 color[4];
        s32 color_type;
        s32 vertex_num;
        Vertex *vertices;
        s32 face_num;
        Face *faces;
    };
public:
    MqoFile();
    ~MqoFile();

    bool Read(const wchar_t* file);
    void Destroy();

private:
    bool ParseScene(FILE* fp, char* buffer, const int bufferSize);
    bool ParseMaterial(FILE* fp, char* buffer, const int bufferSize);
    bool ParseObject(FILE* fp, char* buffer, const int bufferSize);
    bool ParseObjectVertex(Object* p, FILE* fp, char* buffer, const int bufferSize);
    bool ParseObjectFace(Object* p, FILE* fp, char* buffer, const int bufferSize);

private:
    Scene mScene;
    std::vector<Material*> mMaterials;
    std::vector<Object*> mObjects;
};

class ModelMqo : public ModelBase
{
public:
    ModelMqo() ;
    ~ModelMqo();
};

NS_FW_GFX_END

#endif // GRAPHICS_MODEL_MODELMQO_H_
