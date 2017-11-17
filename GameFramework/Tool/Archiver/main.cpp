#include <iostream>
#include <fstream>
#include <memory>
#include "Includes.h"

void PrintUsage()
{
    std::cout << "Usage:" << std::endl;
    std::cout << "    Archiver.exe [-a] [-e] <file> ..." << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "    -a   指定されたファイルを圧縮する" << std::endl;
    std::cout << "    -e   指定されたファイルを展開する" << std::endl;
}

int wmain(const int argc, const wchar_t*argv[])
{
    if (argc <= 2)
    {
        PrintUsage();
        return 1;
    }

    enum
    {
        Archive,
        Extract
    }
    mode;
    
    if (wcscmp(L"-a", argv[1]) == 0)
    {
        mode = Archive;
    }
    else if (wcscmp(L"-e", argv[1]) == 0)
    {
        mode = Extract;
    }
    else
    {
        PrintUsage();
        return 1;
    }

    auto file_list = std::vector<std::wstring>();
    for (s32 index = 2; index < argc; ++index)
    {
        std::wstring path = argv[index];
        std::ifstream ifs(path);
        if (ifs.is_open())
        {
            file_list.push_back(path);
        }
        ifs.close();
    }

    if (file_list.empty())
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
                archiver.Add(L"test.dds", data.get(), fsize);
            }
            else
            {
                // 開けなかったファイルがある
                return 2;
            }
        }

        std::ofstream binfile("test.bin", std::ios::out | std::ios::binary);
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
                for (u32 i = 0; i < archiver.GetDataCount(); ++i)
                {
                    std::wcout << archiver.GetDataName(i) << std::endl;
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