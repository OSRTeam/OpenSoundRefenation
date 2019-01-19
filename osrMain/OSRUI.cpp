/*********************************************************
* Copyright (C) VERTVER, 2018. All rights reserved.
* OpenSoundRefenation - WINAPI open-source DAW
* MIT-License
**********************************************************
* Module Name: OSR DirectX
**********************************************************
* osrUI.cpp
* Main User Interface implementation
*********************************************************/
#include "stdafx.h"
#include "VUMeter.h"
#include "../resource1.h"
#include "DiscordPart.h"
#include "DragAndDrop.h"
#pragma comment(lib, "Winmm.lib")

DX11Render dx11Renderer;
VUMeter vMeter;
bool Window_Flag_Resizeing = false;
DLL_API bool IsBlur = false;
DLL_API bool IsLoad = false;
DLL_API float ProgressBartest = 0.0;
DiscordNetwork disc = {};

DropTarget dropTarget;

VOID 
ImDrawCallbackPostBlur(
	const ImDrawList* parent_list,
	const ImDrawCmd* cmd
) 
{
	ID3D11DeviceContext* pCustomDeviceContext = dx11Renderer.m_pContext;
	ImGuiIO* mainio = (ImGuiIO*)cmd->UserCallbackData;

	if (IsBlur && mainio) 
	{
		struct BACKUP_DX11_STATE
		{
			UINT                        ScissorRectsCount, ViewportsCount;
			D3D11_RECT                  ScissorRects[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
			D3D11_VIEWPORT              Viewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
			ID3D11RasterizerState*      RS;
			ID3D11BlendState*           BlendState;
			FLOAT                       BlendFactor[4];
			UINT                        SampleMask;
			UINT                        StencilRef;
			ID3D11DepthStencilState*    DepthStencilState;
			ID3D11ShaderResourceView*   PSShaderResource;
			ID3D11SamplerState*         PSSampler;
			ID3D11PixelShader*          PS;
			ID3D11VertexShader*         VS;
			UINT                        PSInstancesCount, VSInstancesCount;
			ID3D11ClassInstance*        PSInstances[256], *VSInstances[256];   // 256 is max according to PSSetShader documentation
			D3D11_PRIMITIVE_TOPOLOGY    PrimitiveTopology;
			ID3D11Buffer*               IndexBuffer, *VertexBuffer, *VSConstantBuffer;
			UINT                        IndexBufferOffset, VertexBufferStride, VertexBufferOffset;
			DXGI_FORMAT                 IndexBufferFormat;
			ID3D11InputLayout*          InputLayout;
		};

		BACKUP_DX11_STATE old = {};
		old.ScissorRectsCount = old.ViewportsCount = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
		pCustomDeviceContext->RSGetScissorRects(&old.ScissorRectsCount, old.ScissorRects);
		pCustomDeviceContext->RSGetViewports(&old.ViewportsCount, old.Viewports);
		pCustomDeviceContext->PSGetShaderResources(0, 1, &old.PSShaderResource);

		old.PSInstancesCount = old.VSInstancesCount = 256;

		pCustomDeviceContext->PSGetShader(&old.PS, old.PSInstances, &old.PSInstancesCount);
		pCustomDeviceContext->VSGetShader(&old.VS, old.VSInstances, &old.VSInstancesCount);
		pCustomDeviceContext->IAGetPrimitiveTopology(&old.PrimitiveTopology);
		pCustomDeviceContext->IAGetIndexBuffer(&old.IndexBuffer, &old.IndexBufferFormat, &old.IndexBufferOffset);
		pCustomDeviceContext->IAGetVertexBuffers(0, 1, &old.VertexBuffer, &old.VertexBufferStride, &old.VertexBufferOffset);
		pCustomDeviceContext->IAGetInputLayout(&old.InputLayout);

		dx11Renderer.EndRenderBlur(dx11Renderer.m_pContext, mainio->DisplaySize.y, mainio->DisplaySize.x);

		// Restore modified DX state
		pCustomDeviceContext->RSSetScissorRects(old.ScissorRectsCount, old.ScissorRects);
		pCustomDeviceContext->RSSetViewports(old.ViewportsCount, old.Viewports);
		pCustomDeviceContext->PSSetShaderResources(0, 1, &old.PSShaderResource);

		_RELEASE(old.PSShaderResource);

		pCustomDeviceContext->PSSetShader(old.PS, old.PSInstances, old.PSInstancesCount);

		_RELEASE(old.PS);

		for (UINT i = 0; i < old.PSInstancesCount; i++)
		{
			_RELEASE(old.PSInstances[i]);
		}

		pCustomDeviceContext->VSSetShader(old.VS, old.VSInstances, old.VSInstancesCount); 

		_RELEASE(old.VS);

		for (UINT i = 0; i < old.VSInstancesCount; i++)
		{
			_RELEASE(old.VSInstances[i]);
		}

		pCustomDeviceContext->IASetPrimitiveTopology(old.PrimitiveTopology);
		pCustomDeviceContext->IASetIndexBuffer(old.IndexBuffer, old.IndexBufferFormat, old.IndexBufferOffset);  
		_RELEASE(old.IndexBuffer);
		pCustomDeviceContext->IASetVertexBuffers(0, 1, &old.VertexBuffer, &old.VertexBufferStride, &old.VertexBufferOffset); 
		_RELEASE(old.VertexBuffer);
		pCustomDeviceContext->IASetInputLayout(old.InputLayout); 
		_RELEASE(old.InputLayout);
	}
}

VOID
CycleFunc()
{
	bool bDemo = true;
	static float mnmm = 0;
	static ImVec4 clear_color = ImVec4(0.07f, 0.07f, 0.07f, 1.00f);
	float L = 1.0*abs(sin(mnmm) *sin(mnmm*0.1)); //
	float R = 0.8*abs(cos(mnmm + 1.0)*sin(mnmm*0.1));
	static float LS = 0.0f;
	static float RS = 0.0f;
	LS = max(LS, L);
	RS = max(RS, R);
	static bool peakdetectL = false;
	if (L > 0.98f) peakdetectL = true;

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	static bool FirstStart = true;
	if (FirstStart) {
		ImGui::SetNextWindowSize(ImVec2(100, 500)); FirstStart = false;
	}

	if (ImGui::Begin("VU Meter", nullptr))
	{
		ImVec2 RegionAvail = ImGui::GetContentRegionAvail();
		vMeter.DrawLevels(RegionAvail.x - 30, RegionAvail.y - 50.0, L, R, LS, RS, peakdetectL, false);
	}
	ImGui::End();
	

	mnmm += 0.1;
	LS *= 0.995f;
	RS *= 0.995f;

	IOSRMixer* pMixer = (IOSRMixer*)GetWindowLongPtrW(dx11Renderer.GetCurrentHwnd(), -21);

	if (bDemo)
	{
		static float f = 0.0f;
		static bool isPlay = false;
		static bool bClose = false;
		int counter = 0;

		ImGui::Begin("Test Window");

		ImGui::Text("File dialog");

		if (ImGui::SliderFloat("Track Position", &f, 0.0f, 1.0f))
		{
			pMixer->SetPosition(f);
		}

		ImGui::ColorEdit3("Clear color", (float*)&clear_color);

		if (ImGui::Button("Play Audio"))
		{
			if (!isPlay)
			{
				pMixer->CreateMixer(100);
				pMixer->Play();
			}
			else
			{
				pMixer->RestartMixer(100);
			}
		}

		if (ImGui::Button("Stop Audio"))
		{
			if (isPlay)
			{
				pMixer->Stop();
				pMixer->DestroyMixer();
			}
		}

		if (ImGui::Button("Blur On/Off"))
		{
			IsLoad = !IsLoad;
		}

		if (ImGui::Button("Open/Close Plugin window"))
		{
			static bool isOpened = false;

			if (!isOpened)
			{
				pMixer->OpenPlugin(1, 0);	//#TODO:
			}
			else
			{
				pMixer->ClosePlugin(1, 0);
			}
		}

		ImGui::SameLine();
		ImGui::Text("counter = %d", counter);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
	}
	

	ImGuiIO& iomain = ImGui::GetIO();
	static ImVec2 LoadMaxContext = ImVec2(0, 0);
	ImGui::SetNextWindowPos(ImVec2(iomain.DisplaySize.x / 2 - LoadMaxContext.x/2, iomain.DisplaySize.y / 2 - LoadMaxContext.y/2));
	
	if (IsLoad)
	{
		IsBlur = IsLoad;
		ImGui::BeginTooltip();

		ImDrawList* BlurDC2 = ImGui::GetWindowDrawList();
		BlurDC2->AddCallback(ImDrawCallbackPostBlur, (void*)&iomain);

		ImGui::ProgressBar(ProgressBartest, ImVec2(iomain.DisplaySize.x * 0.4, iomain.DisplaySize.y * 0.1), "");
		LoadMaxContext = ImGui::GetContentRegionMax();
		ImGui::EndTooltip();
	}
	else
	{
		IsBlur = IsLoad;
	}

	// Rendering
	ImGui::Render();

	if (IsBlur)
	{
		dx11Renderer.BeginRenderBlur(dx11Renderer.m_pContext, iomain.DisplaySize.y, iomain.DisplaySize.x);
	} 
	else 
	{
		dx11Renderer.m_pContext->OMSetRenderTargets(1, &dx11Renderer.m_pRenderTargetView, nullptr);
		dx11Renderer.m_pContext->ClearRenderTargetView(dx11Renderer.m_pRenderTargetView, (float*)&clear_color);
	}

	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	
	dx11Renderer.m_pDXGISwapChain->Present(1, 0); // Present with vsync
}

LRESULT
WINAPI
WndProc(
	HWND hWnd,
	UINT msg,
	WPARAM wParam, 
	LPARAM lParam
)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{
	case WM_COPY:
		break;
	case WM_ENTERSIZEMOVE:
		SetTimer(hWnd, 2, 4, nullptr);
		Window_Flag_Resizeing = true;
		return 0;
	case WM_EXITSIZEMOVE:
		KillTimer(hWnd, 2);
		Window_Flag_Resizeing = false;
		return 0;
	case WM_TIMER:
		RedrawWindow(hWnd, nullptr, nullptr, RDW_INVALIDATE | RDW_INVALIDATE);
		return 0;
	case WM_PAINT:
		if (Window_Flag_Resizeing)
		{
			CycleFunc();
		}
		break;
	case WM_SIZE:
		if (dx11Renderer.m_pDevice != nullptr && wParam != SIZE_MINIMIZED)
		{
			_RELEASE(dx11Renderer.m_pRenderTargetView);
			dx11Renderer.m_pDXGISwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);

			ID3D11Texture2D* pBackBuffer = nullptr;
			dx11Renderer.m_pDXGISwapChain->GetBuffer(NULL, IID_PPV_ARGS(&pBackBuffer));
			dx11Renderer.m_pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &dx11Renderer.m_pRenderTargetView);
			_RELEASE(pBackBuffer);
		}
		if (wParam == SIZE_MINIMIZED)
		{
			Sleep(1);
		}
		return 0;
	case WM_SYSCOMMAND:
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	default:
		Sleep(1);
		break;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

