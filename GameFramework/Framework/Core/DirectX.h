#ifndef DIRECTX_H_
#define DIRECTX_H_

NS_FW_SYS_BEGIN

class DirectX
{
public:
    DirectX() ;
    virtual ~DirectX();

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
};

NS_FW_SYS_END

#endif // DIRECTX_H_
