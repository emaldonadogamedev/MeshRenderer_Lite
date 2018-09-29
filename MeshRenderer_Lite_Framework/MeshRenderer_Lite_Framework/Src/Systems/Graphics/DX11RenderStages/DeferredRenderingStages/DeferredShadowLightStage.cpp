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
		auto& renderData = m_renderer->GetRendererData();
		renderData.m_pImmediateContext->OMSetRenderTargets(1, &renderData.m_pMainRenderTargetView, nullptr); //No depth testing required
		renderData.m_pImmediateContext->RSSetState(renderData.m_d3dRasterStateDefault);
		renderData.m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		m_renderer->EnableAlphaBlending();

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
				m_renderer->BindTextureShaderResource(ObjectType::PIXEL_SHADER, textureId, 1, renderData.m_GBufferObjHandles[rtHandle]);
		}

		D3D11_VIEWPORT viewport{ 0, 0, m_renderer->GetRenderTargetWidth(), m_renderer->GetRenderTargetHeight(), 0, 1.0f };
		renderData.m_pImmediateContext->RSSetViewports(1, &viewport);
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
