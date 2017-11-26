#ifndef APPLICATION_H_
#define APPLICATION_H_

namespace Framework {
namespace System {

class DirectX;

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



} // namespace System
} // namespace Framework

#endif // APPLICATION_H_
