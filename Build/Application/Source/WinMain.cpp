/**
 * @file WinMain.cpp
 * @brief Windows ���C���֐�
 */
#ifdef _WIN64

// ���������[�N���o�̗L����
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h> 

#include <windows.h>
#include <time.h>
#include <d3d11.h>

#include <nnfw/common/Includes.h>
#include <nnfw/core/DX11Manager.h>
#include <nnfw/sys/Application.h>
#include <nnfw/gfx/Text.h>
#include <nnfw/GameMain.h>
#include <nnfw/Constants.h>

struct ThreadParam 
{
    DWORD id;       // �X���b�hID
    HANDLE handle;  // �X���b�h�n���h��
    bool exit;      // �I���t���O
}
s_ThreadParam;

DWORD WINAPI GameMainFunc(LPVOID vdParam)
{
    ThreadParam* param = reinterpret_cast<ThreadParam*>(vdParam);

    // �����W�F�l���[�^������
    srand(static_cast<unsigned>(time(NULL)));
    
    NS_FW_SYS::Application& application = NS_FW_SYS::Application::GetInstance();
    NS_FW_SYS::DX11Manager& dxMgr = NS_FW_SYS::DX11Manager::GetInstance();
    NS_FW_GFX::DefaultFontManager& fontMgr = NS_FW_GFX::DefaultFontManager::GetInstance();

    // FPS �v�Z���@
    // http://javaappletgame.blog34.fc2.com/blog-entry-265.html
    long idleTime = (1000 << 16) / NS_FW_CONST::FPS;
    long beforeTime = timeGetTime() << 16;
    long errorTime = 0;
    long progressTime = 0;
    // �t���[����
    long nFPS = 0, nFpsCount = 0;
    //double dFPS = 0, dFpsCount = 0;

    // timeBeginPeriod ���g���� 60 �҂�����ɂȂ邪�A�S�v���Z�X�ɉe������炵��...
    //timeBeginPeriod(1);
    //timeEndPeriod(1);
    
    // �Q�[�����[�v
    HWND hwnd = application.GetWindowHandle();
    while (IsWindow(hwnd))
    {
        // �o�ߎ��Ԃ��Z�o
        long nowTime = timeGetTime() << 16;
        long elapsedTime = (nowTime - beforeTime);
        beforeTime = nowTime;

        // --- �Q�[������ ---

        Update((elapsedTime >> 16), (nowTime >> 16));

        // --- �`�揈�� ---

        dxMgr.ClearRenderView();

        Draw(dxMgr.GetDeviceContext());

        fontMgr.Render(dxMgr.GetDeviceContext());
        fontMgr.SetTextFormat(NS_FW_GFX::DefaultFontManager::eID_System, 1, 1, L"FPS:%ld", nFPS);
        //fontMgr.SetTextFormat(NS_FW_GFX::DefaultFontManager::eID_System, 1, 1, L"FPS:%.2f", dFPS);

        dxMgr.Present();

        // --- �X���[�v���� ---
        
        nowTime = (timeGetTime() << 16);
        long sleepTime = idleTime - (nowTime - beforeTime) - errorTime;
       
        if (sleepTime < (2 << 16)) sleepTime = (2 << 16); // �Œ�ł�2ms�͋x�~
        beforeTime = nowTime;

        //timeBeginPeriod(1); 
        Sleep(sleepTime >> 16);
        //timeEndPeriod(1);
        
        nowTime = (timeGetTime() << 16); 
        errorTime = (nowTime - beforeTime) - sleepTime;

        // --- FPS�J�E���g���� ---

        if ((progressTime += elapsedTime) >= 1000 << 16)
        {
            progressTime = 0;

            nFPS = nFpsCount;
            nFpsCount = 0;
            //dFPS = dFpsCount;
            //dFpsCount = 0;
        }
        else
        {
            //dFpsCount += static_cast<double>(nowTime - beforeTime) / static_cast<double>(idleTime);
            nFpsCount += 1;
        }
        if (param->exit) break;
    }
    return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    DWORD dwParam;

    switch(msg)
    {
        case WM_DESTROY:
            // �X���b�h���I��������
            s_ThreadParam.exit = true;
            GetExitCodeThread(s_ThreadParam.handle, &dwParam);
            WaitForSingleObject(s_ThreadParam.handle, INFINITE);
            PostQuitMessage(0);
            return 0;

        case WM_CREATE:
            return 0;

        case WM_PAINT:
            return 0;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, PSTR /*lpCmdLine*/, int nCmdShow) 
{
    // ���������[�N�`�F�b�N
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    // �A�v���P�[�V�����̏�����
    NS_FW_SYS::Application& application = NS_FW_SYS::Application::GetInstance();
    if (!application.Create(hInstance, WndProc))
    {
        application.ShowMessageBox(L"�G���[", L"�A�v���P�[�V�����̏������Ɏ��s���܂����B", NS_FW_SYS::Application::eMB_Error); 
        return 0;
    }

    // DirectX �̏�����
    NS_FW_SYS::DX11Manager& dxMgr = NS_FW_SYS::DX11Manager::GetInstance();
    if (!NS_FW_SYS::DX11Manager::GetInstance().Initialize(application.GetWindowHandle(), NS_FW_CONST::WIDTH, NS_FW_CONST::HEIGHT))
    {
        application.ShowMessageBox(L"�G���[", L"DirectX �̏������Ɏ��s���܂����B", NS_FW_SYS::Application::eMB_Error);
        return 0;
    }

    // �V�X�e���֘A������
    NS_FW_GFX::DefaultFontManager& fontMgr = NS_FW_GFX::DefaultFontManager::GetInstance();
    for (int i = 0; i < NS_FW_GFX::DefaultFontManager::eID_Num; ++i)
    {
        if (!fontMgr.CreateLayer(dxMgr.GetDevice(), dxMgr.GetDeviceContext(), i, i))
        {
            application.ShowMessageBox(L"�G���[", L"�V�X�e���t�H���g�̏������Ɏ��s���܂����B", NS_FW_SYS::Application::eMB_Error);
            return 0;
        }
    }

    // �Q�[���I�u�W�F�N�g�̍쐬
    if (!Create(dxMgr.GetDevice(), dxMgr.GetDeviceContext()))
    {
        application.ShowMessageBox(L"�G���[", L"�Q�[���I�u�W�F�N�g�̏������Ɏ��s���܂����B", NS_FW_SYS::Application::eMB_Error);
        return 0;
    }

    // �X���b�h�̍쐬�Ǝ��s
    s_ThreadParam.exit = false;
    s_ThreadParam.handle = CreateThread(
        NULL,                                     // �n���h���𑼂̃v���Z�X�Ƌ��L����ꍇ
        0,                                        // �X�^�b�N�T�C�Y(�f�t�H���g:0)
        GameMainFunc,                             // �X���b�h�֐���
        reinterpret_cast<LPVOID>(&s_ThreadParam), // �X���b�h�ɓn���\����
        0,                                        // 0: �쐬�Ɠ����Ɏ��s
        &s_ThreadParam.id);                       // �X���b�hID

    // �E�B���h�E�\��
    application.ShowWindow(nCmdShow);

    // ���b�Z�[�W����
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        DispatchMessage(&msg);
    }

    // �j�������̊J�n
    Destroy();

    fontMgr.Destroy();
    
    dxMgr.Finalize();
    
    application.Destroy();

    return 0;
}

#endif // _WIN64
