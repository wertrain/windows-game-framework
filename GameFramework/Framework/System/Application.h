#ifndef APPLICATION_H_
#define APPLICATION_H_

NS_FW_SYS_BEGIN

class DirectX;

/// <summary>
/// アプリケーション管理クラス
/// </summary>
class Application
{
public:
    static Application& GetInstance();

private:
    Application();
    ~Application();

public:
#ifdef _WIN64
    bool Create(HINSTANCE hInstance, WNDPROC wndproc);
    HWND GetWindowHandle();
    DirectX* GetDirectX();
#endif // _WIN64

    void ShowWindow(const int nCmdShow);
    void Destroy();

private:
#ifdef _WIN64
    HWND mHWnd;
    DirectX* mDirectX;
#endif // _WIN64
};

NS_FW_SYS_END

#endif // APPLICATION_H_
