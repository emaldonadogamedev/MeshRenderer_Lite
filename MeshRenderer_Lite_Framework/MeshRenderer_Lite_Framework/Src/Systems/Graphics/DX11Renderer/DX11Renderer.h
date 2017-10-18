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

	void ClearBuffer(void);
	void SwapBuffers(void);

	//////////////////////////////////////////////////////////////////////////
	//Drawing Functions
	void Draw(unsigned vertexCount, unsigned startVertexLocation = 0);
	void DrawIndexed(unsigned indexCount, unsigned startIndexLocation = 0, unsigned baseVertexLocation = 0);
	void DrawInstanced(unsigned vertexCount, unsigned instanceCount, unsigned startVertexLocation = 0,
		unsigned startInstanceLocation = 0);
	void DrawIndexedInstanced(unsigned indexCountPerInstance, unsigned instanceCount, unsigned startIndexLocation = 0,
		unsigned baseVertexLocation = 0, unsigned startInstanceLocation = 0);

protected:
	bool InitializeD3D(const int width, const int height, HWND hwnd);
	bool InitializeTextureSamplers();


	std::unique_ptr<DX11RendererData> m_renderData;

	bool m_isInitialized = false;

	friend GraphicsSystem;
};