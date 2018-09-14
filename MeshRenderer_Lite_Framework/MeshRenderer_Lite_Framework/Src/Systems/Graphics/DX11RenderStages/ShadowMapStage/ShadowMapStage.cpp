#include<Utilities/precompiled.h>
#include<Systems/Graphics/DX11RenderStages/ShadowMapStage/ShadowMapStage.h>

#include<Systems/Core/GameObject/GameObject.h>
#include<Systems/Core/Components/Transform/Transform.h>
#include<Systems/Graphics/Components/LightComponent/LightComponent.h>
#include <Systems/Graphics/Components/ModelComponent/ModelComponent.h>
#include <Systems/Graphics/ModelClasses/Model/Model.h>
#include<Systems/Graphics/DX11Renderer/DX11Renderer.h>
#include<Systems/Graphics/DX11Renderer/DX11RendererData.h>
#include <Systems/Graphics/GraphicsUtilities/VertexTypes.h>

using DirectX::XMMatrixTranspose;
using DirectX::XMMatrixLookAtLH;
using DirectX::XMMatrixPerspectiveFovLH;
using DirectX::XMVECTOR;

ShadowMapStage::ShadowMapStage(DX11Renderer* const renderer, RenderCompUmap* const gfxComponents)
	:IRenderStage(renderer, gfxComponents)
{
}

ShadowMapStage::~ShadowMapStage()
{
}

void ShadowMapStage::PreRender()
{
		auto& renderData = m_renderer->GetRendererData();
		renderData.m_pImmediateContext->RSSetState(renderData.m_d3dRasterStateSolCullFront);
		renderData.m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void ShadowMapStage::Render(HandleDictionaryVec& graphicsResources, const float dt)
{
		//Set shaders
		ObjectHandle handle = (graphicsResources[(int)ObjectType::VERTEX_SHADER]).at("ShadowVS");
		m_renderer->BindVertexShader(handle);

		handle = (graphicsResources[(int)ObjectType::PIXEL_SHADER]).at("ShadowPS");
		m_renderer->BindPixelShader(handle);

		auto& renderData = m_renderer->GetRendererData();

		const auto& lightComps = (*m_gfxSystemComponents)[ComponentType::RENDERABLE_LIGHT];
		for (auto& light : lightComps)
		{
				LightComponent* lightComp = (LightComponent*)light;

				if (!lightComp->IsUsingShadows())
						continue;

				const auto& shadowRThandle = lightComp->GetShadowRThandle();
				if (!shadowRThandle)
						continue;

				const auto& shadowRTObj = renderData.renderTargets[*shadowRThandle];

				//Set the shadow texture as the render target, also clear it
				m_renderer->BindRenderTarget(shadowRThandle);
				m_renderer->ClearRenderTarget(shadowRThandle, XMVECTOR());

				D3D11_VIEWPORT shadowViewport { 0,0,shadowRTObj.width, shadowRTObj.height,0,1.0f };
				renderData.m_pImmediateContext->RSSetViewports(1, &shadowViewport);

				//update the view matrix according to the light's position
				renderData.testLightViewBuffer.lightViewMtx = XMMatrixTranspose(
						XMMatrixLookAtLH(
								(static_cast<Transform* const>(lightComp->GetOwner()->GetComponent(ComponentType::TRANSFORM)))->GetPosition(),
								renderData.testCamera->m_LookAt,
								renderData.testCamera->m_Up)
				);

				//update the projection matrix according to the shadow's texture resolution
				renderData.testLightViewBuffer.lightProjectionMtx = XMMatrixTranspose(
						XMMatrixPerspectiveLH(shadowRTObj.width, shadowRTObj.height, renderData.testCamera->m_Near, 
								renderData.testCamera->m_Far)
				);

				renderData.m_pImmediateContext->UpdateSubresource(renderData.testLightViewConstBuffer,
						0, NULL, &renderData.testLightViewBuffer, 0, 0);

				//Update / Set const buffers
				renderData.m_pImmediateContext->VSSetConstantBuffers(5, 1, &renderData.testViewProjConstBuffer);

				//forward render all of the objects
				const auto& modelComponents = (*m_gfxSystemComponents)[ComponentType::RENDERABLE_3D];
				for (auto component : modelComponents)
				{
						if (component->GetIsActive())
						{
								const auto model = (static_cast<const ModelComponent*>(component))->GetModel();

								if (!model->m_drawSkin)
										continue;

								m_renderer->BindVertexBuffer(model->GetVBufferHandle(), sizeof(VertexAnimation));
								m_renderer->BindIndexBuffer(model->GetIBufferHandle());
								

								//Update bone anim. const buffer
								const int copySize = model->m_boneFinalTransformMtxVec.size();
								const int copyVecLocSize = model->m_boneLocations.size();
								std::memcpy(renderData.testAnimationBuffer.boneMatrices, model->m_boneFinalTransformMtxVec.data(),
										sizeof(XMMATRIX) * (copySize > Model::s_maxBoneCount ? Model::s_maxBoneCount : copySize));
								std::memcpy(renderData.testAnimationBuffer.boneLocations, model->m_boneLocations.data(),
										sizeof(XMVECTOR) * (copyVecLocSize > Model::s_maxBoneLocCount ? Model::s_maxBoneLocCount : copyVecLocSize)
										* (unsigned char)model->m_debugDrawEnabled);

								renderData.m_pImmediateContext->UpdateSubresource(renderData.testAnimationConstBuffer,
										0, NULL, &renderData.testAnimationBuffer, 0, 0);
								renderData.m_pImmediateContext->VSSetConstantBuffers(2, 1, &renderData.testAnimationConstBuffer);

								//Get the normal world matrix and update the per object const buffer
								renderData.testPerObjectBuffer.isAnimated = model->m_modelType == ModelType::MODEL_SKINNED;
								auto* const transform = (Transform*)component->GetOwner()->GetComponent(ComponentType::TRANSFORM);
								renderData.testPerObjectBuffer.worldMtx = transform->GetWorldTransform();

								renderData.m_pImmediateContext->UpdateSubresource(renderData.testPerObjectConstBuffer,
										0, NULL, &renderData.testPerObjectBuffer, 0, 0);

								renderData.m_pImmediateContext->VSSetConstantBuffers(0, 1, &renderData.testPerObjectConstBuffer);
								renderData.m_pImmediateContext->PSSetConstantBuffers(0, 1, &renderData.testPerObjectConstBuffer);

								//Draw each mesh entry, it's all one big VBuffer and IBufer though
								for (auto& meshEntry : model->m_meshEntryList)
								{
										m_renderer->DrawIndexed(meshEntry.numIndices, meshEntry.baseIndex, meshEntry.baseVertex);
								}
						}
				}
		}
}

void ShadowMapStage::PostRender()
{
		//put back the viewport the way it was
		auto& renderData = m_renderer->GetRendererData();
		renderData.m_pImmediateContext->RSSetViewports(1, &renderData.m_mainViewport);
}
