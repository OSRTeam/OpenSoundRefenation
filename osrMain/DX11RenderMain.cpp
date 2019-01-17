/*********************************************************
* Copyright (C) VERTVER, 2018. All rights reserved.
* OpenSoundRefenation - WINAPI open-source DAW
* MIT-License
**********************************************************
* Module Name: OSR User Interface
**********************************************************
* DX11RenderMain.cpp
* Direct3D11 main implementation
*********************************************************/
#include "stdafx.h"
#include <d3dcompiler.h>

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

OSRCODE
DX11Render::CreateRenderWindow(HWND WindowHandle)
{
	HRESULT hr = NULL;
	DXGI_SWAP_CHAIN_DESC &sd = m_pChainDesc;
	DXGI_ADAPTER_DESC LegacyDesc = {};
	DXGI_ADAPTER_DESC1 Desc = {}; 

	if (!WindowHandle) { return DX_OSR_BAD_ARGUMENT; }
	MainHwnd = WindowHandle;

	// if our PC doesn't support DXGIFactor1 - try to use older
	if (FAILED(CreateDXGIFactory1(IID_PPV_ARGS(&m_pDXGIFactory))))
	{
		FAILEDX2(CreateDXGIFactory(IID_PPV_ARGS(&m_pLegacyDXGIFactory)));
	}

	if (!m_pDXGIFactory)
	{
		if (!m_pLegacyDXGIFactory) 
		{ 
			return DX_OSR_BAD_HW;
		}
		else
		{
			FAILEDX2(m_pLegacyDXGIFactory->QueryInterface(IID_PPV_ARGS(&m_pDXGIFactory)));
		}
	}

	//#TODO: take set adapters for user
	if (m_pLegacyDXGIFactory)
	{
		m_pDXGIFactory->EnumAdapters(NULL, &m_pLegacyDXGIAdapter);
		m_pLegacyDXGIAdapter->QueryInterface(IID_PPV_ARGS(&m_pDXGIAdapter));

		_RELEASE(m_pLegacyDXGIAdapter);
		_RELEASE(m_pLegacyDXGIFactory);

		m_pDXGIAdapter->GetDesc(&LegacyDesc);
	}
	else
	{
		m_pDXGIFactory->EnumAdapters1(NULL, &m_pDXGIAdapter);
		m_pDXGIAdapter->GetDesc1(&Desc);
	}

	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferCount = 2;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	sd.OutputWindow = MainHwnd;
	sd.Windowed = TRUE;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	D3D_FEATURE_LEVEL pFeatureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0
	};

	D3D_FEATURE_LEVEL FeatureLevel = {};

	hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, 
		NULL, NULL, pFeatureLevels, sizeof(pFeatureLevels) / sizeof(pFeatureLevels[0]),
		D3D11_SDK_VERSION, &sd, &m_pDXGISwapChain, &m_pDevice, &FeatureLevel, &m_pContext
	);

	if (FAILED(hr))
	{
		FAILEDX2( //-V568
			D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE,
			NULL, NULL, &pFeatureLevels[1], sizeof(pFeatureLevels) / sizeof(pFeatureLevels[0] - 1),
			D3D11_SDK_VERSION, &sd, &m_pDXGISwapChain, &m_pDevice, &FeatureLevel, &m_pContext
		));
	}	

	ID3D11Texture2D* pBackBuffer = nullptr;
	m_pDXGISwapChain->GetBuffer(NULL, IID_PPV_ARGS(&pBackBuffer));
	m_pDevice->CreateRenderTargetView(pBackBuffer, NULL, &m_pRenderTargetView);
	_RELEASE(pBackBuffer);

	return OSR_SUCCESS;
}

