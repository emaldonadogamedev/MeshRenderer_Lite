#include <Utilities/precompiled.h>
#include <Systems/Graphics/DX11RenderStages/DeferredRenderingStages/DeferredSimpleLightStage.h>

#include <Systems/Core/Components/Transform/Transform.h>
#include <Systems/Core/GameObject/GameObject.h>
#include<Systems/Graphics/Components/LightComponents/Light.h>
#include<Systems/Graphics/Components/LightComponents/LightComponent/LightComponent.h>
#include<Systems/Graphics/DX11Renderer/DX11Renderer.h>
#include<Systems/Graphics/DX11Renderer/DX11RendererData.h>
#include<Systems/Graphics/ModelClasses/Model/Model.h>

DeferredSimpleLightStage::DeferredSimpleLightStage(DX11Renderer* const renderer, RenderCompUmap* const gfxComponents, const Model* const sphereModel)
		:IRenderStage(renderer, gfxComponents)
		,m_boxModel(sphereModel)
{

}

DeferredSimpleLightStage::~DeferredSimpleLightStage()
{

}

void DeferredSimpleLightStage::PreRender()
{
		m_renderData.m_pImmediateContext->RSSetState(m_renderData.m_d3dRasterStateSolCullNone); //We're drawing spheres
		m_renderData.m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		m_renderer->EnableAdditiveBlending();

		//Bind the G-buffer render targets
		for (char rtHandle = 0, textureId = 20; rtHandle < (char)DX11RendererData::GBufferRTType::COUNT; ++rtHandle, ++textureId)
		{
				m_renderer->BindTextureShaderResource(ObjectType::PIXEL_SHADER, textureId, 1, m_renderData.m_GBufferObjHandles[rtHandle]);
		}
}

void DeferredSimpleLightStage::Render(HandleDictionaryVec& graphicsResources, const float dt)
{

		ObjectHandle handle = (graphicsResources[(int)ObjectType::VERTEX_SHADER]).at("defaultVS");
		m_renderer->BindVertexShader(handle);

		handle = (graphicsResources[(int)ObjectType::PIXEL_SHADER]).at("DeferredSimpleLightStagePS");
		m_renderer->BindPixelShader(handle);

		m_renderer->BindVertexBuffer(m_boxModel->GetVBufferHandle(), sizeof(VertexAnimation));
		m_renderer->BindIndexBuffer(m_boxModel->GetIBufferHandle());

		//Update / Set const buffers
		m_renderData.m_pImmediateContext->VSSetConstantBuffers(1, 1, &m_renderData.testViewProjConstBuffer);
		m_renderData.m_pImmediateContext->PSSetConstantBuffers(1, 1, &m_renderData.testViewProjConstBuffer);

		//m_renderer->DrawIndexed(6);
		//
		//return;

		//forward render all of the spheres with light properties
		const auto& lightComponents = (*m_gfxSystemComponents)[ComponentType::RENDERABLE_LIGHT];
		for (auto component : lightComponents)
		{
				if (component->GetIsActive())
				{
						const LightComponent* const lightComp = (LightComponent*)component;
						const auto simpleLight = lightComp->GetLight();

						Transform* const transform = (Transform*)component->GetOwner()->GetComponent(ComponentType::TRANSFORM);
						const auto& translation = transform->GetPosition();

						//Update the per obj. const buffer
						m_renderData.testPerObjectBuffer.worldMtx = transform->GetWorldTransform();
						m_renderData.m_pImmediateContext->UpdateSubresource(m_renderData.testPerObjectConstBuffer,
								0, NULL, &m_renderData.testPerObjectBuffer, 0, 0);

						m_renderData.m_pImmediateContext->VSSetConstantBuffers(0, 1, &m_renderData.testPerObjectConstBuffer);
						m_renderData.m_pImmediateContext->PSSetConstantBuffers(0, 1, &m_renderData.testPerObjectConstBuffer);

						//Bind the view buffer
						m_renderData.m_pImmediateContext->VSSetConstantBuffers(1, 1, &m_renderData.testViewProjConstBuffer);
						m_renderData.m_pImmediateContext->PSSetConstantBuffers(1, 1, &m_renderData.testViewProjConstBuffer);

						//Update the simple light const buffer
						simpleLight->m_position.x = translation.m128_f32[0];
						simpleLight->m_position.y = translation.m128_f32[1];
						simpleLight->m_position.z = translation.m128_f32[2];
						simpleLight->m_range = transform->GetScale().m128_f32[0];

						m_renderData.m_pImmediateContext->UpdateSubresource(m_renderData.testLightNoShadowConstBuffer,
								0, NULL, simpleLight, 0, 0);
						m_renderData.m_pImmediateContext->PSSetConstantBuffers(8, 1, &m_renderData.testLightNoShadowConstBuffer);

						m_renderer->DrawIndexed(m_boxModel->GetIndicesCount());
				}
		}
}

void DeferredSimpleLightStage::PostRender()
{
		//m_renderer->DisableColorBlending();
}
