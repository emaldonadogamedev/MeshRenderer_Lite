#pragma once

#include <Systems/Graphics/GraphicsUtilities/D3DObjects.h>
#include <Systems/Graphics/BufferClasses/ConstantBuffers.h>

class GraphicsSystem;

class DX11RendererData
{
public:
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pImmediateContext = nullptr;
	IDXGISwapChain* m_pSwapChain = nullptr;

	ID3D11Texture2D* m_DepthStencilBuffer = nullptr;
	ID3D11DepthStencilView* m_DepthStencilView = nullptr;

	//Pointer to the main render target stored on the GPU
	ID3D11RenderTargetView* m_pMainRenderTargetView = nullptr;
	XMVECTOR m_clearColor = { 0,0.3f,0,1 };

	D3D_DRIVER_TYPE m_DriverType;
	D3D_FEATURE_LEVEL m_FeatureLevel;
	D3D_PRIMITIVE_TOPOLOGY m_currentPrimitiveTopology;
	D3D11_VIEWPORT m_mainViewport;

	//rasterizer states
	ID3D11RasterizerState* m_currentRasterState = nullptr;
	ID3D11RasterizerState* m_d3dRasterStateDefault = nullptr;
	ID3D11RasterizerState* m_d3dRasterStateSolCullBack = nullptr;
	ID3D11RasterizerState* m_d3dRasterStateSolCullFront = nullptr;
	ID3D11RasterizerState* m_d3dRasterStateWireframe = nullptr;
	ID3D11RasterizerState* m_d3dRasterStateImgui = nullptr;
	
	ID3D11InputLayout* m_pVSInputLayoutVertexWire = nullptr;
	ID3D11InputLayout* m_pVSInputLayoutVertexNormal = nullptr;
	ID3D11InputLayout* m_pVSInputLayoutVertexTexture = nullptr;
	ID3D11InputLayout* m_pVSInputLayoutVertexAnimation = nullptr;

	//Resource containers
	std::vector<RenderTarget> renderTargets;
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
	ID3D11SamplerState* m_pWrapSamplerState = nullptr;
	ID3D11SamplerState* m_pMirrorSamplerState = nullptr;
	ID3D11SamplerState* m_pClampSamplerState = nullptr;
	ID3D11SamplerState* m_pBorderSamplerState = nullptr;

	//////////////////////////////////////////////////////////////////////////
	// TEST RESOURCES!
	PerObectBuffer testPerObjectBuffer;
	ID3D11Buffer* testPerObjectConstBuffer = nullptr;

	ViewProjBuffer testViewProjBuffer;
	ID3D11Buffer* testViewProjConstBuffer = nullptr;

	AnimationBuffer testAnimationBuffer;
	ID3D11Buffer* testAnimationConstBuffer = nullptr;

	SimpleCloth_ConstBuffer testSimpleClothBuffer;
	ID3D11Buffer* testSimpleClothConstBuffer = nullptr;

	ID3D11Buffer* testLightConstBuffer = nullptr;

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

	const DXGI_FORMAT dxgiFormatArrHelper[(int)DataFormat::COUNT] =
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

	friend GraphicsSystem;
	friend DX11Renderer;
};