#pragma once

#include<memory>

class GraphicsSystem;
class DX11RendererData;

class DX11Renderer
{
public:
	DX11Renderer();
	~DX11Renderer();

	bool InitializeRenderer(const int width, const int height, HWND hwnd);
	void ReleaseData();

protected:
	bool InitializeD3D(const int width, const int height, HWND hwnd);
	bool InitializeTextureSamplers();

	std::unique_ptr<DX11RendererData> m_renderData;

	bool m_isInitialized = false;

	friend GraphicsSystem;
};