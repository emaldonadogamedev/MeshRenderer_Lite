#include <Utilities/precompiled.h>
#include <Systems/Graphics/DX11RenderStages/DeferredRenderingStages/DeferredShadowLightStage.h>

#include<Systems/Graphics/Components/LightComponents/Light.h>
#include<Systems/Graphics/Components/LightComponents/ShadowLightComponent/ShadowLightComponent.h>
#include<Systems/Graphics/DX11Renderer/DX11Renderer.h>
#include<Systems/Graphics/DX11Renderer/DX11RendererData.h>

DeferredShadowLightStage::DeferredShadowLightStage(DX11Renderer* const renderer, RenderCompUmap* const gfxComponents, const ObjectHandle& fsqIndexBuffer)
		:IRenderStage(renderer, gfxComponents)
		, m_fsqIndexBuffer(fsqIndexBuffer)
{

}

DeferredShadowLightStage::~DeferredShadowLightStage()
{

}

void DeferredShadowLightStage::PreRender()
{
		//bind main render target and clear it
		//m_renderer->BindRenderTarget(m_renderData.m_MainRenderTargets[m_renderData.m_currentMainRTindex], false);//No depth testing 
		//m_renderer->ClearRenderTarget(m_renderData.m_MainRenderTargets[m_renderData.m_currentMainRTindex], m_renderData.m_clearColor);

		//bind main render target and clear it
		//m_renderData.m_pImmediateContext->OMSetRenderTargets(1, &m_renderData.m_pBackBufferRenderTargetView, nullptr); //No depth testing required
		m_renderData.m_pImmediateContext->RSSetState(m_renderData.m_d3dRasterStateDefault);
		m_renderData.m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		m_renderer->EnableAdditiveBlending();

		//Bind all of the shadow maps
		const auto& lightComponents = (*m_gfxSystemComponents)[ComponentType::RENDERABLE_LIGHT_WITH_SHADOW];
		for (const auto& component : lightComponents)
		{
				ShadowLightComponent* lightComp = (ShadowLightComponent*)component;
				m_renderer->BindTextureShaderResource(ObjectType::PIXEL_SHADER, lightComp->GetShadowTextureIdx(), 1,
						lightComp->GetShadowRThandle());
		}

		//Bind the G-buffer render targets
		for (char rtHandle = 0, textureId = 20; rtHandle < (char)DX11RendererData::GBufferRTType::COUNT; ++rtHandle, ++textureId)
		{
				m_renderer->BindTextureShaderResource(ObjectType::PIXEL_SHADER, textureId, 1, m_renderData.m_GBufferObjHandles[rtHandle]);
		}
}

void DeferredShadowLightStage::Render(HandleDictionaryVec& graphicsResources, const float dt)
{
		m_renderer->BindNullVertexBuffer(); //we create the geometry on the Vertex Shader
		m_renderer->BindIndexBuffer(m_fsqIndexBuffer);

		ObjectHandle handle = (graphicsResources[(int)ObjectType::VERTEX_SHADER]).at("FullScreenQuadVS");
		m_renderer->BindVertexShader(handle);

		handle = (graphicsResources[(int)ObjectType::PIXEL_SHADER]).at("DeferredShadowLightStagePS");
		m_renderer->BindPixelShader(handle);

		m_renderer->DrawIndexed(6);
}

void DeferredShadowLightStage::PostRender()
{
}
