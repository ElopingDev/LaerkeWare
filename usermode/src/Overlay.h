#pragma once
#include <d3d11.h>
#include <dwmapi.h>

class Overlay
{
public:
	Overlay(const wchar_t* windowName);
	~Overlay();
	bool isMenuVisible();
	void toggleMenu();
	void startRender();
	void render();
	void endRender();
	void renderLoop();
	void handleMessages();

private:
	WNDCLASSEXW wc;
	HWND windowHandle = nullptr;

	ID3D11Device* d3dDevice = nullptr;
	ID3D11DeviceContext* d3dDeviceContext = nullptr;
	IDXGISwapChain* d3dSwapChain = nullptr;
	ID3D11RenderTargetView* d3dRenderTargetView = nullptr;

	unsigned int resizeWidth;
	unsigned int resizeHeight;

	bool showMenu = false;
};

