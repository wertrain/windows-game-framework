/**
 * @file ModelMqo.cpp
 * @brief メタセコイアモデルを扱うクラス
 */
#include "Precompiled.h"

#include <d3d11.h>
#include <DirectXMath.h>

#include <nnfw/common/Includes.h>
#include <nnfw/sys/Includes.h>
#include <nnfw/Constants.h>
#include <nnfw/external/DirectXTex/WICTextureLoader/WICTextureLoader.h>

#include <nnfw/gfx/model/ModelMqo.h>
#include <codecvt> 
#include <regex>

NS_FW_GFX_BEGIN

// シェーダ定数バッファ
struct ConstBuffer
{
    Matrix44 mtxProj;
    Matrix44 mtxView;
    Matrix44 mtxWorld;
    Vector4 Diffuse;
};

MqoFile::MqoFile()
    : mScene()
    , mMaterials()
    , mObjects()
{

}

MqoFile::~MqoFile()
{

}

bool MqoFile::Read(const wchar_t* file)
{
    FILE *fp = NULL;
    _wfopen_s(&fp, file, L"r");
    if (fp == NULL) return false;

    const int bufferSize = 1024 * 1;
    char buffer[bufferSize];

    while (std::fgets(buffer, bufferSize - 1, fp) != NULL)
    {
        //const size_t len = strnlen(buffer, bufferSize);

        if (strncmp("Scene ", buffer, 6) == 0)
        {
            ParseScene(fp, buffer, bufferSize);
        }
        else if (strncmp("Material ", buffer, 9) == 0)
        {
            ParseMaterial(fp, buffer, bufferSize);
        }
        else if (strncmp("Object ", buffer, 6) == 0)
        {
            ParseObject(fp, buffer, bufferSize);
        }
    }
    fclose(fp);
    return true;
}

void MqoFile::Destroy()
{
    memset(&mScene, 0, sizeof(Scene));
    for each(auto m in mMaterials)
    {
        delete m;
    }
    mMaterials.clear();
    for each(auto o in mObjects)
    {
        if (o->vertices) delete[] o->vertices;
        for (int i = 0; i < o->face_num; ++i)
        {
            if (o->faces[i].V) delete[] o->faces[i].V;
            if (o->faces[i].UV) delete[] o->faces[i].UV;
        }
        if (o->faces) delete o->faces;
        delete o;
    }
    mObjects.clear();
}

const MqoFile::Scene* MqoFile::GetScene()
{
    return &mScene;
}

const std::vector<MqoFile::Material*>* MqoFile::GetMaterials()
{
    return &mMaterials;
}

const std::vector<MqoFile::Object*>* MqoFile::GetObjects()
{
    return &mObjects;
}

/// パラメータ取得マクロ
/// よくない実装のお手本だよ

#define PARAM_GET_START                                                          \
    char tmpbuf[2048];                                                           \
    strcpy_s(tmpbuf, 2047, buffer);                                              \
    char* ctx;                                                                   \
    char* tp = strtok_s(tmpbuf, " ", &ctx)

