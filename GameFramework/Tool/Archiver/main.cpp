#include <iostream>
#include <fstream>
#include <memory>
#include <direct.h>
#include "Includes.h"

void PrintUsage()
{
    std::cout << "Usage:" << std::endl;
    std::cout << "    Archiver.exe [-a] [-e] -o <output> -i <file> ..." << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "    -a   指定されたファイルを圧縮する" << std::endl;
    std::cout << "    -e   指定されたファイルを展開する" << std::endl;
    std::cout << "    -i   入力ファイル（圧縮対象・展開対象）パスの指定" << std::endl; 
    std::cout << "    -o   出力ファイル（フォルダ）パスの指定" << std::endl;
}

const wchar_t* GetExtension(const wchar_t* path, std::wstring& dest)
{
    std::wstring fullpath = path;
    const size_t path_i = fullpath.find_last_of(L"\\") + 1;
    const size_t ext_i = fullpath.find_last_of(L".");
    dest = fullpath.substr(ext_i, fullpath.size() - ext_i).c_str();
    return dest.c_str();
}

const wchar_t* GetFilename(const wchar_t* path, std::wstring& dest)
{
    std::wstring fullpath = path;
    const size_t path_i = fullpath.find_last_of(L"\\") + 1;
    dest = fullpath.substr(path_i).c_str();
    return dest.c_str();
}

const wchar_t* GetFilenameWithoutExtension(const wchar_t* path, std::wstring& dest)
{
    std::wstring fullpath = path;
    const size_t path_i = fullpath.find_last_of(L"\\") + 1;
    const size_t ext_i = fullpath.find_last_of(L".");
    dest = fullpath.substr(path_i, ext_i - path_i).c_str();
    return dest.c_str();
}

const wchar_t* GetDirectoryPath(const wchar_t* path, std::wstring& dest)
{
    std::wstring fullpath = path;
    const size_t path_i = fullpath.find_last_of(L"\\") + 1;
    const size_t ext_i = fullpath.find_last_of(L".");
    dest = fullpath.substr(0, path_i + 1).c_str();
    return dest.c_str();
}

int wmain(const int argc, const wchar_t*argv[])
{
    enum
    {
        Error,
        Archive,
        Extract
    }
    mode = Error;

    std::wstring output;
    auto file_list = std::vector<std::wstring>(); 

    for (s32 argindex = 1; argindex< argc; ++argindex)
    {
        if (wcscmp(L"-a", argv[argindex]) == 0)
        {
            mode = Archive;
        }
        else if (wcscmp(L"-e", argv[argindex]) == 0)
        {
            mode = Extract;
        }
        else if (wcscmp(L"-o", argv[argindex]) == 0)
        {
            output = argv[++argindex];
        }
        else if (wcscmp(L"-i", argv[argindex]) == 0)
        {
            std::wstring path = argv[++argindex];
            std::ifstream ifs(path);
            if (ifs.is_open())
            {
                file_list.push_back(path);
            }
            ifs.close();
        }
        else
        {
            mode = Error;
            break;
        }
    }

    if (mode == Error || output.empty() || file_list.empty())
    {
        PrintUsage();
        return 1;
    }

    Framework::Tool::Archiver archiver;

    if (mode == Archive)
    {
        for each (auto path in file_list)
        {
            std::ifstream binfile(path, std::ios::in | std::ios::binary);

            if (binfile.is_open())
            {
                int fsize = static_cast<int>(binfile.seekg(0, std::ios::end).tellg());
                binfile.seekg(0, std::ios::beg);
                std::unique_ptr<char> data(new char[fsize]);
                binfile.read(data.get(), fsize);
                std::wstring dest;
                archiver.Add(GetFilename(path.c_str(), dest), data.get(), fsize);
            }
            else
            {
                // 開けなかったファイルがある
                return 2;
            }
        }

        std::ofstream binfile(output, std::ios::out | std::ios::binary);
        std::unique_ptr<char> file(new char[archiver.GetBinarySize()]);
        archiver.WriteBinary(file.get());
        binfile.write(file.get(), archiver.GetBinarySize());
        binfile.close();
    }
    else if (mode == Extract)
    {
        for each (auto path in file_list)
        {
            std::ifstream binfile(path, std::ios::in | std::ios::binary);

            if (binfile.is_open())
            {
                int fsize = static_cast<int>(binfile.seekg(0, std::ios::end).tellg());
                binfile.seekg(0, std::ios::beg);
                std::unique_ptr<char> data(new char[fsize]);
                binfile.read(data.get(), fsize);
                archiver.ReadBinary(data.get(), fsize);

                std::wstring dir;
                std::wstring output_dir = output + L"\\" + GetFilenameWithoutExtension(path.c_str(), dir) + L"\\";

                if (_wmkdir(output_dir.c_str()) == 0)
                {
                    for (u32 i = 0; i < archiver.GetDataCount(); ++i)
                    {
                        std::ofstream file(output_dir + archiver.GetDataName(i), std::ios::out | std::ios::binary);
                        std::unique_ptr<char> data(new char[archiver.GetDataSize(i)]);
                        archiver.GetData(i, data.get());
                        file.write(data.get(), archiver.GetDataSize(i));
                        file.close();
                    }
                }
                else
                {
                    return 3;
                }
                break;
            }
            else
            {
                // 開けなかったファイルがある
                return 2;
            }
        }
    }
    return 0;
}