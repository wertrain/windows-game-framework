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

static bool s_ExitThread = false;

// �Q�[���E�B���h�E�\����
typedef struct GameWindow
{
    System::DirectX directX;
    HWND hWnd;     // �E�C���h�E
    SIZE size;     // �T�C�Y
    DWORD dwFps;   // FPS
} GameWindow;

DWORD WINAPI GameMainFunc(LPVOID vdParam)
{
    GameWindow *gameWindow = static_cast<GameWindow*>(vdParam);
    gameWindow->directX.Initialize(gameWindow->hWnd, Constants::WIDTH, Constants::HEIGHT);

    // �t���[�����ƈȑO�̎���
    DWORD frames = 0, beforeTime;
    // FPS�̕\���p
    TCHAR str[16] = {0};
    // �L�[�{�[�h�̏�Ԃ��i�[
    //BYTE keyTable[256];

    // �f�o�C�X�R���e�L�X�g
    HDC hdc;
    // �`��\����
    PAINTSTRUCT ps;

    // �����W�F�l���[�^������
    srand(static_cast<unsigned>(time(NULL)));
    
    // �������Ԃ̎擾
    beforeTime = timeGetTime();

    // �Q�[�����[�v
    while (IsWindow(gameWindow->hWnd))
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

        gameWindow->directX.ClearRenderView();
        gameWindow->directX.Present();

        // ���z���Ԃ̎Z�o
        DWORD idealTime = (DWORD)(frames * (1000.0 / gameWindow->dwFps));
        if (idealTime > progress)
        {
            Sleep(idealTime - progress);
        }


        if (Framework::s_ExitThread) break;
    }

    gameWindow->directX.Finalize();

    return TRUE;
}

} // namespace Framework 

LRESULT CALLBACK WndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    // �f�o�C�X�R���e�L�X�g
    HDC hdc;
    // �`��\����
    //PAINTSTRUCT ps;

    // �Q�[���E�B���h�E
    static Framework::GameWindow gameWindow;
    // BITMAP�n���h��
    static HBITMAP hBitmap;
    // �X���b�hID
    DWORD dwID;
    // �X���b�h�n���h��
    static HANDLE hThread;
    DWORD dwParam;

    switch(msg)
    {
        case WM_DESTROY:
            // �X���b�h���I��������
            Framework::s_ExitThread = true;
            GetExitCodeThread(hThread, &dwParam);
            WaitForSingleObject(hThread, INFINITE);
            Destroy();
            PostQuitMessage(0);
            _CrtDumpMemoryLeaks();
            return 0;

        case WM_CREATE:

            // GameWindow�\���̂̐ݒ�
            gameWindow.hWnd = hWnd;
            gameWindow.size.cx = Framework::Constants::WIDTH;
            gameWindow.size.cy = Framework::Constants::HEIGHT;
            gameWindow.dwFps = Framework::Constants::FPS;

            // �Q�[���I�u�W�F�N�g�̍쐬
            if (!Create(gameWindow.hWnd))
            {
                //assert( !"�Q�[���I�u�W�F�N�g�̍쐬�Ɏ��s���܂����B" );
                return 0;
            }

            // �X���b�h�̍쐬�Ǝ��s
            hThread = CreateThread(NULL,          // �n���h���𑼂̃v���Z�X�Ƌ��L����ꍇ
                0,                      // �X�^�b�N�T�C�Y(�f�t�H���g:0)
                Framework::GameMainFunc,// �X���b�h�֐���
                (LPVOID)&gameWindow,    // �X���b�h�ɓn���\����
                0,                      // 0:�쐬�Ɠ����Ɏ��s
                &dwID);                 // �X���b�hID
        
            return 0;

        case WM_PAINT:
            return 0;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, PSTR /*lpCmdLine*/, int nCmdShow) 
{
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
        0, TEXT("GameFramework") , TEXT("Windows Native GameFramework") ,
        dwStyle,
        CW_USEDEFAULT, CW_USEDEFAULT ,
        CW_USEDEFAULT, CW_USEDEFAULT ,
        NULL, NULL ,
        hInstance, NULL
    );

    if (hwnd == NULL) return 0;

    // �E�B���h�E�̃T�C�Y����
    RECT window_rect;
    SetRect(&window_rect, 0, 0, Framework::Constants::WIDTH, Framework::Constants::HEIGHT);
    AdjustWindowRectEx(&window_rect, GetWindowLong(hwnd,GWL_STYLE), GetMenu(hwnd) != NULL, GetWindowLong(hwnd,GWL_EXSTYLE));
    const int nWidth  = window_rect.right  - window_rect.left;
    const int nHeight = window_rect.bottom - window_rect.top;
    SetWindowPos(hwnd, NULL, 0, 0, nWidth, nHeight, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);

    ShowWindow(hwnd, nCmdShow); // �E�B���h�E�\��

    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        DispatchMessage(&msg);
    }

    return 0;
}
