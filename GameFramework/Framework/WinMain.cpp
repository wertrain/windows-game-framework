#include <windows.h>
#include <time.h>
#include <tchar.h>
#include <mmsystem.h>
#pragma comment (lib, "winmm.lib")

#include "GameMain.h"
#include "Constants.h"

namespace Framework {

static bool s_ExitThread = false;

// ゲームウィンドウ構造体
typedef struct GameWindow
{
    HWND hWnd;      // ウインドウ
    HDC hScreenDC;  // バックバッファ
    SIZE size;      // サイズ
    DWORD dwFps;    // FPS
} GameWindow;

DWORD WINAPI GameMainFunc(LPVOID vdParam)
{
    GameWindow *gameWindow = static_cast<GameWindow*>(vdParam);

    //フレーム数と以前の時間
    DWORD frames = 0, beforeTime;
    //FPSの表示用
    TCHAR str[16] = {0};
    // キーボードの状態を格納
    //BYTE keyTable[256];

    // デバイスコンテキスト
    HDC hdc;
    // 描画構造体
    PAINTSTRUCT ps;

    // 乱数ジェネレータ初期化
    srand(static_cast<unsigned>(time(NULL)));
    
    // 初期時間の取得
    beforeTime = timeGetTime();

    // ゲームループ
    while (IsWindow(gameWindow->hWnd))
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

        Draw(gameWindow->hScreenDC);

        // 理想時間の算出
        DWORD idealTime = (DWORD)(frames * (1000.0 / gameWindow->dwFps));
        if (idealTime > progress)
        {
            Sleep(idealTime - progress);
        }

        // FPSの表示位置
        TextOut(gameWindow->hScreenDC, 10, 10, str, lstrlen(str));

        // スレッド内で作成された画像を描画
        hdc = BeginPaint(gameWindow->hWnd, &ps);
        BitBlt(hdc, 0, 0,
            gameWindow->size.cx,
            gameWindow->size.cy,
            gameWindow->hScreenDC,
            0, 0,
            SRCCOPY);
        PatBlt(gameWindow->hScreenDC, 0, 0, gameWindow->size.cx,gameWindow->size.cy, BLACKNESS); // 裏画面を黒で初期化
        EndPaint(gameWindow->hWnd, &ps);

        // ウインドウの再描画
        InvalidateRect(gameWindow->hWnd, NULL, FALSE); // TRUE:画面初期化

        if (progress >= 1000)
        {
            wsprintf(str, TEXT("FPS %d"), frames);
            beforeTime = nowTime;
            frames = 0;
        }
        else
        {
            ++frames;
        }

        if (Framework::s_ExitThread) break;
    }

    DeleteDC(gameWindow->hScreenDC);

    return TRUE;
}

} // namespace Framework 

LRESULT CALLBACK WndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    // デバイスコンテキスト
    HDC hdc;
    // 描画構造体
    //PAINTSTRUCT ps;

    // ゲームウィンドウ
    static Framework::GameWindow gameWindow;
    // BITMAPハンドル
    static HBITMAP hBitmap;
    // スレッドID
    DWORD dwID;
    // スレッドハンドル
    static HANDLE hThread;
    DWORD dwParam;

    switch(msg)
    {
        case WM_DESTROY:
            // スレッドを終了させる
            Framework::s_ExitThread = true;
            GetExitCodeThread(hThread, &dwParam);
            WaitForSingleObject(hThread, INFINITE);
            Destroy();
            PostQuitMessage(0);
            return 0;

        case WM_CREATE:

            // GameWindow構造体の設定
            gameWindow.hWnd = hWnd;
            gameWindow.size.cx = Framework::Constants::WIDTH;
            gameWindow.size.cy = Framework::Constants::HEIGHT;
            gameWindow.dwFps = Framework::Constants::FPS;

            // HDCの作成
            hdc = GetDC(hWnd);
            hBitmap = CreateCompatibleBitmap(hdc, gameWindow.size.cx, gameWindow.size.cy);
            gameWindow.hScreenDC = CreateCompatibleDC(hdc);
            SelectObject(gameWindow.hScreenDC, hBitmap);

            // HDCの解放
            ReleaseDC(hWnd, hdc);

            // ゲームオブジェクトの作成
            if (!Create(gameWindow.hWnd))
            {
                //assert( !"ゲームオブジェクトの作成に失敗しました。" );
                return 0;
            }

            //スレッドの作成と実行
            hThread = CreateThread(NULL,          // ハンドルを他のプロセスと共有する場合
                0,                      // スタックサイズ(デフォルト:0)
                Framework::GameMainFunc,// スレッド関数名
                (LPVOID)&gameWindow,    // スレッドに渡す構造体
                0,                      // 0:作成と同時に実行
                &dwID);                 // スレッドID
        
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
        0, TEXT("GameFramework") , TEXT("Windows Native GameFramework") ,
        dwStyle,
        CW_USEDEFAULT, CW_USEDEFAULT ,
        CW_USEDEFAULT, CW_USEDEFAULT ,
        NULL, NULL ,
        hInstance, NULL
    );

    if (hwnd == NULL) return 0;

    // ウィンドウのサイズ調整
    RECT window_rect;
    SetRect(&window_rect, 0, 0, Framework::Constants::WIDTH, Framework::Constants::HEIGHT);
    AdjustWindowRectEx(&window_rect, GetWindowLong(hwnd,GWL_STYLE), GetMenu(hwnd) != NULL, GetWindowLong(hwnd,GWL_EXSTYLE));
    const int nWidth  = window_rect.right  - window_rect.left;
    const int nHeight = window_rect.bottom - window_rect.top;
    SetWindowPos(hwnd, NULL, 0, 0, nWidth, nHeight, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);

    ShowWindow(hwnd, nCmdShow); // ウィンドウ表示

    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        DispatchMessage(&msg);
    }

    return 0;
}
