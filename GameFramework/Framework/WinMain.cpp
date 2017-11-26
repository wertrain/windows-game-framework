#ifdef _WIN64

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
#include "Common/Includes.h"
#include "Core/DirectX.h"
#include "System/Application.h"
#include "GameMain.h"
#include "Constants.h"

struct ThreadParam 
{
    DWORD id;       // スレッドID
    HANDLE handle;  // スレッドハンドル
    bool exit;      // 終了フラグ
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

    Framework::System::Application& application = Framework::System::Application::GetInstance();
    HWND hwnd = application.GetWindowHandle();
    Framework::System::DirectX* directX = application.GetDirectX();

    // ゲームループ
    while (IsWindow(hwnd))
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
        directX->ClearRenderView();

        Draw(directX->GetDeviceContext());

        directX->Present();

        // 理想時間の算出
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
            // スレッドを終了させる
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
    // メモリリークチェック
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    Framework::System::Application& application = Framework::System::Application::GetInstance();
    if (!application.Create(hInstance, WndProc))
    {
        return 0;
    }
    Framework::System::DirectX* directX = application.GetDirectX();

    // ゲームオブジェクトの作成
    if (!Create(directX->GetDevice(), directX->GetDeviceContext()))
    {
        //assert( !"ゲームオブジェクトの作成に失敗しました。" );
        return 0;
    }

    // スレッドの作成と実行
    s_ThreadParam.exit = false;
    s_ThreadParam.handle = CreateThread(
        NULL,                              // ハンドルを他のプロセスと共有する場合
        0,                                 // スタックサイズ(デフォルト:0)
        GameMainFunc,                      // スレッド関数名
        reinterpret_cast<LPVOID>(&s_ThreadParam), // スレッドに渡す構造体
        0,                                 // 0:作成と同時に実行
        &s_ThreadParam.id);     // スレッドID

    application.GetInstance().ShowWindow(nCmdShow); // ウィンドウ表示

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
