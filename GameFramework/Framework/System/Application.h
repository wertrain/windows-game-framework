#ifndef APPLICATION_H_
#define APPLICATION_H_

#include "../Utility/Singleton.h"

NS_FW_SYS_BEGIN

/// <summary>
/// アプリケーション管理クラス
/// </summary>
class Application : public NS_FW_UTIL::Singleton<Application>
{
protected:
    friend class NS_FW_UTIL::Singleton<Application>;
    Application();

public:
#ifdef _WIN64
    bool Create(HINSTANCE hInstance, WNDPROC wndproc);
    HWND GetWindowHandle();
#endif // _WIN64

    void ShowWindow(const int nCmdShow);
    void Destroy();

private:
#ifdef _WIN64
    HWND mHWnd;
#endif // _WIN64
};

NS_FW_SYS_END

#endif // APPLICATION_H_
