#ifndef DIRECTX_H_
#define DIRECTX_H_

namespace Framework {
namespace System {

class DirectX
{
public:
    DirectX() ;
    virtual ~DirectX();

    bool Initialize(const HWND hWnd, const uint32_t width, const uint32_t height);
    void Finalize();

    void ClearRenderView();
    void Present();

    ID3D11Device* GetDevice() { return mDevice.p; }
    ID3D11DeviceContext* GetDeviceContext() { return mDeviceContext.p; }

private:
    CComPtr<ID3D11Device> mDevice;
    CComPtr<ID3D11DeviceContext> mDeviceContext;
    CComPtr<IDXGISwapChain> mDXGISwpChain;
    CComPtr<ID3D11RenderTargetView> mRenderTargetView;
};

} // namespace System 
} // namespace Framework

#endif // DIRECTX_H_
