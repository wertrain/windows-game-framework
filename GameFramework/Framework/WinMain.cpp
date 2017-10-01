// メモリリーク検出の有効化
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
    DWORD id;       // スレッドID
    HANDLE handle;  // スレッドハンドル
    bool exit;      // 終了フラグ

    System::DirectX directX;
    HWND hWnd;
}
s_ThreadParam;

DWORD WINAPI GameMainFunc(LPVOID vdParam)
{
    ThreadParam* param = reinterpret_cast<ThreadParam*>(vdParam);

    // フレーム数と以前の時間
    DWORD frames = 0, beforeTime;
    // FPSの表示用
    TCHAR str[16] = {0};
    // キーボードの状態を格納
    //BYTE keyTable[256];

    // 乱数ジェネレータ初期化
    srand(static_cast<unsigned>(time(NULL)));
    
    // 初期時間の取得
    beforeTime = timeGetTime();

    // ゲームループ
    while (IsWindow(param->hWnd))
    {
        DWORD nowTime, progress;
        // 現在の時間を取得
        nowTime = timeGetTime();
        // 経過時間を算出
        progress = nowTime - beforeTime;
        
        // --- ゲーム処理 ---

        // キー情報の更新
        //if (!GetKeyboardState(keyTable))
        //{
        //    MessageBox( gameWindow->hWnd, "キー情報の取得に失敗", "ERROR", MB_OK);
        //    return false;
        //}

        Update(nowTime);

        // --- 描画処理 ---

        //Draw(gameWindow->hScreenDC);

        param->directX.ClearRenderView();
        param->directX.Present();

        // 理想時間の算出
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
            // スレッドを終了させる
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
    // メモリリークチェック
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    HWND hwnd;
    WNDCLASSEX winc;
    MSG msg;

    // 内部の情報を全て0
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

    DWORD dwStyle = WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX; // ウィンドウの大きさ変更できないように

    hwnd = CreateWindowEx(
        0, TEXT("GameFramework") , TEXT("Windows GameFramework") ,
        dwStyle,
        CW_USEDEFAULT, CW_USEDEFAULT ,
        CW_USEDEFAULT, CW_USEDEFAULT ,
        NULL, NULL ,
        hInstance, NULL
    );

    if (hwnd == NULL) return 0;

    // DirectXの初期化    
    Framework::System::DirectX* directX = &Framework::s_ThreadParam.directX;
    directX->Initialize(hwnd, Framework::Constants::WIDTH, Framework::Constants::HEIGHT);

    // ゲームオブジェクトの作成
    if (!Create(directX->GetDevice(), directX->GetDeviceContext()))
    {
        //assert( !"ゲームオブジェクトの作成に失敗しました。" );
        return 0;
    }

    // ウィンドウのサイズ調整
    RECT window_rect;
    SetRect(&window_rect, 0, 0, Framework::Constants::WIDTH, Framework::Constants::HEIGHT);
    AdjustWindowRectEx(&window_rect, GetWindowLong(hwnd,GWL_STYLE), GetMenu(hwnd) != NULL, GetWindowLong(hwnd,GWL_EXSTYLE));
    const int nWidth  = window_rect.right  - window_rect.left;
    const int nHeight = window_rect.bottom - window_rect.top;
    SetWindowPos(hwnd, NULL, 0, 0, nWidth, nHeight, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);

    // スレッドの作成と実行
    Framework::s_ThreadParam.hWnd = hwnd;
    Framework::s_ThreadParam.exit = false;
    Framework::s_ThreadParam.handle = CreateThread(
        NULL,                              // ハンドルを他のプロセスと共有する場合
        0,                                 // スタックサイズ(デフォルト:0)
        Framework::GameMainFunc,           // スレッド関数名
        reinterpret_cast<LPVOID>(&Framework::s_ThreadParam), // スレッドに渡す構造体
        0,                                 // 0:作成と同時に実行
        &Framework::s_ThreadParam.id);     // スレッドID

    ShowWindow(hwnd, nCmdShow); // ウィンドウ表示

    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        DispatchMessage(&msg);
    }

    Destroy();

    directX->Finalize();

    return 0;
}
