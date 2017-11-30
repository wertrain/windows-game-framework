#include <windows.h>
#include <time.h>
#include <tchar.h>
#include <mmsystem.h>
#pragma comment (lib, "winmm.lib")

#include <d3d11.h>
#include <atlbase.h>
#include <cstdint>
#include "../Common/Includes.h"
#include "../Core/DirectX.h"
#include "../GameMain.h"
#include "../Constants.h"

#include "Application.h"

namespace Framework {
namespace System {

Application& Application::GetInstance()
{
    static Application instance;
    return instance;
}

Application::Application()
#ifdef _WIN64
    : mHWnd(NULL)
    , mDirectX(nullptr)
#endif // _WIN64
{

}

Application::~Application()
{

}

#ifdef _WIN64
bool Application::Create(HINSTANCE hInstance, WNDPROC wndproc)
{
    WNDCLASSEX winc;

    // �����̏���S��0
    ZeroMemory(&winc, sizeof(WNDCLASSEX));

    winc.cbSize = sizeof(WNDCLASSEX);
    winc.style = CS_HREDRAW | CS_VREDRAW;
    winc.lpfnWndProc = wndproc;
    winc.cbClsExtra = winc.cbWndExtra = 0;
    winc.hInstance = hInstance;
    winc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    winc.hCursor = LoadCursor(NULL, IDC_ARROW);
    winc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    winc.lpszClassName = TEXT("GameFramework");

    if (!RegisterClassEx(&winc)) return 0;

    DWORD dwStyle = WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX; // �E�B���h�E�̑傫���ύX�ł��Ȃ��悤��

    mHWnd = CreateWindowEx(
        0, TEXT("GameFramework"), TEXT("Windows GameFramework"),
        dwStyle,
        CW_USEDEFAULT, CW_USEDEFAULT,
        CW_USEDEFAULT, CW_USEDEFAULT,
        NULL, NULL,
        hInstance, NULL
    );

    if (mHWnd == NULL) return false;

    // COM ���C�u������������
    // �Q�l�ɂ����\�[�X�͌Ă�ł��Ȃ������̂�����
    // ����Ă΂Ȃ��đ��v�ȏ󋵂�����̂��H
    HRESULT hr = ::CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    if (FAILED(hr)) return false;

    // DirectX�̏�����
    mDirectX = new DirectX();
    mDirectX->Initialize(mHWnd, Framework::Constants::WIDTH, Framework::Constants::HEIGHT);

    // �E�B���h�E�̃T�C�Y����
    RECT window_rect;
    SetRect(&window_rect, 0, 0, Framework::Constants::WIDTH, Framework::Constants::HEIGHT);
    AdjustWindowRectEx(&window_rect, GetWindowLong(mHWnd, GWL_STYLE), GetMenu(mHWnd) != NULL, GetWindowLong(mHWnd, GWL_EXSTYLE));
    const int nWidth = window_rect.right - window_rect.left;
    const int nHeight = window_rect.bottom - window_rect.top;
    SetWindowPos(mHWnd, NULL, 0, 0, nWidth, nHeight, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);

    return true;
}

HWND Application::GetWindowHandle()
{
    return mHWnd;
}

DirectX* Application::GetDirectX()
{
    return mDirectX;
}

#endif // _WIN64

void Application::Destroy()
{
#ifdef _WIN64
    if (mDirectX)
    {
        delete mDirectX;
        mDirectX = nullptr;
    }
#endif // _WIN64
}

void Application::ShowWindow(const int nCmdShow)
{
#ifdef _WIN64
    ::ShowWindow(mHWnd, nCmdShow); // �E�B���h�E�\��
#endif // _WIN64
}

} // namespace System
} // namespace Framework
