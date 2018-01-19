#include "../../Common/Debug.h"

#include "ModelMqo.h"
#include <regex>

NS_FW_GFX_BEGIN

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
                if (tp != NULL) ##var.##elem = static_cast<f32>(atof(tp));          \
            }                                                                       \
        }

#define NOPARAM_GET_FLOAT_ARRAY(var, elem)                                          \
        {                                                                           \
            int index = 0;                                                          \
            do                                                                      \
            {                                                                       \
                if (tp != NULL) ##var.##elem[index++] = static_cast<f32>(atof(tp)); \
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
                if (tp != NULL) ##var.##elem[index++] = static_cast<f32>(atof(tp)); \
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
                p->##member = static_cast<f32>(atof(results[1].str().c_str()));  \
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
                    p->##member[i] = static_cast<f32>(atof(results[i + 1].str().c_str())); \
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
            p->faces[face_index].V = new s32[v_num];
            memset(p->faces[face_index].V, 0, sizeof(s32) * v_num);
            p->faces[face_index].UV = new f32[v_num * 2];
            memset(p->faces[face_index].UV, 0, sizeof(f32) * v_num * 2);

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
{

}

ModelMqo::~ModelMqo()
{

}

bool ModelMqo::Create(const wchar_t* filename)
{
    return mFile.Read(filename);
}

void ModelMqo::Destroy()
{
    mFile.Destroy();
}

NS_FW_GFX_END
