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
#include "System/DirectX.h"
#include "GameMain.h"
#include "Constants.h"

namespace Framework {

struct ThreadParam 
{
    DWORD id;       // �X���b�hID
    HANDLE handle;  // �X���b�h�n���h��
    bool exit;      // �I���t���O

    System::DirectX directX;
    HWND hWnd;
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

    // �Q�[�����[�v
    while (IsWindow(param->hWnd))
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

        //Draw(gameWindow->hScreenDC);

        param->directX.ClearRenderView();
        param->directX.Present();

        // ���z���Ԃ̎Z�o
        DWORD idealTime = (DWORD)(frames * (1000.0 / Constants::FPS));
        if (idealTime > progress)
        {
            Sleep(idealTime - progress);
        }


        if (param->exit) break;
    }
    return TRUE;
}

} // namespace Framework 

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    DWORD dwParam;

    switch(msg)
    {
        case WM_DESTROY:
            // �X���b�h���I��������
            Framework::s_ThreadParam.exit = true;
            GetExitCodeThread(Framework::s_ThreadParam.handle, &dwParam);
            WaitForSingleObject(Framework::s_ThreadParam.handle, INFINITE);
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

    HWND hwnd;
    WNDCLASSEX winc;
    MSG msg;

    // �����̏���S��0
    ZeroMemory(&winc, sizeof(WNDCLASSEX));

    winc.cbSize = sizeof(WNDCLASSEX);
    winc.style = CS_HREDRAW | CS_VREDRAW;
    winc.lpfnWndProc = WndProc;
    winc.cbClsExtra = winc.cbWndExtra = 0;
    winc.hInstance = hInstance;
    winc.hIcon = LoadIcon(NULL , IDI_APPLICATION);
    winc.hCursor = LoadCursor(NULL , IDC_ARROW);
    winc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    winc.lpszClassName = TEXT("GameFramework");

    if (!RegisterClassEx(&winc)) return 0;

    DWORD dwStyle = WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX; // �E�B���h�E�̑傫���ύX�ł��Ȃ��悤��

    hwnd = CreateWindowEx(
        0, TEXT("GameFramework") , TEXT("Windows GameFramework") ,
        dwStyle,
        CW_USEDEFAULT, CW_USEDEFAULT ,
        CW_USEDEFAULT, CW_USEDEFAULT ,
        NULL, NULL ,
        hInstance, NULL
    );

    if (hwnd == NULL) return 0;

    // DirectX�̏�����    
    Framework::System::DirectX* directX = &Framework::s_ThreadParam.directX;
    directX->Initialize(hwnd, Framework::Constants::WIDTH, Framework::Constants::HEIGHT);

    // �Q�[���I�u�W�F�N�g�̍쐬
    if (!Create(directX->GetDevice(), directX->GetDeviceContext()))
    {
        //assert( !"�Q�[���I�u�W�F�N�g�̍쐬�Ɏ��s���܂����B" );
        return 0;
    }

    // �E�B���h�E�̃T�C�Y����
    RECT window_rect;
    SetRect(&window_rect, 0, 0, Framework::Constants::WIDTH, Framework::Constants::HEIGHT);
    AdjustWindowRectEx(&window_rect, GetWindowLong(hwnd,GWL_STYLE), GetMenu(hwnd) != NULL, GetWindowLong(hwnd,GWL_EXSTYLE));
    const int nWidth  = window_rect.right  - window_rect.left;
    const int nHeight = window_rect.bottom - window_rect.top;
    SetWindowPos(hwnd, NULL, 0, 0, nWidth, nHeight, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);

    // �X���b�h�̍쐬�Ǝ��s
    Framework::s_ThreadParam.hWnd = hwnd;
    Framework::s_ThreadParam.exit = false;
    Framework::s_ThreadParam.handle = CreateThread(
        NULL,                              // �n���h���𑼂̃v���Z�X�Ƌ��L����ꍇ
        0,                                 // �X�^�b�N�T�C�Y(�f�t�H���g:0)
        Framework::GameMainFunc,           // �X���b�h�֐���
        reinterpret_cast<LPVOID>(&Framework::s_ThreadParam), // �X���b�h�ɓn���\����
        0,                                 // 0:�쐬�Ɠ����Ɏ��s
        &Framework::s_ThreadParam.id);     // �X���b�hID

    ShowWindow(hwnd, nCmdShow); // �E�B���h�E�\��

    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        DispatchMessage(&msg);
    }

    Destroy();

    directX->Finalize();

    return 0;
}
