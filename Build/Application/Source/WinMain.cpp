/**
 * @file WinMain.cpp
 * @brief Windows メイン関数
 */
#ifdef _WIN64

// メモリリーク検出の有効化
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
    DWORD id;       // スレッドID
    HANDLE handle;  // スレッドハンドル
    bool exit;      // 終了フラグ
}
s_ThreadParam;

DWORD WINAPI GameMainFunc(LPVOID vdParam)
{
    ThreadParam* param = reinterpret_cast<ThreadParam*>(vdParam);

    // 乱数ジェネレータ初期化
    srand(static_cast<unsigned>(time(NULL)));
    
    NS_FW_SYS::Application& application = NS_FW_SYS::Application::GetInstance();
    NS_FW_SYS::DX11Manager& dxMgr = NS_FW_SYS::DX11Manager::GetInstance();
    NS_FW_GFX::DefaultFontManager& fontMgr = NS_FW_GFX::DefaultFontManager::GetInstance();

    // FPS 計算方法
    // http://javaappletgame.blog34.fc2.com/blog-entry-265.html
    long idleTime = (1000 << 16) / NS_FW_CONST::FPS;
    long beforeTime = timeGetTime() << 16;
    long errorTime = 0;
    long progressTime = 0;
    // フレーム数
    long nFPS = 0, nFpsCount = 0;
    //double dFPS = 0, dFpsCount = 0;

    // timeBeginPeriod を使えば 60 ぴったりになるが、全プロセスに影響するらしい...
    //timeBeginPeriod(1);
    //timeEndPeriod(1);
    
    // ゲームループ
    HWND hwnd = application.GetWindowHandle();
    while (IsWindow(hwnd))
    {
        // 経過時間を算出
        long nowTime = timeGetTime() << 16;
        long elapsedTime = (nowTime - beforeTime);
        beforeTime = nowTime;

        // --- ゲーム処理 ---

        Update((elapsedTime >> 16), (nowTime >> 16));

        // --- 描画処理 ---

        dxMgr.ClearRenderView();

        Draw(dxMgr.GetDeviceContext());

        fontMgr.Render(dxMgr.GetDeviceContext());
        fontMgr.SetTextFormat(NS_FW_GFX::DefaultFontManager::eID_System, 1, 1, L"FPS:%ld", nFPS);
        //fontMgr.SetTextFormat(NS_FW_GFX::DefaultFontManager::eID_System, 1, 1, L"FPS:%.2f", dFPS);

        dxMgr.Present();

        // --- スリープ処理 ---
        
        nowTime = (timeGetTime() << 16);
        long sleepTime = idleTime - (nowTime - beforeTime) - errorTime;
       
        if (sleepTime < (2 << 16)) sleepTime = (2 << 16); // 最低でも2msは休止
        beforeTime = nowTime;

        //timeBeginPeriod(1); 
        Sleep(sleepTime >> 16);
        //timeEndPeriod(1);
        
        nowTime = (timeGetTime() << 16); 
        errorTime = (nowTime - beforeTime) - sleepTime;

        // --- FPSカウント処理 ---

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

    // アプリケーションの初期化
    NS_FW_SYS::Application& application = NS_FW_SYS::Application::GetInstance();
    if (!application.Create(hInstance, WndProc))
    {
        application.ShowMessageBox(L"エラー", L"アプリケーションの初期化に失敗しました。", NS_FW_SYS::Application::eMB_Error); 
        return 0;
    }

    // DirectX の初期化
    NS_FW_SYS::DX11Manager& dxMgr = NS_FW_SYS::DX11Manager::GetInstance();
    if (!NS_FW_SYS::DX11Manager::GetInstance().Initialize(application.GetWindowHandle(), NS_FW_CONST::WIDTH, NS_FW_CONST::HEIGHT))
    {
        application.ShowMessageBox(L"エラー", L"DirectX の初期化に失敗しました。", NS_FW_SYS::Application::eMB_Error);
        return 0;
    }

    // システム関連初期化
    NS_FW_GFX::DefaultFontManager& fontMgr = NS_FW_GFX::DefaultFontManager::GetInstance();
    for (int i = 0; i < NS_FW_GFX::DefaultFontManager::eID_Num; ++i)
    {
        if (!fontMgr.CreateLayer(dxMgr.GetDevice(), dxMgr.GetDeviceContext(), i, i))
        {
            application.ShowMessageBox(L"エラー", L"システムフォントの初期化に失敗しました。", NS_FW_SYS::Application::eMB_Error);
            return 0;
        }
    }

    // ゲームオブジェクトの作成
    if (!Create(dxMgr.GetDevice(), dxMgr.GetDeviceContext()))
    {
        application.ShowMessageBox(L"エラー", L"ゲームオブジェクトの初期化に失敗しました。", NS_FW_SYS::Application::eMB_Error);
        return 0;
    }

    // スレッドの作成と実行
    s_ThreadParam.exit = false;
    s_ThreadParam.handle = CreateThread(
        NULL,                                     // ハンドルを他のプロセスと共有する場合
        0,                                        // スタックサイズ(デフォルト:0)
        GameMainFunc,                             // スレッド関数名
        reinterpret_cast<LPVOID>(&s_ThreadParam), // スレッドに渡す構造体
        0,                                        // 0: 作成と同時に実行
        &s_ThreadParam.id);                       // スレッドID

    // ウィンドウ表示
    application.ShowWindow(nCmdShow);

    // メッセージ処理
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        DispatchMessage(&msg);
    }

    // 破棄処理の開始
    Destroy();

    fontMgr.Destroy();
    
    dxMgr.Finalize();
    
    application.Destroy();

    return 0;
}

#endif // _WIN64