OSRCODE
DX11Render::LoadShader(
	LPCWSTR lpShaderName, 
	LPCSTR* lpShaderFile
)
{
	DWORD dwShaderWritten = 0;
	DWORD dwShaderSize = 0;
	LPCSTR lpShader = nullptr;
	LPWSTR StaticBuf = nullptr;
	WSTRING_PATH szPathToDir = { 0 };

	GetApplicationDirectory(&StaticBuf);
	memcpy(szPathToDir, StaticBuf, 520);

	_snwprintf_s(szPathToDir, sizeof(WSTRING_PATH), L"%s%s", szPathToDir, L"\\Shaders"); //-V575

	DWORD dwGetDir = GetFileAttributesW(szPathToDir);
	if (dwGetDir == INVALID_FILE_ATTRIBUTES || !(dwGetDir & FILE_ATTRIBUTE_DIRECTORY))
	{
		if (!CreateDirectoryW(szPathToDir, nullptr))
		{
			DWORD dwError = GetLastError();
			if (!IsProcessWithAdminPrivilege() && dwError == ERROR_ACCESS_DENIED)
			{
				if (THROW4(L"Can't create temp directory because access denied. Re-run application with 'Administrator' privilege?"))
				{
					RunWithAdminPrivilege();
				}
			}
			else
			{
				THROW1(L"Can't create temp directory");
			}
		}
	}  
	else
	{
		_snwprintf_s(szPathToDir, sizeof(WSTRING_PATH), L"%s%s%s", szPathToDir, L"\\", lpShaderName); //-V575

		HANDLE hFile = CreateFileW(szPathToDir, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (!hFile || hFile == INVALID_HANDLE_VALUE) { return FS_OSR_BAD_HANDLE; }

		FILE_STANDARD_INFO fileInfo = {};
		GetFileInformationByHandleEx(hFile, FileStandardInfo, &fileInfo, sizeof(fileInfo));

		dwShaderSize = fileInfo.EndOfFile.LowPart;
		lpShader = (LPCSTR)FastAlloc(dwShaderSize);

		ReadFile(hFile, (LPVOID)lpShader, dwShaderSize, &dwShaderWritten, nullptr);

		CloseHandle(hFile);
	}

	if (!lpShader) { return FS_OSR_BAD_PATH; }
	*lpShaderFile = lpShader;
	
	return OSR_SUCCESS;
}

OSRCODE
DX11Render::BeginRenderBlur(
	ID3D11DeviceContext* pCustomDeviceContext, 
	DWORD Height,
	DWORD Width
)
{
	if (Width > 10000 || !Width) Width = 1;
	if (Height > 10000 || !Height) Height = 1;

	static D3D11_TEXTURE2D_DESC textureDesc = { };
	bool RestartRenderTargets = false;

	if (textureDesc.Width != Width || textureDesc.Height != Height)
	{
		RestartRenderTargets = true;
	}

	textureDesc.ArraySize = 1;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.Height = Height;
	textureDesc.MipLevels = 1;
	textureDesc.MiscFlags = 0;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.Width = Width;

	static D3D11_SHADER_RESOURCE_VIEW_DESC textureDescSR = { };
	textureDescSR.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDescSR.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	textureDescSR.Texture2D.MipLevels = 1;
	textureDescSR.Texture2D.MostDetailedMip = 0;

	if (RestartRenderTargets)
	{
		_RELEASE(pTargetView1);
		_RELEASE(pTargetView2);
		_RELEASE(pTargetView3);

		_RELEASE(pResourceView1);
		_RELEASE(pResourceView2);
		_RELEASE(pResourceView3);

		_RELEASE(pTexture1);
		_RELEASE(pTexture2);
		_RELEASE(pTexture3);

		FAILEDX2(m_pDevice->CreateTexture2D(&textureDesc, nullptr, &pTexture1));
		FAILEDX2(m_pDevice->CreateTexture2D(&textureDesc, nullptr, &pTexture2));
		FAILEDX2(m_pDevice->CreateTexture2D(&textureDesc, nullptr, &pTexture3));

		FAILEDX2(m_pDevice->CreateRenderTargetView(pTexture1, nullptr, &pTargetView1));
		FAILEDX2(m_pDevice->CreateRenderTargetView(pTexture2, nullptr, &pTargetView2));
		FAILEDX2(m_pDevice->CreateRenderTargetView(pTexture3, nullptr, &pTargetView3));

		FAILEDX2(m_pDevice->CreateShaderResourceView(pTexture1, &textureDescSR, &pResourceView1));
		FAILEDX2(m_pDevice->CreateShaderResourceView(pTexture2, &textureDescSR, &pResourceView2));
		FAILEDX2(m_pDevice->CreateShaderResourceView(pTexture3, &textureDescSR, &pResourceView3));
	}

	static LPCSTR lpShader = nullptr;

	if (!lpShader)
	{
		OSRFAIL2(LoadShader(L"MBlur.hlsl", &lpShader), L"Can't load shader");
	}

	if (!vertexShaderBlur)
	{
		FAILEDX2(D3DCompile(lpShader, strlen(lpShader), nullptr, nullptr, nullptr, "VSSimpleBlur", "vs_4_0", 0, 0, &vertexShaderBlur, nullptr));
		FAILEDX2(m_pDevice->CreateVertexShader(vertexShaderBlur->GetBufferPointer(), vertexShaderBlur->GetBufferSize(), nullptr, &pBlurVertexShader));
	}
	
	if (!pBlurPixelShaderU)
	{
		FAILEDX2(D3DCompile(lpShader, strlen(lpShader), nullptr, nullptr, nullptr, "PSSimpleBlurU", "ps_4_0", 0, 0, &pixelShaderBlurU, nullptr));
		FAILEDX2(m_pDevice->CreatePixelShader(pixelShaderBlurU->GetBufferPointer(), pixelShaderBlurU->GetBufferSize(), nullptr, &pBlurPixelShaderU));
	}

	if (!pBlurPixelShaderV)
	{
		FAILEDX2(D3DCompile(lpShader, strlen(lpShader), nullptr, nullptr, nullptr, "PSSimpleBlurV", "ps_4_0", 0, 0, &pixelShaderBlurV, nullptr));
		FAILEDX2(m_pDevice->CreatePixelShader(pixelShaderBlurV->GetBufferPointer(), pixelShaderBlurV->GetBufferSize(), nullptr, &pBlurPixelShaderV));
	}

	if (!m_pDisplaySizeBuffer)
	{
		D3D11_BUFFER_DESC pDesc = {};
		pDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		pDesc.ByteWidth = sizeof(float) * 4;
		pDesc.CPUAccessFlags = 0;// D3D11_CPU_ACCESS_WRITE;
		pDesc.MiscFlags = 0;
		pDesc.StructureByteStride = 0;
		pDesc.Usage = D3D11_USAGE_DEFAULT;// DYNAMIC;
		FAILEDX2(m_pDevice->CreateBuffer(&pDesc, nullptr, &m_pDisplaySizeBuffer));
	}

	pCustomDeviceContext->OMSetRenderTargets(1, &pTargetView1, nullptr);
	float ffgfdgfdg[4] = { 0.07f, 0.07f, 0.07f, 1.00f };
	pCustomDeviceContext->ClearRenderTargetView(pTargetView1, ffgfdgfdg);

	return OSR_SUCCESS;
}

OSRCODE 
DX11Render::EndRenderBlur(
	ID3D11DeviceContext* pCustomDeviceContext,
	DWORD Height, 
	DWORD Width
)
{
	ID3D11Buffer* Buf = nullptr;
	UINT ba1 = 0;
	UINT ba2 = 0;

	// stage 1
	D3D11_VIEWPORT vp;
	memset(&vp, 0, sizeof(D3D11_VIEWPORT));
	vp.Width = Width;
	vp.Height = Height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = vp.TopLeftY = 0;
	pCustomDeviceContext->RSSetViewports(1, &vp);

	const D3D11_RECT r = { 0, 0, Width, Height };
	pCustomDeviceContext->RSSetScissorRects(1, &r);

	float DisplaySize[4] = { Width, Height , 0.0, 0.0};
	pCustomDeviceContext->UpdateSubresource(m_pDisplaySizeBuffer, 0, nullptr, &DisplaySize[0], 0, 0);
	
	if (m_pDisplaySizeBuffer)
	{
		pCustomDeviceContext->PSSetConstantBuffers(5, 1, &m_pDisplaySizeBuffer);
	}

	pCustomDeviceContext->VSSetShader(pBlurVertexShader, nullptr, 0);

	pCustomDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pCustomDeviceContext->IASetVertexBuffers(0, 1, &Buf, &ba1, &ba2);
	pCustomDeviceContext->IASetIndexBuffer(nullptr, DXGI_FORMAT_R16_UINT, 0);
	pCustomDeviceContext->IASetInputLayout(nullptr);

	pCustomDeviceContext->OMSetRenderTargets(1, &pTargetView2, nullptr);
	pCustomDeviceContext->PSSetShaderResources(0, 1, &pResourceView1);
	pCustomDeviceContext->PSSetShader(pBlurPixelShaderU, nullptr, 0);
	pCustomDeviceContext->Draw(3, 0);

	// stage 2
	pCustomDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, nullptr);
	pCustomDeviceContext->PSSetShaderResources(0, 1, &pResourceView2);
	pCustomDeviceContext->PSSetShader(pBlurPixelShaderV, nullptr, 0);
	pCustomDeviceContext->Draw(3, 0);

	return OSR_SUCCESS;
}