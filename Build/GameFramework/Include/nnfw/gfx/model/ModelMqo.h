/**
 * @file ModelMqo.h
 * @brief メタセコイアモデルを扱うクラス
 */
#ifndef GRAPHICS_MODEL_MODELMQO_H_
#define GRAPHICS_MODEL_MODELMQO_H_

#include <nnfw/gfx/model/ModelBase.h>
#include <string>
#include <vector>

NS_FW_GFX_BEGIN

/// <summary>
/// メタセコイアモデルを扱うクラス
/// </summary>
class MqoFile
{
public:
    struct Scene
    {
        float pos[4];
        float lookat[4];
        float head;
        float pich;
        float ortho;
        float zoom2;
        float amb[4];
    };

    struct Material
    {
        std::string name;
        int32_t shader;
        float col[4];
        float dif;
        float amb;
        float emi;
        float spc;
        float power;
        std::string tex;
    };

    struct Object
    {
        struct Vertex
        {
            float pos[4];
        };
        struct Face
        {
            int32_t num;
            int32_t* V;
            int32_t  M;
            float* UV;
        };

        std::string name;
        int32_t depth;
        int32_t folding;
        float scale[4];
        float rotation[4];
        float translation[4];
        int32_t visible;
        int32_t locking;
        int32_t shading;
        float facet;
        float color[4];
        int32_t color_type;
        int32_t vertex_num;
        Vertex *vertices;
        int32_t face_num;
        Face *faces;
    };
public:
    MqoFile();
    ~MqoFile();

    bool Read(const wchar_t* file);
    void Destroy();
    const Scene* GetScene();
    const std::vector<Material*>* GetMaterials();
    const std::vector<Object*>* GetObjects();

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

    bool Create(ID3D11Device* device, ID3D11DeviceContext* context, const wchar_t* filename);
    void Destroy();
    void Render(ID3D11DeviceContext* context);

private:
    // 頂点データ構造体
    struct VertexData
    {
        float pos[3]; // Vector3 のほうが高速なはず
        float uv[2];
    };
    static_assert(sizeof(VertexData) == (4 * 3 + 4 * 2), "sizeof VertexData == (4 * 3 + 4 * 2)");

    // 1オブジェクト描画分
    struct MeshData
    {
        VertexData* vertices;
        uint32_t* indices;
        ID3D11Buffer* vertexBuffer;
        ID3D11Buffer* indexBuffer;
        uint32_t vertex_num;
        uint32_t index_num;
        int32_t material_id;
        int32_t visible;

        MeshData()
            : vertices(nullptr)
            , indices(nullptr)
            , vertexBuffer(nullptr)
            , indexBuffer(nullptr)
            , vertex_num(0)
            , index_num(0)
            , material_id(0)
            , visible(0)
        {

        }
    };

    struct MaterialData
    {
        ID3D11Resource* texture;
        ID3D11ShaderResourceView* shaderResView;
        ID3D11SamplerState* sampler;

        MaterialData()
            : texture(nullptr)
            , shaderResView(nullptr)
            , sampler(nullptr)
        {

        }
    };

private:
    MqoFile mFile;

    std::vector<MeshData*> mMeshData;
    std::vector<MaterialData*> mMaterials;
    ID3D11Buffer* mConstBuffer;
    ID3D11InputLayout* mVertexLayout;
    ID3D11RasterizerState* mRsState;
    ID3D11DepthStencilState* mDsState;
    ID3D11BlendState* mBdState;
    ID3D11VertexShader* mVertexShader;
    ID3D11PixelShader* mPixelShader;
};

NS_FW_GFX_END

#endif // GRAPHICS_MODEL_MODELMQO_H_