STRING_PATH szPath = { NULL };

DWORD StartApplication(LPWSTR lpCmdLine)
{
	MSG msg = { nullptr };
	IOSRUI* pUI = new IOSRUI();
	IOSRMixer* pMixer = new IOSRMixer();
	
	// set begin for multimedia period (needy for Sleep(16) or lower sleep time)
	timeBeginPeriod(1);
	MFStartup(MF_VERSION);

	{
		// init static string with application path
		LPWSTR lpPathe = nullptr;
		GetApplicationDirectory(&lpPathe);
	}

	pUI->CreateMainWindow();

	SetWindowLongPtr((HWND)pUI->WindowHandle, -21, (LONG_PTR)pMixer);

	dropTarget.Window = pUI->WindowHandle;
	dropTarget.AddMixer(pMixer);
	RegisterDragDrop(pUI->WindowHandle, &dropTarget);

	disc.Init();
	disc.SetStatus(DiscordNetwork::StatusNumber::Waiting);

	// Main loop
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			continue;
		}

		Sleep(1);
		CycleFunc();
		//g_pSwapChain->Present(0, 0); // Present without vsync
	}

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	// set end for period (in 1/10 cases we can get BSoD if failed) 
	timeEndPeriod(1);
	MFShutdown();

	return 0;
}

