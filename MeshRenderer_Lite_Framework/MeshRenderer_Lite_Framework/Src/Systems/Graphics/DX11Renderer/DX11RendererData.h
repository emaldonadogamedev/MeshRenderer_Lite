#pragma once

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
	std::vector<ID3D11Texture1D*> textures1D;
	std::vector<ID3D11Texture2D*> textures2D;
	std::vector<ID3D11Texture3D*> textures3D;
	std::vector<ID3D11RenderTargetView*> renderTargets;
	std::vector<std::pair<ID3D11VertexShader*, ID3D11InputLayout*>> vertexShaders;
	std::vector<ID3D11PixelShader*> pixelShaders;
	std::vector<ID3D11GeometryShader*> geometryShaders;
	std::vector<ID3D11GeometryShader*> computeShaders;
	std::vector<ID3D11HullShader*> hullShaders;
	std::vector<ID3D11Buffer*> vertexBuffers;
	std::vector<ID3D11Buffer*> indexBuffers;
	std::vector<ID3D11Buffer*> constantBuffers;
	std::vector<std::pair<ID3D11Texture2D*, ID3D11DepthStencilView*>> depthBuffers_AndViews;

	//Sampler states for textures
	ID3D11SamplerState* m_pDiffSampleState, *m_pSpecSampleState, *m_pNormalSampleState;

	//////////////////////////////////////////////////////////////////////////
	// TEST RESOURCES!
	ID3D11VertexShader* testVertexShader;

	ID3D11PixelShader* testPixelShader;

	ID3D11Buffer* testConstBuffer;

};