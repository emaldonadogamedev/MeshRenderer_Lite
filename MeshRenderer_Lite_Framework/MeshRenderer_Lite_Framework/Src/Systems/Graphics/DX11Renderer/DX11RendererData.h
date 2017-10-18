#pragma once

#include<d3d11.h>

struct DX11RendererData
{
	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pImmediateContext;
	IDXGISwapChain* m_pSwapChain;
	ID3D11Texture2D* m_DepthStencilBuffer;
	ID3D11RenderTargetView* m_pRenderTargetView;
	ID3D11DepthStencilView* m_DepthStencilView;
	D3D_DRIVER_TYPE m_DriverType;
	D3D_FEATURE_LEVEL m_FeatureLevel;
	D3D_PRIMITIVE_TOPOLOGY m_currentPrimitiveTopology;
	D3D11_VIEWPORT m_mainViewport;

	ID3D11RasterizerState* m_d3dRasterStateDefault;
	ID3D11RasterizerState* m_d3dRasterStateSolCullBack;
	ID3D11RasterizerState* m_d3dRasterStateSolCullFront;
	ID3D11RasterizerState* m_d3dRasterStateWireframe;
	ID3D11RasterizerState* m_d3dRasterStateImgui;

	//Sampler states for textures
	ID3D11SamplerState* m_pDiffSampleState, *m_pSpecSampleState, *m_pNormalSampleState;
};