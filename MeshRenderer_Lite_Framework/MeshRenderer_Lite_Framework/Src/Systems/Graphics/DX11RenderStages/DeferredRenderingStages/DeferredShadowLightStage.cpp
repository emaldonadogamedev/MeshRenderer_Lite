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
		m_renderData.m_pImmediateContext->RSSetState(m_renderData.m_d3dRasterStateSolCullNone);
		m_renderData.m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		m_renderer->EnableAdditiveBlending();

		//Bind all of the shadow maps
		const auto& lightComponents = (*m_gfxSystemComponents)[ComponentType::RENDERABLE_LIGHT_WITH_SHADOW];
		for (const auto& component : lightComponents)
		{
				ShadowLightComponent* lightComp = (ShadowLightComponent*)component;
				if (lightComp->IsUsingShadows())
				{
						m_renderer->BindTextureShaderResource(ObjectType::PIXEL_SHADER, lightComp->GetShadowTextureIdx(), 1, lightComp->GetShadowDepthMapHandle());
				}
		}

		//Bind the G-buffer render targets
		for (char rtHandle = 0, textureId = 20; rtHandle < (char)DX11RendererData::GBufferRTType::COUNT; ++rtHandle, ++textureId)
		{
				m_renderer->BindTextureShaderResource(ObjectType::PIXEL_SHADER, textureId, 1, m_renderData.m_GBufferObjHandles[rtHandle]);
		}
}

void DeferredShadowLightStage::Render(HandleDictionaryVec& graphicsResources, const float dt)
{
		if (m_renderData.testGlobalShaderProperties.gDebugInfoType != (int)GlobalGraphicsDebugType::G_DEBUG_NONE)
				return;

		m_renderer->BindNullVertexBuffer(); //we create the geometry on the Vertex Shader
		m_renderer->BindIndexBuffer(m_fsqIndexBuffer);

		// Set light POV buffers
		m_renderData.m_pImmediateContext->PSSetConstantBuffers(5, 1, &m_renderData.testLightViewConstBuffer);

		ObjectHandle handle = (graphicsResources[(int)ObjectType::VERTEX_SHADER]).at("FullScreenQuadVS");
		m_renderer->BindVertexShader(handle);

		handle = (graphicsResources[(int)ObjectType::PIXEL_SHADER]).at("DeferredShadowLightStagePS");
		m_renderer->BindPixelShader(handle);

		m_renderer->DrawIndexed(6);
}

void DeferredShadowLightStage::PostRender()
{
}
