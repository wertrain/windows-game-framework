/**
 * @file DX11Manager.cpp
 * @brief DirectX 11 管理クラス
 */
#include <d3d11.h>
#include <atlbase.h>
#include "../Common/Includes.h"
#include "DX11Manager.h"

#pragma comment(lib, "d3d11.lib")

NS_FW_SYS_BEGIN

DX11Manager::DX11Manager()
    : mDevice(nullptr)
    , mDeviceContext(nullptr)
    , mDXGISwpChain(nullptr)
    , mRenderTargetView(nullptr)
    , mDepthStencil(nullptr)
    , mDepthStencilView(nullptr)
{

}

bool DX11Manager::Initialize(const HWND hWnd, const u32 width, const u32 height)
{
    // デバイスの生成
    HRESULT hr = D3D11CreateDevice(
        NULL,
        D3D_DRIVER_TYPE_HARDWARE,
        NULL,
        0,
        NULL,
        0,
        D3D11_SDK_VERSION,
        &mDevice,
        NULL,
        &mDeviceContext);

    if (FAILED(hr))
    {
        return false;
    }


    // インターフェース取得
    CComPtr<IDXGIDevice1> hpDXGI;
    if (FAILED(mDevice->QueryInterface(__uuidof(IDXGIDevice1), (void**)&hpDXGI)))
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
    hDXGISwapChainDesc.BufferDesc.Width = width;
    hDXGISwapChainDesc.BufferDesc.Height = height;
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
    if (FAILED(hpDXGIFactory->CreateSwapChain(mDevice, &hDXGISwapChainDesc, &mDXGISwpChain)))
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
    if (FAILED(mDevice->CreateRenderTargetView(hpBackBuffer, NULL, &mRenderTargetView)))
    {
        return false;
    }

    // Create depth stencil texture
    D3D11_TEXTURE2D_DESC descDepth;
    ZeroMemory(&descDepth, sizeof(descDepth));
    descDepth.Width = width;
    descDepth.Height = height;
    descDepth.MipLevels = 1;
    descDepth.ArraySize = 1;
    descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    descDepth.SampleDesc.Count = 1;
    descDepth.SampleDesc.Quality = 0;
    descDepth.Usage = D3D11_USAGE_DEFAULT;
    descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    descDepth.CPUAccessFlags = 0;
    descDepth.MiscFlags = 0;
    hr = mDevice->CreateTexture2D(&descDepth, NULL, &mDepthStencil);
    if (FAILED(hr))
    {
        return false;
    }

    // Create the depth stencil view
    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
    ZeroMemory(&descDSV, sizeof(descDSV));
    descDSV.Format = descDepth.Format;
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0;
    hr = mDevice->CreateDepthStencilView(mDepthStencil, &descDSV, &mDepthStencilView);
    if (FAILED(hr))
    {
        return false;
    }

    // 更にその描画ターゲットをコンテキストに設定
    mDeviceContext->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);

    //ビューポート設定
    D3D11_VIEWPORT vp;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    vp.Width = static_cast<float>(width);
    vp.Height = static_cast<float>(height);
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    mDeviceContext->RSSetViewports(1, &vp);

    return true;
}

void DX11Manager::Finalize()
{
    if (mRenderTargetView)
    {
        mRenderTargetView->Release();
        mRenderTargetView = nullptr;
    }
    if (mDepthStencilView)
    {
        mDepthStencilView->Release();
        mDepthStencilView = nullptr;
    }
    if (mDepthStencil)
    {
        mDepthStencil->Release();
        mDepthStencil = nullptr;
    }
    if (mDXGISwpChain)
    {
        mDXGISwpChain->Release();
        mDXGISwpChain = nullptr;
    }
    if (mDeviceContext)
    {
        mDeviceContext->Release();
        mDeviceContext = nullptr;
    }
    if (mDevice)
    {
        mDevice->Release();
        mDevice = nullptr;
    }
}

void DX11Manager::ClearRenderView()
{
    float ClearColor[] = { 0.0f, 0.0f, 1.0f, 1.0f };
    mDeviceContext->ClearRenderTargetView(mRenderTargetView, ClearColor);

    mDeviceContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void DX11Manager::Present()
{
    mDXGISwpChain->Present(0, 0);
}

NS_FW_SYS_END
