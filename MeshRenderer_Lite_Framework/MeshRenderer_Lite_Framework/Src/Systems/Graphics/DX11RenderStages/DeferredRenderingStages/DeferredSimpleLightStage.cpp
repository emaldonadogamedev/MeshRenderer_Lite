#include <Utilities/precompiled.h>
#include <Systems/Graphics/DX11RenderStages/DeferredRenderingStages/DeferredSimpleLightStage.h>

#include <Systems/Core/Components/Transform/Transform.h>
#include <Systems/Core/GameObject/GameObject.h>
#include<Systems/Graphics/Components/LightComponents/Light.h>
#include<Systems/Graphics/Components/LightComponents/LightComponent/LightComponent.h>
#include<Systems/Graphics/DX11Renderer/DX11Renderer.h>
#include<Systems/Graphics/DX11Renderer/DX11RendererData.h>
#include<Systems/Graphics/ModelClasses/Model/Model.h>

DeferredSimpleLightStage::DeferredSimpleLightStage(DX11Renderer* const renderer, RenderCompUmap* const gfxComponents, 
		const ObjectHandle& fsqIndexBuffer, const Model* const sphereModel)
		:IRenderStage(renderer, gfxComponents)
		,m_fsqIndexBuffer(fsqIndexBuffer)
		,m_sphereModel(sphereModel)
{

}

DeferredSimpleLightStage::~DeferredSimpleLightStage()
{

}

void DeferredSimpleLightStage::PreRender()
{
		//bind main render target and clear it
		m_renderData.m_pImmediateContext->OMSetRenderTargets(1, &m_renderData.m_pMainRenderTargetView, nullptr); //No depth testing required for now
		//am_renderData.m_pImmediateContext->ClearRenderTargetView(m_renderData.m_pMainRenderTargetView, m_renderData.m_clearColor.m128_f32);
		m_renderData.m_pImmediateContext->RSSetState(m_renderData.m_d3dRasterStateSolCullBack); //We're drawing spheres
		m_renderData.m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		m_renderer->EnableAlphaBlending();

		//Bind the G-buffer render targets
		for (char rtHandle = 0, textureId = 20; rtHandle < (char)DX11RendererData::GBufferRTType::COUNT; ++rtHandle, ++textureId)
		{
				m_renderer->BindTextureShaderResource(ObjectType::PIXEL_SHADER, textureId, 1, m_renderData.m_GBufferObjHandles[rtHandle]);
		}

		D3D11_VIEWPORT viewport{ 0,0,m_renderer->GetRenderTargetWidth(), m_renderer->GetRenderTargetHeight(), 0, 1.0f };
		m_renderData.m_pImmediateContext->RSSetViewports(1, &viewport);
}

void DeferredSimpleLightStage::Render(HandleDictionaryVec& graphicsResources, const float dt)
{
		ObjectHandle handle = (graphicsResources[(int)ObjectType::VERTEX_SHADER]).at("defaultVS");
		m_renderer->BindVertexShader(handle);

		handle = (graphicsResources[(int)ObjectType::PIXEL_SHADER]).at("DeferredSimpleLightStagePS");
		m_renderer->BindPixelShader(handle);

		m_renderer->BindVertexBuffer(m_sphereModel->GetVBufferHandle(), sizeof(VertexAnimation));
		m_renderer->BindIndexBuffer(m_sphereModel->GetIBufferHandle());

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

						m_renderData.m_pImmediateContext->UpdateSubresource(m_renderData.testLightNoShadowConstBuffer,
								0, NULL, simpleLight, 0, 0);
						m_renderData.m_pImmediateContext->PSSetConstantBuffers(8, 1, &m_renderData.testLightNoShadowConstBuffer);

						m_renderer->DrawIndexed(m_sphereModel->GetIndicesCount());
				}
		}
}

void DeferredSimpleLightStage::PostRender()
{

}
