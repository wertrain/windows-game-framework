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

    ID3D11Device* GetDevice() { return mDevice; };
    ID3D11DeviceContext* GetDeviceContext() { return mDeviceContext; }
    const ID3D11Device* GetConstDevice() { return mDevice; }
    const ID3D11DeviceContext* GetConstDeviceContext() { return mDeviceContext; }

private:
    ID3D11Device* mDevice;
    ID3D11DeviceContext* mDeviceContext;
    IDXGISwapChain* mDXGISwpChain;
    ID3D11RenderTargetView* mRenderTargetView;
};

} // namespace System 
} // namespace Framework

#endif // DIRECTX_H_
