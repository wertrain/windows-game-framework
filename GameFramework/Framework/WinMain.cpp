#ifdef _WIN64

// メモリリーク検出の有効化
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h> 

#include <windows.h>
#include <time.h>
#include <d3d11.h>

#include "Common/Includes.h"
#include "Core/DirectX.h"
#include "System/Application.h"
#include "Graphics/Text.h"
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
    DWORD frames = 0, pframes = 0;
    // キーボードの状態を格納
    //BYTE keyTable[256];

    // 乱数ジェネレータ初期化
    srand(static_cast<unsigned>(time(NULL)));
    
    NS_FW_SYS::Application& application = NS_FW_SYS::Application::GetInstance();
    HWND hwnd = application.GetWindowHandle();
    NS_FW_SYS::DirectX* directX = application.GetDirectX();

    // FPS 計算方法
    // http://javaappletgame.blog34.fc2.com/blog-entry-265.html
    long idleTime = (1000 << 16) / NS_FW_CONST::FPS;
    long beforeTime = timeGetTime() << 16;
    long errorTime = 0;
    long progressTime = 0;
    // timeBeginPeriod を使えば 60 ぴったりになるが、全プロセスに影響するらしい...
    //timeBeginPeriod(1);
    //timeEndPeriod(1);

    // ゲームループ
    while (IsWindow(hwnd))
    {
        // 経過時間を算出
        long nowTime = timeGetTime() << 16;
        long elapsedTime = (nowTime - beforeTime);
        beforeTime = nowTime;

        // --- ゲーム処理 ---

        // キー情報の更新
        //if (!GetKeyboardState(keyTable))
        //{
        //    MessageBox( gameWindow->hWnd, "キー情報の取得に失敗", "ERROR", MB_OK);
        //    return false;
        //}

        Update((elapsedTime >> 16), (nowTime >> 16));

        // --- 描画処理 ---
        directX->ClearRenderView();

        Draw(directX->GetDeviceContext());

        NS_FW_GFX::DefaultFontManager::GetInstance().Render(directX->GetDeviceContext());
        NS_FW_GFX::DefaultFontManager::GetInstance().SetTextFormat(NS_FW_GFX::DefaultFontManager::eID_System, 1, 1, L"FPS:%ld", pframes);

        directX->Present();

        // スリープ時間の計算
        nowTime = (timeGetTime() << 16);
        long sleepTime = idleTime - (nowTime - beforeTime) - errorTime;
        if (sleepTime < (2 << 16)) sleepTime = (2 << 16); // 最低でも2msは休止
        beforeTime = nowTime;

        //timeBeginPeriod(1); 
        Sleep(sleepTime >> 16);
        //timeEndPeriod(1);
        
        nowTime = (timeGetTime() << 16); 
        errorTime = (nowTime - beforeTime) - sleepTime;

        progressTime += elapsedTime >> 16;
        if (progressTime >= 1000)
        {
            pframes = frames;
            frames = 0;
            progressTime = 0;
        }
        else
        {
            ++frames;
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

    NS_FW_SYS::Application& application = NS_FW_SYS::Application::GetInstance();
    if (!application.Create(hInstance, WndProc))
    {
        return 0;
    }
    NS_FW_SYS::DirectX* directX = application.GetDirectX();

    // システム関連初期化
    NS_FW_GFX::DefaultFontManager& fontMngr = NS_FW_GFX::DefaultFontManager::GetInstance();
    for (int i = 0; i < NS_FW_GFX::DefaultFontManager::eID_Num; ++i)
    {
        fontMngr.CreateLayer(directX->GetDevice(), directX->GetDeviceContext(), i, i);
    }

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

    NS_FW_GFX::DefaultFontManager::GetInstance().Destroy();

    application.Destroy();

    return 0;
}

#endif // _WIN64
