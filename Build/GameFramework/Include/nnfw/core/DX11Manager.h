/**
 * @file DX11Manager.h
 * @brief DirectX 11 管理クラス
 */
#ifndef CORE_DIRECTX_H_
#define CORE_DIRECTX_H_

#include <nnfw/ut/Singleton.h>

NS_FW_SYS_BEGIN

/// <summary>
/// DirectX 11 管理クラス
/// </summary>
class DX11Manager : public NS_FW_UTIL::Singleton<DX11Manager>
{
protected:
    friend class NS_FW_UTIL::Singleton<DX11Manager>;
    DX11Manager();

public:
    bool Initialize(const HWND hWnd, const uint32_t width, const uint32_t height);
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

#endif // CORE_DIRECTX_H_
