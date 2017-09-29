#include <d3d11.h>
#include <atlbase.h>
#include <cstdint>
#include "DirectX.h"

#pragma comment(lib, "d3d11.lib")

namespace Framework {
namespace System {

DirectX::DirectX()
{

}

DirectX::~DirectX()
{

}

bool DirectX::Initialize(const HWND hWnd, const uint32_t width, const uint32_t height)
{
    // デバイスの生成
    CComPtr<ID3D11Device> hpDevice; 
    HRESULT hr = D3D11CreateDevice(
        NULL,
        D3D_DRIVER_TYPE_HARDWARE,
        NULL,
        0,
        NULL,
        0,
        D3D11_SDK_VERSION,
        &hpDevice,
        NULL,
        &mDeviceContext);

    if (FAILED(hr))
    {
        return false;
    }


    // インターフェース取得
    CComPtr<IDXGIDevice1> hpDXGI;
    if (FAILED(hpDevice->QueryInterface(__uuidof(IDXGIDevice1), (void**)&hpDXGI)))
    {
        return false;
    }

    // アダプター取得
    CComPtr<IDXGIAdapter> hpAdapter;
    if (FAILED(hpDXGI->GetAdapter(&hpAdapter)))
    {
        return false;
    }

    // ファクトリー取得
    CComPtr<IDXGIFactory> hpDXGIFactory;
    hpAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&hpDXGIFactory);
    if (hpDXGIFactory == NULL)
    {
        return false;
    }

    // ALT+Enterでフルスクリーンを許可する
    if (FAILED(hpDXGIFactory->MakeWindowAssociation(hWnd, 0)))
    {
        return false;
    }

    // スワップチェイン作成
    DXGI_SWAP_CHAIN_DESC hDXGISwapChainDesc;
    hDXGISwapChainDesc.BufferDesc.Width = 1980;
    hDXGISwapChainDesc.BufferDesc.Height = 1080;
    hDXGISwapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
    hDXGISwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    hDXGISwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    hDXGISwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    hDXGISwapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    hDXGISwapChainDesc.SampleDesc.Count = 1;
    hDXGISwapChainDesc.SampleDesc.Quality = 0;
    hDXGISwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    hDXGISwapChainDesc.BufferCount = 1;
    hDXGISwapChainDesc.OutputWindow = hWnd;
    hDXGISwapChainDesc.Windowed = TRUE;
    hDXGISwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    hDXGISwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    if (FAILED(hpDXGIFactory->CreateSwapChain(hpDevice, &hDXGISwapChainDesc, &mDXGISwpChain)))
    {
        return false;
    }

    //そのスワップチェインのバックバッファ取得
    CComPtr<ID3D11Texture2D> hpBackBuffer;
    if (FAILED(mDXGISwpChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&hpBackBuffer)))
    {
        return false;
    }

    // そのバックバッファから描画ターゲット生成    
    if (FAILED(hpDevice->CreateRenderTargetView(hpBackBuffer, NULL, &mRenderTargetView)))
    {
        return false;
    }

    // 更にその描画ターゲットをコンテキストに設定
    mDeviceContext->OMSetRenderTargets(1, &mRenderTargetView, NULL);

    //ビューポート設定
    D3D11_VIEWPORT vp;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    vp.Width = 1920;
    vp.Height = 1080;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    mDeviceContext->RSSetViewports(1, &vp);

    return true;
}

void DirectX::Finalize()
{
    mRenderTargetView.Release();
    mDXGISwpChain.Release();
    mDeviceContext.Release();
}

void DirectX::ClearRenderView()
{
    float ClearColor[] = { 0.0f, 0.0f, 1.0f, 1.0f };
    mDeviceContext->ClearRenderTargetView(mRenderTargetView, ClearColor);
}

void DirectX::Present()
{
    mDXGISwpChain->Present(0, 0);
}

} // namespace Graphics
} // namespace Framework