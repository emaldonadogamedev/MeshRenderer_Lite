#include <Utilities/precompiled.h>

#include <Systems/Graphics/DX11RenderStages/SkyBoxRenderStage/SkyBoxRenderStage.h>

#include<Systems/Graphics/DX11Renderer/DX11Renderer.h>
#include<Systems/Graphics/DX11Renderer/DX11RendererData.h>

SkyBoxRenderStage::SkyBoxRenderStage(DX11Renderer* const renderer, RenderCompUmap* const gfxComponents, const Model* const boxGeometry)
	:IRenderStage(renderer, gfxComponents)
	, m_boxModel(boxGeometry)
{

}

SkyBoxRenderStage::~SkyBoxRenderStage()
{

}

void SkyBoxRenderStage::PreRender()
{
	m_renderData.m_pImmediateContext->RSSetState(m_renderData.m_d3dRasterStateSolCullFront); 
	m_renderData.m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_renderer->DisableColorBlending();
}

void SkyBoxRenderStage::Render(HandleDictionaryVec& graphicsResources, const float dt)
{

}

void SkyBoxRenderStage::PostRender()
{

}
