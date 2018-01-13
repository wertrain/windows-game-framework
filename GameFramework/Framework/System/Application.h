#ifndef SYSTEM_APPLICATION_H_
#define SYSTEM_APPLICATION_H_

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
    enum eMBStyle : u16
    {
#ifdef _WIN64
        eMB_Info = MB_ICONINFORMATION,
        eMB_Error = MB_ICONWARNING,
#endif // _WIN64
        eMB_Max
    };

public:
#ifdef _WIN64
    bool Create(HINSTANCE hInstance, WNDPROC wndproc);
    HWND GetWindowHandle();
#endif // _WIN64

    int ShowMessageBox(const wchar_t* title, const wchar_t* message, const eMBStyle style);
    void ShowWindow(const int nCmdShow);
    void Destroy();

private:
#ifdef _WIN64
    HWND mHWnd;
#endif // _WIN64
};

NS_FW_SYS_END

#endif // SYSTEM_APPLICATION_H_
