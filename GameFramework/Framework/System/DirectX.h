#ifndef DIRECTX_H_
#define DIRECTX_H_

namespace Framework {
namespace System {

class DirectX
{
public:
    DirectX() ;
    virtual ~DirectX();

    bool Initialize(const HWND hWnd);
    void Finalize();

    void ClearRenderView();
    void Present();

private:
    CComPtr<ID3D11DeviceContext> mDeviceContext;
    CComPtr<IDXGISwapChain> mDXGISwpChain;
    CComPtr<ID3D11RenderTargetView> mRenderTargetView;
};

} // namespace System 
} // namespace Framework

#endif // DIRECTX_H_
