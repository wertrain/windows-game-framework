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

        }
    }
    fclose(fp);
    return true;
}

/// パラメータ取得マクロ

#define PARAM_GET_START char* tp = strtok_s(buffer, " ", &buffer)

#define PARAM_GET(var, elem)                                                            \
        (strstr(tp, #elem) != NULL)                                                     \
        {                                                                               \
                while (tp != NULL)                                                      \
                {                                                                       \
                    tp = strtok_s(NULL, " ", &buffer);                                  \
                    if (tp != NULL) ##var.##elem = tp;                                  \
                }                                                                       \
        }

#define PARAM_GET_INT(var, elem)                                                        \
        (strstr(tp, #elem) != NULL)                                                     \
        {                                                                               \
                while (tp != NULL)                                                      \
                {                                                                       \
                    tp = strtok_s(NULL, " ", &buffer);                                  \
                    if (tp != NULL) ##var.##elem = atoi(tp);                            \
                }                                                                       \
        }

#define PARAM_GET_INT_ARRAY(var, elem)                                                  \
        (strstr(tp, #elem) != NULL)                                                     \
        {                                                                               \
                int index = 0;                                                          \
                while (tp != NULL)                                                      \
                {                                                                       \
                    tp = strtok_s(NULL, " ", &buffer);                                  \
                    if (tp != NULL) ##var.##elem[index++] = atoi(tp);                   \
                }                                                                       \
        }

#define PARAM_GET_FLOAT(var, elem)                                                      \
        (strstr(tp, #elem) != NULL)                                                     \
        {                                                                               \
                while (tp != NULL)                                                      \
                {                                                                       \
                    tp = strtok_s(NULL, " ", &buffer);                                  \
                    if (tp != NULL) ##var.##elem = static_cast<f32>(atof(tp));          \
                }                                                                       \
        }

#define PARAM_GET_FLOAT_ARRAY(var, elem)                                                \
        (strstr(tp, #elem) != NULL)                                                     \
        {                                                                               \
                int index = 0;                                                          \
                while (tp != NULL)                                                      \
                {                                                                       \
                    tp = strtok_s(NULL, " ", &buffer);                                  \
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
                for (int i = 0; i < results.size(); ++i)                                   \
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

NS_FW_GFX_END
