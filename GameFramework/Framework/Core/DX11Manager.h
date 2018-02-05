#ifndef GRAPHICS_DIRECTX_H_
#define GRAPHICS_DIRECTX_H_

#include "../Utility/Singleton.h"

NS_FW_SYS_BEGIN

/// <summary>
/// DirectX 11 ä«óùÉNÉâÉX
/// </summary>
class DX11Manager : public NS_FW_UTIL::Singleton<DX11Manager>
{
protected:
    friend class NS_FW_UTIL::Singleton<DX11Manager>;
    DX11Manager();

public:
    bool Initialize(const HWND hWnd, const u32 width, const u32 height);
    void Finalize();

    void ClearRenderView();
    void Present();

    ID3D11Device* GetDevice() { return mDevice; };
    ID3D11DeviceContext* GetDeviceContext() { return mDeviceContext; }
    const ID3D11Device* GetConstDevice() { return mDevice; }
    const ID3D11DeviceContext* GetConstDeviceContext() { return mDeviceContext; }

private:
    ID3D11Device* mDevice;
    ID3D11DeviceContext* mDeviceContext;
    IDXGISwapChain* mDXGISwpChain;
    ID3D11RenderTargetView* mRenderTargetView;
    ID3D11Texture2D* mDepthStencil;
    ID3D11DepthStencilView* mDepthStencilView;
};

NS_FW_SYS_END

#endif // GRAPHICS_DIRECTX_H_
