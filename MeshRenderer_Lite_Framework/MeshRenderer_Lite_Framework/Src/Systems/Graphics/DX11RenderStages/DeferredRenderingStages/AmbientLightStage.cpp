#include <Utilities/precompiled.h>
#include <Systems//Graphics/DX11RenderStages/DeferredRenderingStages/AmbientLightStage.h>

#include<Systems/Graphics/DX11Renderer/DX11Renderer.h>
#include<Systems/Graphics/DX11Renderer/DX11RendererData.h>

AmbientLightStage::AmbientLightStage(DX11Renderer* const renderer, RenderCompUmap* const gfxComponents, const ObjectHandle& fsqIndexBuffer)
		:IRenderStage(renderer, gfxComponents)
		, m_fsqIndexBuffer(fsqIndexBuffer)
{
}

AmbientLightStage::~AmbientLightStage()
{

}

void AmbientLightStage::PreRender()
{
		//bind main render target and clear it
		auto& renderData = m_renderer->GetRendererData();
		renderData.m_pImmediateContext->OMSetRenderTargets(1, &renderData.m_pMainRenderTargetView, nullptr); //No depth testing required
		renderData.m_pImmediateContext->ClearRenderTargetView(renderData.m_pMainRenderTargetView, renderData.m_clearColor.m128_f32);
		renderData.m_pImmediateContext->RSSetState(renderData.m_currentRasterState);
		renderData.m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		m_renderer->DisableAlphaBlending();

		D3D11_VIEWPORT viewport{ 0,0,m_renderer->GetRenderTargetWidth(), m_renderer->GetRenderTargetHeight(),0,1.0f };
		renderData.m_pImmediateContext->RSSetViewports(1, &viewport);
}

void AmbientLightStage::Render(HandleDictionaryVec& graphicsResources, const float dt)
{
		m_renderer->BindNullVertexBuffer();
		m_renderer->BindIndexBuffer(m_fsqIndexBuffer);

		const ObjectHandle& handle = (graphicsResources[(int)ObjectType::VERTEX_SHADER]).at("FullScreenQuadVS");
		m_renderer->BindVertexShader(handle);
}

void AmbientLightStage::PostRender()
{

}