#define PARAM_GET(var, elem)                                                     \
        (strstr(tp, #elem) != NULL)                                              \
        {                                                                        \
            while (tp != NULL)                                                   \
            {                                                                    \
                tp = strtok_s(NULL, " ", &ctx);                                  \
                if (tp != NULL) ##var.##elem = tp;                               \
            }                                                                    \
        }

#define PARAM_GET_INT(var, elem)                                                    \
        (strstr(tp, #elem) != NULL)                                                 \
        {                                                                           \
            while (tp != NULL)                                                      \
            {                                                                       \
                tp = strtok_s(NULL, " ", &ctx);                                     \
                if (tp != NULL) ##var.##elem = atoi(tp);                            \
            }                                                                       \
        }

#define PARAM_GET_INT_ARRAY(var, elem)                                              \
        (strstr(tp, #elem) != NULL)                                                 \
        {                                                                           \
            int index = 0;                                                          \
            while (tp != NULL)                                                      \
            {                                                                       \
                tp = strtok_s(NULL, " ", &ctx);                                     \
                if (tp != NULL) ##var.##elem[index++] = atoi(tp);                   \
            }                                                                       \
        }

#define PARAM_GET_FLOAT(var, elem)                                                  \
        (strstr(tp, #elem) != NULL)                                                 \
        {                                                                           \
            while (tp != NULL)                                                      \
            {                                                                       \
                tp = strtok_s(NULL, " ", &ctx);                                     \
                if (tp != NULL) ##var.##elem = static_cast<float>(atof(tp));          \
            }                                                                       \
        }

#define NOPARAM_GET_FLOAT_ARRAY(var, elem)                                          \
        {                                                                           \
            int index = 0;                                                          \
            do                                                                      \
            {                                                                       \
                if (tp != NULL) ##var.##elem[index++] = static_cast<float>(atof(tp)); \
                tp = strtok_s(NULL, " ", &ctx);                                     \
            }                                                                       \
            while (tp != NULL);                                                     \
        }

#define PARAM_GET_FLOAT_ARRAY(var, elem)                                            \
        (strstr(tp, #elem) != NULL)                                                 \
        {                                                                           \
            int index = 0;                                                          \
            while (tp != NULL)                                                      \
            {                                                                       \
                tp = strtok_s(NULL, " ", &ctx);                                     \
                if (tp != NULL) ##var.##elem[index++] = static_cast<float>(atof(tp)); \
            }                                                                       \
        }

#define REGEX_GET_START std::string src = buffer

#define REGEX_GET(r, member)                               \
        do                                                 \
        {                                                  \
            std::regex re(r);                              \
            std::smatch results;                           \
            if (std::regex_search(src, results, re))       \
            {                                              \
                p->##member = results[1].str();            \
                src = results.suffix();                    \
            }                                              \
        }                                                  \
        while(0)

#define REGEX_GET_INT(r, member)                              \
        do                                                    \
        {                                                     \
            std::regex re(r);                                 \
            std::smatch results;                              \
            if (std::regex_search(src, results, re))          \
            {                                                 \
                p->##member = atoi(results[1].str().c_str()); \
                src = results.suffix();                       \
            }                                                 \
        }                                                     \
        while(0)

#define REGEX_GET_INT_ARRAY(r, member)                                   \
        do                                                               \
        {                                                                \
            std::regex re(r);                                            \
            std::smatch results;                                         \
            if (std::regex_search(src, results, re))                     \
            {                                                            \
                for (int i = 0; i < results.size() - 1; ++i)             \
                {                                                        \
                    p->##member[i] = atoi(results[i + 1].str().c_str()); \
                }                                                        \
                src = results.suffix();                                  \
            }                                                            \
        }                                                                \
        while(0)

#define REGEX_GET_FLOAT(r, member)                                               \
        do                                                                       \
        {                                                                        \
            std::regex re(r);                                                    \
            std::smatch results;                                                 \
            if (std::regex_search(src, results, re))                             \
            {                                                                    \
                p->##member = static_cast<float>(atof(results[1].str().c_str()));  \
                src = results.suffix();                                          \
            }                                                                    \
        }                                                                        \
        while(0)

#define REGEX_GET_FLOAT_ARRAY(r, member)                                                   \
        do                                                                                 \
        {                                                                                  \
            std::regex re(r);                                                              \
            std::smatch results;                                                           \
            if (std::regex_search(src, results, re))                                       \
            {                                                                              \
                for (int i = 0; i < results.size() - 1; ++i)                               \
                {                                                                          \
                    p->##member[i] = static_cast<float>(atof(results[i + 1].str().c_str())); \
                }                                                                          \
                src = results.suffix();                                                    \
            }                                                                              \
        }                                                                                  \
        while(0)

bool MqoFile::ParseScene(FILE* fp, char* buffer, const int bufferSize)
{
    do
    {
        if (strncmp("}", buffer, 1) == 0)
        {
            return true;
        }
        else
        {
            PARAM_GET_START;
            if PARAM_GET_FLOAT_ARRAY(mScene, pos)
            else if PARAM_GET_FLOAT_ARRAY(mScene, lookat)
            else if PARAM_GET_FLOAT(mScene, head)
            else if PARAM_GET_FLOAT(mScene, pich)
            else if PARAM_GET_FLOAT(mScene, ortho)
            else if PARAM_GET_FLOAT(mScene, zoom2)
            else if PARAM_GET_FLOAT_ARRAY(mScene, amb)
        }
    }
    while (std::fgets(buffer, bufferSize - 1, fp) != NULL);

    return true;
}

bool MqoFile::ParseMaterial(FILE* fp, char* buffer, const int bufferSize)
{
    // 次の行から開始
    while (std::fgets(buffer, bufferSize - 1, fp) != NULL)
    {
        if (strncmp("}", buffer, 1) == 0)
        {
            return true;
        }
        else
        {
            Material* p = new Material();
            mMaterials.push_back(p);

            REGEX_GET_START;
            REGEX_GET("\"(\\w+)\"", name);
            REGEX_GET_INT("shader\\((\\d+)\\)", shader);
            REGEX_GET_FLOAT_ARRAY("col\\((\\d+\\.\\d+) (\\d+\\.\\d+) (\\d+\\.\\d+) (\\d+\\.\\d+)\\)", col);
            REGEX_GET_FLOAT("dif\\((\\d+\\.\\d+)\\)", dif);
            REGEX_GET_FLOAT("amb\\((\\d+\\.\\d+)\\)", amb);
            REGEX_GET_FLOAT("emi\\((\\d+\\.\\d+)\\)", emi);
            REGEX_GET_FLOAT("spc\\((\\d+\\.\\d+)\\)", spc);
            REGEX_GET_FLOAT("power\\((\\d+\\.\\d+)\\)", power);
            REGEX_GET("tex\\(\"(.+)\"\\)", tex);
        }
    }

    return true;
}

bool MqoFile::ParseObject(FILE* fp, char* buffer, const int bufferSize)
{
    Object* p = new Object();
    mObjects.push_back(p);

    REGEX_GET_START;
    REGEX_GET("Object \"(.+)\"", name);

    // 次の行から開始
    while (std::fgets(buffer, bufferSize - 1, fp) != NULL)
    {
        if (strncmp("}", buffer, 1) == 0)
        {
            return true;
        }
        else
        {
            PARAM_GET_START;
            if PARAM_GET_INT((*p), depth)
            else if PARAM_GET_INT((*p), folding)
            else if PARAM_GET_FLOAT_ARRAY((*p), scale)
            else if PARAM_GET_FLOAT_ARRAY((*p), rotation)
            else if PARAM_GET_FLOAT_ARRAY((*p), translation)
            else if PARAM_GET_INT((*p), visible)
            else if PARAM_GET_INT((*p), locking)
            else if PARAM_GET_INT((*p), shading)
            else if PARAM_GET_FLOAT((*p), facet)
            else if PARAM_GET_FLOAT_ARRAY((*p), color)
            else if PARAM_GET_INT((*p), color_type)
            else if (strstr(buffer, "vertex ") != NULL)
            {
                ParseObjectVertex(p, fp, buffer, bufferSize);
            }
            else if (strstr(buffer, "face ") != NULL)
            {
                ParseObjectFace(p, fp, buffer, bufferSize);
            }
        }
    }

    return true;
}

bool MqoFile::ParseObjectVertex(Object* p, FILE* fp, char* buffer, const int bufferSize)
{
    REGEX_GET_START;
    REGEX_GET_INT("vertex (\\d+)", vertex_num);
    _ASSERT(p->vertex_num);

    p->vertices = new Object::Vertex[p->vertex_num];
    memset(p->vertices, 0, sizeof(Object::Vertex) * p->vertex_num);

    int vertex_index = 0;
    while (std::fgets(buffer, bufferSize - 1, fp) != NULL)
    {
        if (strstr(buffer, "}") != 0)
        {
            return true;
        }
        else
        {
            PARAM_GET_START;
            NOPARAM_GET_FLOAT_ARRAY(p->vertices[vertex_index], pos)
            ++vertex_index;
        }
    }

    return true;
}

bool MqoFile::ParseObjectFace(Object* p, FILE* fp, char* buffer, const int bufferSize)
{
    {
        REGEX_GET_START;
        REGEX_GET_INT("face (\\d+)", face_num);
        _ASSERT(p->face_num);
    }

    p->faces = new Object::Face[p->face_num];
    memset(p->faces, 0, sizeof(Object::Face) * p->face_num);

    int face_index = 0;
    while (std::fgets(buffer, bufferSize - 1, fp) != NULL)
    {
        if (strstr(buffer, "}") != 0)
        {
            return true;
        }
        else
        {
            REGEX_GET_START;
            REGEX_GET_INT("(\\d+)", faces[face_index].num);
            _ASSERTE(p->faces[face_index].num > 0);
            if (p->faces[face_index].num == 0) return false;

            const size_t v_num = p->faces[face_index].num;
            p->faces[face_index].V = new int32_t[v_num];
            memset(p->faces[face_index].V, 0, sizeof(int32_t) * v_num);
            p->faces[face_index].UV = new float[v_num * 2];
            memset(p->faces[face_index].UV, 0, sizeof(float) * v_num * 2);

            {
                char regex_v[256] = { 0 };
                strcat_s(regex_v, 256, "V\\(");
                for (int i = 0; i < v_num; ++i)
                {
                    strcat_s(regex_v, 256, "(\\d+)");
                    if (i < v_num - 1) strcat_s(regex_v, 256, " ");
                }
                strcat_s(regex_v, 256, "\\)");
                REGEX_GET_INT_ARRAY(regex_v, faces[face_index].V);
            }

            REGEX_GET_INT("M\\((\\d+)\\)", faces[face_index].M);

            {
                char regex_uv[256] = { 0 };
                strcat_s(regex_uv, 256, "UV\\(");
                for (int i = 0; i < v_num * 2; ++i)
                {
                    strcat_s(regex_uv, 256, "(\\d+\\.\\d+)");
                    if (i < v_num * 2 - 1) strcat_s(regex_uv, 256, " ");
                }
                strcat_s(regex_uv, 256, "\\)");
                REGEX_GET_FLOAT_ARRAY(regex_uv, faces[face_index].UV);
            }
            ++face_index;
        }
    }

    return true;
}

#undef REGEX_GET_START
#undef REGEX_GET
#undef REGEX_GET_INT
#undef REGEX_GET_FLOAT
#undef REGEX_GET_FLOAT_ARRAY

#undef PARAM_GET_START
#undef PARAM_GET
#undef PARAM_GET_INT
#undef PARAM_GET_INT_ARRAY
#undef PARAM_GET_FLOAT
#undef PARAM_GET_FLOAT_ARRAY

ModelMqo::ModelMqo()
    : mFile()
    , mMeshData()
    , mMaterials()
    , mConstBuffer(nullptr)
    , mVertexLayout(nullptr)
    , mRsState(nullptr)
    , mDsState(nullptr)
    , mBdState(nullptr)
    , mVertexShader(nullptr)
    , mPixelShader(nullptr)
{

}

ModelMqo::~ModelMqo()
{

}

bool ModelMqo::Create(ID3D11Device* device, ID3D11DeviceContext* context, const wchar_t* filename)
{
    Destroy();
    
    if (!mFile.Read(filename))
    {
        return false;
    }

    NS_FW_SYS::Binary vsFile;
    NS_FW_SYS::Binary psFile;

    // コンパイル済みバーテックスシェーダーファイルの読み込み
    if (!vsFile.Read(L"vs_model_inner.cso"))
    {
        return false;
    }
    // コンパイル済みピクセルシェーダーファイルの読み込み
    if (!psFile.Read(L"ps_model_inner.cso"))
    {
        return false;
    }

    // 頂点シェーダー生成
    if (FAILED(device->CreateVertexShader(vsFile.Get(), vsFile.Size(), NULL, &mVertexShader)))
    {
        return false;
    }

    // ピクセルシェーダー生成
    if (FAILED(device->CreatePixelShader(psFile.Get(), psFile.Size(), NULL, &mPixelShader)))
    {
        return false;
    }

    // 入力レイアウト定義
    D3D11_INPUT_ELEMENT_DESC layout[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    const UINT elem_num = ARRAYSIZE(layout);

    // 入力レイアウト作成
    HRESULT hr = device->CreateInputLayout(layout, elem_num, vsFile.Get(), vsFile.Size(), &mVertexLayout);
    if (FAILED(hr)) {
        return false;
    }

    // 頂点座標の展開
    auto objects = mFile.GetObjects();
    _ASSERT(objects->size() > 0);
    for (int obj_idx = 0; obj_idx < objects->size(); ++obj_idx)
    {
        auto obj = objects->at(obj_idx);
        _ASSERT(obj->face_num > 0);

        // 1メッシュ分のデータ
        MeshData *mesh = new MeshData();
        mesh->vertices = new VertexData[obj->face_num * 4]; // 多めに確保
        mesh->indices = new uint32_t[obj->face_num * 4];    // 同上
        mesh->material_id = obj->faces[0].M;
        mesh->visible = obj->visible;

        int idx_idx = 0;
        for (int face_idx = 0; face_idx < obj->face_num; ++face_idx)
        {
            // この範囲は同じマテリアルである想定
            //_ASSERT(mesh->material_id == obj->faces[face_idx].M);

            auto face = obj->faces[face_idx];
            _ASSERT(face.num > 0);
            for (int v_idx = 0; v_idx < face.num; ++v_idx)
            {
                // 頂点を展開して入れているのに
                // インデックスがおかしいのであとで直す
                auto index = face.V[v_idx];
                mesh->vertices[idx_idx].pos[0] = obj->vertices[index].pos[0];
                mesh->vertices[idx_idx].pos[1] = obj->vertices[index].pos[1];
                mesh->vertices[idx_idx].pos[2] = obj->vertices[index].pos[2];

                const int uv_idx = v_idx * 2;
                mesh->vertices[idx_idx].uv[0] = face.UV[uv_idx];
                mesh->vertices[idx_idx].uv[1] = face.UV[uv_idx + 1];

                mesh->indices[idx_idx] = index;
                idx_idx++;
            }
        }
        mesh->vertex_num = idx_idx;
        mesh->index_num = idx_idx;

        // バーテックスバッファ
        {
            D3D11_BUFFER_DESC bd;
            ZeroMemory(&bd, sizeof(bd));
            bd.Usage = D3D11_USAGE_DEFAULT;
            bd.ByteWidth = sizeof(VertexData) * mesh->vertex_num;
            bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
            bd.CPUAccessFlags = 0;
            D3D11_SUBRESOURCE_DATA InitData;
            ZeroMemory(&InitData, sizeof(InitData));
            InitData.pSysMem = mesh->vertices;
            hr = device->CreateBuffer(&bd, &InitData, &mesh->vertexBuffer);
            if (FAILED(hr)) {
                return false;
            }
        }

        // インデックスバッファ
        {
            D3D11_BUFFER_DESC bd;
            ZeroMemory(&bd, sizeof(bd));
            bd.Usage = D3D11_USAGE_DEFAULT;
            bd.ByteWidth = sizeof(uint32_t) * mesh->vertex_num;
            bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
            bd.CPUAccessFlags = 0;
            D3D11_SUBRESOURCE_DATA InitData;
            ZeroMemory(&InitData, sizeof(InitData));
            InitData.pSysMem = mesh->indices;
            hr = device->CreateBuffer(&bd, &InitData, &mesh->indexBuffer);
            if (FAILED(hr)) {
                return false;
            }
        }

        mMeshData.push_back(mesh);
    }

    // マテリアル
    auto materials = mFile.GetMaterials();
    _ASSERT(materials->size() > 0);
    for (int mat_idx = 0; mat_idx < materials->size(); ++mat_idx)
    {
        auto mat = materials->at(mat_idx);
        
        MaterialData* data = new MaterialData();
        if (!mat->tex.empty())
        {
            // テクスチャ作成
            std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> cv;
            std::wstring wpath = cv.from_bytes(mat->tex);
            hr = DirectX::CreateWICTextureFromFile(device, context, wpath.c_str(), &data->texture, &data->shaderResView);
            if (FAILED(hr)) {
                return false;
            }

            // サンプラー作成
            D3D11_SAMPLER_DESC sampDesc;
            ZeroMemory(&sampDesc, sizeof(sampDesc));
            sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
            sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
            sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
            sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
            sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
            sampDesc.MinLOD = 0;
            sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
            hr = device->CreateSamplerState(&sampDesc, &data->sampler);
            if (FAILED(hr)) {
                return false;
            }
        }
        mMaterials.push_back(data);
    }

    // 定数バッファ
    {
        D3D11_BUFFER_DESC bd;
        ZeroMemory(&bd, sizeof(bd));
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.ByteWidth = sizeof(ConstBuffer);
        bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        bd.CPUAccessFlags = 0;
        hr = device->CreateBuffer(&bd, nullptr, &mConstBuffer);
        if (FAILED(hr)) {
            return false;
        }
    }

    CD3D11_DEFAULT default_state;
    // ラスタライザステート
    CD3D11_RASTERIZER_DESC rsdesc(default_state);
    rsdesc.CullMode = D3D11_CULL_NONE;
    hr = device->CreateRasterizerState(&rsdesc, &mRsState);
    if (FAILED(hr)) {
        return false;
    }

    // デプスステンシルステート
    CD3D11_DEPTH_STENCIL_DESC dsdesc(default_state);
    dsdesc.DepthEnable = true;
    dsdesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    dsdesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
    hr = device->CreateDepthStencilState(&dsdesc, &mDsState);
    if (FAILED(hr)) {
        return false;
    }

    // ブレンドステート
    CD3D11_BLEND_DESC bddesc(default_state);
    //ZeroMemory(&bddesc, sizeof(D3D11_BLEND_DESC));
    //bddesc.AlphaToCoverageEnable = TRUE;
    //bddesc.IndependentBlendEnable = TRUE;
    //bddesc.RenderTarget[0].BlendEnable = TRUE;
    hr = device->CreateBlendState(&bddesc, &mBdState);
    if (FAILED(hr)) {
        return false;
    }

    return true;
}

void ModelMqo::Destroy()
{
    if (mPixelShader)
    {
        mPixelShader->Release();
        mPixelShader = nullptr;
    }
    if (mVertexShader)
    {
        mVertexShader->Release();
        mVertexShader = nullptr;
    }
    if (mRsState)
    {
        mRsState->Release();
        mRsState = nullptr;
    }
    if (mDsState)
    {
        mDsState->Release();
        mDsState = nullptr;
    }
    if (mBdState)
    {
        mBdState->Release();
        mBdState = nullptr;
    }
    if (mVertexLayout)
    {
        mVertexLayout->Release();
        mVertexLayout = nullptr;
    }
    if (mConstBuffer)
    {
        mConstBuffer->Release();
        mConstBuffer = nullptr;
    }

    for each (auto mat in mMaterials)
    {
        if (mat->sampler)
        {
            mat->sampler->Release();
            mat->sampler = nullptr;
        }
        if (mat->shaderResView)
        {
            mat->shaderResView->Release();
            mat->shaderResView = nullptr;
        }
        if (mat->texture)
        {
            mat->texture->Release();
            mat->texture = nullptr;
        }
        delete mat;
    }
    mMaterials.clear();

    for each (auto mesh in mMeshData)
    {
        if (mesh->vertexBuffer)
        {
            mesh->vertexBuffer->Release();
            mesh->vertexBuffer = nullptr;
        }
        if (mesh->indexBuffer)
        {
            mesh->indexBuffer->Release();
            mesh->indexBuffer = nullptr;
        }
        delete[] mesh->vertices;
        delete[] mesh->indices;
        delete mesh;
    }
    mMeshData.clear();

    mFile.Destroy();
}

void ModelMqo::Render(ID3D11DeviceContext* context)
{
    //定数バッファ
    ConstBuffer cbuff;

    // プロジェクション行列
    float aspect = NS_FW_CONST::WIDTH / NS_FW_CONST::HEIGHT;//アスペクト比
    float min_z = 0.01f;
    float max_z = 10000.0f;
    float fov = DirectX::XM_PIDIV4;//画角
    cbuff.mtxProj = DirectX::XMMatrixTranspose(DirectX::XMMatrixPerspectiveFovLH(fov, aspect, min_z, max_z));

    auto scene = mFile.GetScene();
    // カメラ行列
    DirectX::XMVECTOR Eye = DirectX::XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
    DirectX::XMVECTOR At = DirectX::XMVectorSet(scene->lookat[0], scene->lookat[1], scene->lookat[2], 0.0f);
    DirectX::XMVECTOR Up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    cbuff.mtxView = DirectX::XMMatrixTranspose(DirectX::XMMatrixLookAtLH(Eye, At, Up));
    static float rotY = 0;
    Matrix44 matRot = DirectX::XMMatrixRotationY(rotY); rotY += 0.01f;
    Matrix44 matTrans = DirectX::XMMatrixTranslation(scene->pos[0], scene->pos[1], scene->pos[2]);
    cbuff.mtxWorld = DirectX::XMMatrixTranspose(DirectX::XMMatrixMultiply(matRot, matTrans));
    cbuff.Diffuse = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
    // シェーダーでは行列を転置してから渡す

    // 定数バッファ内容更新
    context->UpdateSubresource(mConstBuffer, 0, NULL, &cbuff, 0, 0);

    for each(auto mesh in mMeshData)
    {
        if (mesh->visible == 0) continue;

        // 頂点バッファ
        uint32_t vb_slot = 0;
        ID3D11Buffer* vb[] = { mesh->vertexBuffer };
        uint32_t stride[] = { sizeof(VertexData) };
        uint32_t offset[] = { 0 };
        context->IASetVertexBuffers(vb_slot, ARRAYSIZE(vb), vb, stride, offset);

        // 入力レイアウト
        context->IASetInputLayout(mVertexLayout);

        // インデックスバッファ
        //context->IASetIndexBuffer(mesh->indexBuffer, DXGI_FORMAT_R32_UINT, 0);

        // プリミティブ形状
        context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

        // シェーダ
        context->VSSetShader(mVertexShader, nullptr, 0);
        context->PSSetShader(mPixelShader, nullptr, 0);

        // 定数バッファ
        uint32_t cb_slot = 0;
        ID3D11Buffer* cb[1] = { mConstBuffer };
        context->VSSetConstantBuffers(cb_slot, 1, cb);
        context->PSSetConstantBuffers(cb_slot, 1, cb);

        // マテリアル
        auto material = mMaterials.at(mesh->material_id);
        // サンプラー
        if (material->texture)
        {
            uint32_t smp_slot = 0;
            ID3D11SamplerState* smp[1] = { material->sampler };
            context->PSSetSamplers(smp_slot, ARRAYSIZE(smp), smp);

            // シェーダーリソースビュー（テクスチャ）
            uint32_t srv_slot = 0;
            ID3D11ShaderResourceView* srv[1] = { material->shaderResView };
            context->PSSetShaderResources(srv_slot, ARRAYSIZE(srv), srv);
        }

        // ラスタライザステート
        context->RSSetState(mRsState);

        // デプスステンシルステート
        context->OMSetDepthStencilState(mDsState, 0);

        // ブレンドステート
        context->OMSetBlendState(mBdState, NULL, 0xfffffff);

        // ポリゴン描画
        context->Draw(mesh->vertex_num, 0);
    }
}

NS_FW_GFX_END
