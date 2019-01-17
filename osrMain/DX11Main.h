/*********************************************************
* Copyright (C) VERTVER, 2018. All rights reserved.
* OpenSoundRefenation - WINAPI open-source DAW
* MIT-License
**********************************************************
* Module Name: OSR DirectX
**********************************************************
* DX11Maim.h
* Direct3D11 main implementation
*********************************************************/
#pragma once
#include <d3d11.h>
#include "dxgi.h"
#include "OSR.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

struct VERTEX_CONSTANT_BUFFER
{
	float   mvp[4][4];
};

class DX11Render
{
public:
	HWND GetCurrentHwnd() { return MainHwnd; }
	OSRCODE CreateRenderWindow(HWND WindowHandle);
	OSRCODE LoadShader(LPCWSTR lpShaderName, LPCSTR* lpShaderFile);
	OSRCODE BeginRenderBlur(ID3D11DeviceContext* pCustomDeviceContext, DWORD Height, DWORD Width);
	OSRCODE EndRenderBlur(ID3D11DeviceContext* pCustomDeviceContext, DWORD Height, DWORD Width);

	ID3D11BlendState* pBlendState = nullptr;
	ID3D11RasterizerState* pRasterizerState = nullptr;
	ID3D11DepthStencilState* pDepthStencilState = nullptr;

	DXGI_SWAP_CHAIN_DESC m_pChainDesc = { NULL };
	IDXGIFactory1* m_pDXGIFactory = nullptr;
	IDXGIFactory* m_pLegacyDXGIFactory = nullptr;
	IDXGIAdapter1* m_pDXGIAdapter = nullptr;
	IDXGIAdapter* m_pLegacyDXGIAdapter = nullptr;
	IDXGISwapChain* m_pDXGISwapChain = nullptr;
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pContext = nullptr;
	ID3D11RenderTargetView* m_pRenderTargetView = nullptr;
	ID3D11Buffer*	m_pDisplaySizeBuffer = nullptr;
private:
	ID3D11Texture2D* pTexture1 = nullptr;
	ID3D11Texture2D* pTexture2 = nullptr;
	ID3D11Texture2D* pTexture3 = nullptr;

	ID3D11RenderTargetView* pTargetView1 = nullptr;
	ID3D11RenderTargetView* pTargetView2 = nullptr;
	ID3D11RenderTargetView* pTargetView3 = nullptr;

	ID3D11ShaderResourceView* pResourceView1 = nullptr;
	ID3D11ShaderResourceView* pResourceView2 = nullptr;
	ID3D11ShaderResourceView* pResourceView3 = nullptr;

	ID3DBlob* vertexShader = nullptr;
	ID3DBlob* pixelShader = nullptr;
	ID3DBlob* vertexShaderBlur = nullptr;
	ID3DBlob* pixelShaderBlurU = nullptr;
	ID3DBlob* pixelShaderBlurV = nullptr;

	ID3D11VertexShader* pBlurVertexShader = nullptr;
	ID3D11PixelShader* pBlurPixelShaderU = nullptr;
	ID3D11PixelShader* pBlurPixelShaderV = nullptr;

private:
	HWND MainHwnd;
};