OSRCODE
IOSRUI::CreateMainWindow() 
{
	// create window class
	WNDCLASSEXW wc = { sizeof(WNDCLASSEXW), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"OSR_DAW", nullptr };
	wc.hIcon = LoadIcon(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDI_OSR));
	wc.hIconSm = LoadIcon(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDI_OSRSMALL));
	RegisterClassExW(&wc);

	RECT rec = { 0, 0, 640, 360 };
	AdjustWindowRectEx(&rec, WS_OVERLAPPEDWINDOW, FALSE, WS_EX_APPWINDOW);

	WindowHandle = CreateWindowW(
		L"OSR_DAW",
		L"Open Sound Refenation 0.51A",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		rec.right - rec.left,
		rec.bottom - rec.top,
		nullptr,
		nullptr,
		wc.hInstance,
		nullptr
	);

	if (OSRFAILED(dx11Renderer.CreateRenderWindow(WindowHandle)))
	{
		THROW2(L"Can't create render window");
	}

	ShowWindow(WindowHandle, SW_SHOWDEFAULT);
	UpdateWindow(WindowHandle);

	TaskbarValue** pTask = GetTaskbarPointer();
	*pTask = new TaskbarValue(WindowHandle);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->Clear();

	GetCurrentDirectoryA(sizeof(STRING_PATH), szPath);
	snprintf(szPath, sizeof(STRING_PATH), "%s%s", szPath, "\\arimo_reg.ttf"); //-V541

	ImFont* font = io.Fonts->AddFontFromFileTTF(szPath, 16.0f);
	if (font) 
	{
		io.FontDefault = font; 
	}
	else 
	{ 
		io.Fonts->AddFontDefault();
	}
	io.Fonts->Build();

	ImGui_ImplWin32_Init(WindowHandle);
	ImGui_ImplDX11_Init(dx11Renderer.m_pDevice, dx11Renderer.m_pContext);

	// Setup style
	ImGui::StyleColorsDark();

	return OSR_SUCCESS;
}
