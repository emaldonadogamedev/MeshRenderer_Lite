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
		m_renderData.m_currentMainRTindex = 0;
		m_renderer->BindRenderTarget(m_renderData.m_MainRenderTargets[m_renderData.m_currentMainRTindex], false);//No depth testing 
		m_renderer->ClearRenderTarget(m_renderData.m_MainRenderTargets[m_renderData.m_currentMainRTindex], m_renderData.m_clearColor);


		//m_renderData.m_pImmediateContext->OMSetRenderTargets(1, &m_renderData.m_pBackBufferRenderTargetView, nullptr); //No depth testing required
		//m_renderData.m_pImmediateContext->ClearRenderTargetView(m_renderData.m_pBackBufferRenderTargetView, m_renderData.m_clearColor.m128_f32);
		m_renderData.m_pImmediateContext->RSSetState(m_renderData.m_d3dRasterStateDefault);
		m_renderData.m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		m_renderer->DisableAlphaBlending();

		m_renderData.m_pImmediateContext->RSSetViewports(1, &m_renderData.m_mainViewport);
}

void AmbientLightStage::Render(HandleDictionaryVec& graphicsResources, const float dt)
{
		ObjectHandle handle = (graphicsResources[(int)ObjectType::VERTEX_SHADER]).at("FullScreenQuadVS");
		m_renderer->BindVertexShader(handle);

		handle = (graphicsResources[(int)ObjectType::PIXEL_SHADER]).at("DeferredAmbientStagePS");
		m_renderer->BindPixelShader(handle);

		m_renderData.m_pImmediateContext->PSSetConstantBuffers(7, 1, &m_renderData.testLightWithShadowConstBuffer);

		m_renderer->BindNullVertexBuffer(); //we create the geometry on the Vertex Shader
		m_renderer->BindIndexBuffer(m_fsqIndexBuffer);

		m_renderer->BindTextureShaderResource(ObjectType::PIXEL_SHADER, 22, 1, m_renderData.m_GBufferObjHandles[(char)DX11RendererData::GBufferRTType::DIFFUSE]);

		m_renderer->DrawIndexed(6);
}

void AmbientLightStage::PostRender()
{
		m_renderData.m_currentMainRTindex = !m_renderData.m_currentMainRTindex;
}
