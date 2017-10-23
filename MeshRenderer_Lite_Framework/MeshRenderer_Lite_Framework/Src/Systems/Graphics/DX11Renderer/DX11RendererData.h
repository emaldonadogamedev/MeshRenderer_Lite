#pragma once

#include <Systems/Graphics/GraphicsUtilities/D3DObject.h>

struct DX11RendererData
{
	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pImmediateContext;
	IDXGISwapChain* m_pSwapChain;

	ID3D11Texture2D* m_DepthStencilBuffer;
	ID3D11DepthStencilView* m_DepthStencilView;

	//Pointer to the main render target stored on the GPU
	ID3D11RenderTargetView* m_pMainRenderTargetView;
	float m_clearColor[4] = { 0,0,0,0 };

	D3D_DRIVER_TYPE m_DriverType;
	D3D_FEATURE_LEVEL m_FeatureLevel;
	D3D_PRIMITIVE_TOPOLOGY m_currentPrimitiveTopology;
	D3D11_VIEWPORT m_mainViewport;

	//rasterizer states
	ID3D11RasterizerState* m_d3dRasterStateDefault;
	ID3D11RasterizerState* m_d3dRasterStateSolCullBack;
	ID3D11RasterizerState* m_d3dRasterStateSolCullFront;
	ID3D11RasterizerState* m_d3dRasterStateWireframe;
	ID3D11RasterizerState* m_d3dRasterStateImgui;
	
	ID3D11InputLayout* m_pVSInputLayoutVertexWire;
	ID3D11InputLayout* m_pVSInputLayoutVertexNormal;
	ID3D11InputLayout* m_pVSInputLayoutVertexTexture;

	//Resource containers
	std::vector<Texture1D> textures1D;
	std::vector<Texture2D> textures2D;
	std::vector<Texture3D> textures3D;
	std::vector<VertexShader> vertexShaders;
	std::vector<PixelShader> pixelShaders;
	std::vector<GeometryShader> geometryShaders;
	std::vector<ComputeShader> computeShaders;
	std::vector<Buffer> vertexBuffers;
	std::vector<Buffer> indexBuffers;
	std::vector<Buffer> constantBuffers;

	//Sampler states for textures
	ID3D11SamplerState* m_pDiffSampleState, *m_pSpecSampleState, *m_pNormalSampleState;

	//////////////////////////////////////////////////////////////////////////
	// TEST RESOURCES!
	ID3D11Buffer* testVertBuffer;
	ID3D11VertexShader* testVertexShader;
	ID3D11PixelShader* testPixelShader;
	ID3D11Buffer* testPerObjectConstBuffer;
	ID3D11Buffer* testViewProjConstBuffer;

private:
	template<typename Container>
	int NextAvailableIndex(const Container& container)
	{
		//Iterate through container
		for (unsigned i = 0; i < container.size(); ++i)
		{
			//If the element is a free and already deleted handle,
			//return it's index
			if (container[i].deleted)
				return i;
		}

		//No free spaces
		return -1;
	}

	DXGI_FORMAT dxgiFormatArrHelper[(int)DataFormat::COUNT] =
	{
		DXGI_FORMAT_R8_UNORM,
		DXGI_FORMAT_R8G8_UNORM,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_FORMAT_B8G8R8A8_UNORM,
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		DXGI_FORMAT_R32G32B32_FLOAT,
		DXGI_FORMAT_R32G32_FLOAT,
		DXGI_FORMAT_R32_FLOAT,
		DXGI_FORMAT_R32_UINT,
		DXGI_FORMAT_R32G32_UINT,
		DXGI_FORMAT_R32G32B32_UINT,
		DXGI_FORMAT_R32G32B32A32_UINT,
		DXGI_FORMAT_R32_SINT,
		DXGI_FORMAT_R32G32_SINT,
		DXGI_FORMAT_R32G32B32_SINT,
		DXGI_FORMAT_R32G32B32A32_SINT
	};

	friend DX11Renderer;
};