#ifdef _WIN64

// ���������[�N���o�̗L����
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h> 

#include <windows.h>
#include <time.h>
#include <tchar.h>
#include <mmsystem.h>
#pragma comment (lib, "winmm.lib")

#include <d3d11.h>
#include <atlbase.h>
#include <cstdint>
#include "Common/Includes.h"
#include "Core/DirectX.h"
#include "System/Application.h"
#include "GameMain.h"
#include "Constants.h"

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

    // �t���[�����ƈȑO�̎���
    DWORD frames = 0, beforeTime;
    // FPS�̕\���p
    TCHAR str[16] = {0};
    // �L�[�{�[�h�̏�Ԃ��i�[
    //BYTE keyTable[256];

    // �����W�F�l���[�^������
    srand(static_cast<unsigned>(time(NULL)));
    
    // �������Ԃ̎擾
    beforeTime = timeGetTime();

    Framework::System::Application& application = Framework::System::Application::GetInstance();
    HWND hwnd = application.GetWindowHandle();
    Framework::System::DirectX* directX = application.GetDirectX();

    // �Q�[�����[�v
    while (IsWindow(hwnd))
    {
        DWORD nowTime, progress;
        // ���݂̎��Ԃ��擾
        nowTime = timeGetTime();
        // �o�ߎ��Ԃ��Z�o
        progress = nowTime - beforeTime;
        
        // --- �Q�[������ ---

        // �L�[���̍X�V
        //if (!GetKeyboardState(keyTable))
        //{
        //    MessageBox( gameWindow->hWnd, "�L�[���̎擾�Ɏ��s", "ERROR", MB_OK);
        //    return false;
        //}

        Update(nowTime);

        // --- �`�揈�� ---
        directX->ClearRenderView();

        Draw(directX->GetDeviceContext());

        directX->Present();

        // ���z���Ԃ̎Z�o
        DWORD idealTime = (DWORD)(frames * (1000.0 / Framework::Constants::FPS));
        if (idealTime > progress)
        {
            Sleep(idealTime - progress);
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

    Framework::System::Application& application = Framework::System::Application::GetInstance();
    if (!application.Create(hInstance, WndProc))
    {
        return 0;
    }
    Framework::System::DirectX* directX = application.GetDirectX();

    // �Q�[���I�u�W�F�N�g�̍쐬
    if (!Create(directX->GetDevice(), directX->GetDeviceContext()))
    {
        //assert( !"�Q�[���I�u�W�F�N�g�̍쐬�Ɏ��s���܂����B" );
        return 0;
    }

    // �X���b�h�̍쐬�Ǝ��s
    s_ThreadParam.exit = false;
    s_ThreadParam.handle = CreateThread(
        NULL,                              // �n���h���𑼂̃v���Z�X�Ƌ��L����ꍇ
        0,                                 // �X�^�b�N�T�C�Y(�f�t�H���g:0)
        GameMainFunc,                      // �X���b�h�֐���
        reinterpret_cast<LPVOID>(&s_ThreadParam), // �X���b�h�ɓn���\����
        0,                                 // 0:�쐬�Ɠ����Ɏ��s
        &s_ThreadParam.id);     // �X���b�hID

    application.GetInstance().ShowWindow(nCmdShow); // �E�B���h�E�\��

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        DispatchMessage(&msg);
    }

    Destroy();

    application.Destroy();

    return 0;
}

#endif // _WIN64
