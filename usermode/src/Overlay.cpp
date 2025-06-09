#define IMGUI_DISABLE_DEBUG_TOOLS

#include "Overlay.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"
#include <dinput.h>
#include <d3dcompiler.h>
#include <exception>
#include <iostream>


#pragma comment(lib, "d3d11.lib")

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI WndProc(HWND handle, UINT msg, WPARAM wParam, LPARAM lParam)
{
	Overlay* overlay = reinterpret_cast<Overlay*>(GetWindowLongPtr(handle, GWLP_USERDATA));
	if (ImGui_ImplWin32_WndProcHandler(handle, msg, wParam, lParam)) {
		return true;
	}
	switch (msg) {

	case WM_NCHITTEST:
			if (overlay && overlay->isMenuVisible())
				return HTCLIENT;
			break;
	case WM_SYSCOMMAND:
			if ((wParam & 0xFFF0) == SC_KEYMENU)
				return 0;
			break;
		case WM_DESTROY:
			::PostQuitMessage(0);
			return 0;
	}
	return ::DefWindowProcW(handle, msg, wParam, lParam);

}
Overlay::Overlay(const wchar_t* windowName)
{
	wc = { sizeof(WNDCLASSEXW) };
	wc.lpfnWndProc = WndProc;
	wc.lpszClassName = windowName;
	wc.hInstance = GetModuleHandle(nullptr);
	RegisterClassExW(&wc);

	windowHandle = CreateWindowExW(WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST | WS_EX_TOOLWINDOW, windowName, windowName, WS_POPUP, 0, 0, GetSystemMetrics(SM_CXSCREEN), 
		GetSystemMetrics(SM_CYSCREEN), nullptr, nullptr, wc.hInstance, nullptr);
	SetLayeredWindowAttributes(windowHandle, RGB(0, 0, 0), 255, LWA_COLORKEY | LWA_ALPHA);
	SetWindowPos(windowHandle, HWND_TOPMOST, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOMOVE);

	SetWindowLongPtr(windowHandle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
	
	RegisterHotKey(NULL, 1, MOD_NOREPEAT, VK_DELETE); // DELETE to toggle menu

	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferDesc.Width = GetSystemMetrics(SM_CXSCREEN);
	sd.BufferDesc.Height = GetSystemMetrics(SM_CYSCREEN);
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 120;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = windowHandle;
	sd.SampleDesc.Count = 1;
	sd.Windowed = TRUE;
	sd.BufferCount = 1;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	D3D_FEATURE_LEVEL featureLevel;
	const D3D_FEATURE_LEVEL featureLevelArray[1] = { D3D_FEATURE_LEVEL_11_0 };
	HRESULT hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, D3D11_CREATE_DEVICE_BGRA_SUPPORT, featureLevelArray,
		1, D3D11_SDK_VERSION, &sd, &d3dSwapChain, &d3dDevice, &featureLevel, &d3dDeviceContext);

	if (FAILED(hr)) {
		throw std::exception("Failed to create device and swap chain");
	}

	ID3D11Texture2D* pBackBuffer = nullptr;
	d3dSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
	d3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &d3dRenderTargetView);
	pBackBuffer->Release();

	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; 

	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(windowHandle);
	ImGui_ImplDX11_Init(d3dDevice, d3dDeviceContext);

	ShowWindow(windowHandle, SW_SHOW);

}

Overlay::~Overlay()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

		d3dRenderTargetView->Release();
		d3dSwapChain->Release();
		d3dDeviceContext->Release();
		d3dDevice->Release();

		DestroyWindow(windowHandle);
		UnregisterClass(wc.lpszClassName, wc.hInstance);
	
}

bool Overlay::isMenuVisible()
{
	return showMenu;
}

void Overlay::toggleMenu()
{
	showMenu = !showMenu;
	std::cout << "Menu Toggled" << std::endl;
	LONG style = GetWindowLong(windowHandle, GWL_EXSTYLE);
	if (showMenu)
		style &= ~WS_EX_TRANSPARENT;
	else 
		style |= WS_EX_TRANSPARENT;
	SetWindowLongW(windowHandle, GWL_EXSTYLE, style);
	SetLayeredWindowAttributes(windowHandle, RGB(0, 0, 0), 255, LWA_COLORKEY | LWA_ALPHA);

}

void Overlay::startRender()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	float clearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	d3dDeviceContext->ClearRenderTargetView(d3dRenderTargetView, clearColor);
}

void Overlay::render()
{

	if (!showMenu) 
		return;
	ImGui::Begin("LaerkeWare", &showMenu);

	if (ImGui::Button("A")) {
		std::cout << "A" << std::endl;
	}

	
}

void Overlay::endRender()
{
	if (showMenu)
		ImGui::End();
	ImGui::Render();
	d3dDeviceContext->OMSetRenderTargets(1, &d3dRenderTargetView, nullptr);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	d3dSwapChain->Present(1, 0);
}

void Overlay::renderLoop()
{
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));

	while (msg.message != WM_QUIT) {
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) 
				break;
			if (msg.message == WM_HOTKEY) {
				if (msg.wParam == 1)  // Hotkey ID for VK_DELETE
					toggleMenu();
			}
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		startRender();
		render();
		endRender();
		d3dSwapChain->Present(1, 0); 

		}
}

void Overlay::handleMessages()
{
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));

	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT)
				break;
			if (msg.message == WM_HOTKEY) {
				if (msg.wParam == 1)  // Hotkey ID for VK_DELETE
					toggleMenu();
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}


}
