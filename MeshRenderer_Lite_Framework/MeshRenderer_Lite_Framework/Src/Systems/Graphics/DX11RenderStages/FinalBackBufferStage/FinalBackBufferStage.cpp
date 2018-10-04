#include <Utilities/precompiled.h>
#include <Systems/Graphics/DX11RenderStages/FinalBackBufferStage/FinalBackBufferStage.h>

#include<Systems/Graphics/DX11Renderer/DX11Renderer.h>
#include<Systems/Graphics/DX11Renderer/DX11RendererData.h>

FinalBackBufferStage::FinalBackBufferStage(DX11Renderer* const renderer, RenderCompUmap* const gfxComponents, const ObjectHandle& fsqIndexBuffer)
		:IRenderStage(renderer, gfxComponents)
{

}

FinalBackBufferStage::~FinalBackBufferStage()
{

}

void FinalBackBufferStage::PreRender()
{
		//bind main render target and clear it
		m_renderData.m_pImmediateContext->OMSetRenderTargets(1, &m_renderData.m_pBackBufferRenderTargetView, nullptr); //No depth testing required
		m_renderData.m_pImmediateContext->ClearRenderTargetView(m_renderData.m_pBackBufferRenderTargetView, m_renderData.m_clearColor.m128_f32);
		m_renderData.m_pImmediateContext->RSSetState(m_renderData.m_d3dRasterStateDefault);
		m_renderData.m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		m_renderer->DisableColorBlending();

		m_renderData.m_pImmediateContext->RSSetViewports(1, &m_renderData.m_mainViewport);
}

void FinalBackBufferStage::Render(HandleDictionaryVec& graphicsResources, const float dt)
{
		ObjectHandle handle = (graphicsResources[(int)ObjectType::VERTEX_SHADER]).at("FullScreenQuadVS");
		m_renderer->BindVertexShader(handle);

		handle = (graphicsResources[(int)ObjectType::PIXEL_SHADER]).at("CopyRenderTarget");
		m_renderer->BindPixelShader(handle);

		m_renderer->BindNullVertexBuffer(); //we create the geometry on the Vertex Shader
		m_renderer->BindIndexBuffer(m_fsqIndexBuffer);

		m_renderer->BindTextureShaderResource(ObjectType::PIXEL_SHADER, 24, 1, m_renderData.m_MainRenderTargets[m_renderData.m_currentMainRTindex]);

		m_renderer->DrawIndexed(6);
}

void FinalBackBufferStage::PostRender()
{

}
