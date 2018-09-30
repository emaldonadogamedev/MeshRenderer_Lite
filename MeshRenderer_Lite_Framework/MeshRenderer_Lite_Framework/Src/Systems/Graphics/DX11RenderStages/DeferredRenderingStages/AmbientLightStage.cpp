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
		m_renderData.m_pImmediateContext->OMSetRenderTargets(1, &m_renderData.m_pMainRenderTargetView, nullptr); //No depth testing required
		m_renderData.m_pImmediateContext->ClearRenderTargetView(m_renderData.m_pMainRenderTargetView, m_renderData.m_clearColor.m128_f32);
		m_renderData.m_pImmediateContext->RSSetState(m_renderData.m_d3dRasterStateDefault);
		m_renderData.m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		m_renderer->DisableAlphaBlending();

		D3D11_VIEWPORT viewport{ 0,0,m_renderer->GetRenderTargetWidth(), m_renderer->GetRenderTargetHeight(), 0, 1.0f };
		m_renderData.m_pImmediateContext->RSSetViewports(1, &viewport);

}

void AmbientLightStage::Render(HandleDictionaryVec& graphicsResources, const float dt)
{
		ObjectHandle handle = (graphicsResources[(int)ObjectType::VERTEX_SHADER]).at("FullScreenQuadVS");
		m_renderer->BindVertexShader(handle);

		handle = (graphicsResources[(int)ObjectType::PIXEL_SHADER]).at("DeferredAmbientStagePS");
		m_renderer->BindPixelShader(handle);


		//renderData.m_pImmediateContext->VSSetShader(nullptr, nullptr, 0);
		//renderData.m_pImmediateContext->IASetInputLayout(nullptr);

		//renderData.m_pImmediateContext->VSSetConstantBuffers(7, 1, &renderData.testLightConstBuffer);
		m_renderData.m_pImmediateContext->PSSetConstantBuffers(7, 1, &m_renderData.testLightWithShadowConstBuffer);

		m_renderer->BindNullVertexBuffer(); //we create the geometry on the Vertex Shader
		m_renderer->BindIndexBuffer(m_fsqIndexBuffer);

		m_renderer->BindTextureShaderResource(ObjectType::PIXEL_SHADER, 22, 1, m_renderData.m_GBufferObjHandles[(char)DX11RendererData::GBufferRTType::DIFFUSE]);

		m_renderer->DrawIndexed(6);
}

void AmbientLightStage::PostRender()
{

}
