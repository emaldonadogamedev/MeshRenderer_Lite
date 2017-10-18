#pragma once

#include<memory>

class GraphicsSystem;
class DX11RendererData;

class DX11Renderer
{
public:
	DX11Renderer();
	~DX11Renderer();

	bool InitializeRenderer();
	bool ReleaseData();

protected:
	bool InitializeDevice();
	bool InitializeSwapChain();

	std::unique_ptr<DX11RendererData*> m_renderData;

	friend GraphicsSystem;
};