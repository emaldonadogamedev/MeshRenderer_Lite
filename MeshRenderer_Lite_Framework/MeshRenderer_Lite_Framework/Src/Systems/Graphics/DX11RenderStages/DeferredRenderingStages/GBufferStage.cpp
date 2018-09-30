#include <Utilities/precompiled.h>
#include <Systems/Graphics/DX11RenderStages/DeferredRenderingStages/GBufferStage.h>

#include<Systems/Core/Components/Transform/Transform.h>
#include<Systems/Core/GameObject/GameObject.h>

#include<Systems/Graphics/Components/ModelComponent/ModelComponent.h>
#include<Systems/Graphics/DX11Renderer/DX11Renderer.h>
#include<Systems/Graphics/DX11Renderer/DX11RendererData.h>
#include<Systems/Graphics/ModelClasses/Model/Model.h>

GBufferStage::GBufferStage(DX11Renderer* const renderer, RenderCompUmap* const gfxComponents)
		:IRenderStage(renderer, gfxComponents)
{
}

GBufferStage::~GBufferStage()
{
}

void GBufferStage::PreRender()
{
		m_renderData.m_pImmediateContext->OMSetRenderTargets((int)DX11RendererData::GBufferRTType::COUNT, m_renderData.m_pGbufferRTVs,
				m_renderData.m_DepthStencilView);

		static const auto blackClearColor = XMVectorSet(0,0,0,1.0f);
		for (unsigned char i = 0; i < (unsigned char)DX11RendererData::GBufferRTType::COUNT; ++i)
		{
				m_renderData.m_pImmediateContext->ClearRenderTargetView(m_renderData.m_pGbufferRTVs[i], blackClearColor.m128_f32);
		}
		m_renderData.m_pImmediateContext->ClearDepthStencilView(m_renderData.m_DepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);
		m_renderData.m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		m_renderData.m_pImmediateContext->RSSetState(m_renderData.m_d3dRasterStateDefault);
		m_renderer->DisableAlphaBlending();
}

void GBufferStage::Render(HandleDictionaryVec& graphicsResources, const float dt)
{
		//Set shaders
		ObjectHandle handle = (graphicsResources[(int)ObjectType::VERTEX_SHADER]).at("defaultVS");
		m_renderer->BindVertexShader(handle);

		handle = (graphicsResources[(int)ObjectType::PIXEL_SHADER]).at("GbufferPS");
		m_renderer->BindPixelShader(handle);

		//Update / Set const buffers
		m_renderData.m_pImmediateContext->VSSetConstantBuffers(1, 1, &m_renderData.testViewProjConstBuffer);

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

						auto* const transform = (Transform*)component->GetOwner()->GetComponent(ComponentType::TRANSFORM);
						m_renderData.testPerObjectBuffer.worldMtx = transform->GetWorldTransform();

						m_renderData.testPerObjectBuffer.isAnimated = model->m_modelType == ModelType::MODEL_SKINNED;
						m_renderData.m_pImmediateContext->UpdateSubresource(m_renderData.testPerObjectConstBuffer,
								0, NULL, &m_renderData.testPerObjectBuffer, 0, 0);

						m_renderData.m_pImmediateContext->VSSetConstantBuffers(0, 1, &m_renderData.testPerObjectConstBuffer);
						m_renderData.m_pImmediateContext->PSSetConstantBuffers(0, 1, &m_renderData.testPerObjectConstBuffer);

						//Update bone animation const buffer
						const int copySize = model->m_boneFinalTransformMtxVec.size();
						const int copyVecLocSize = model->m_boneLocations.size();
						std::memcpy(m_renderData.testAnimationBuffer.boneMatrices, model->m_boneFinalTransformMtxVec.data(),
								sizeof(XMMATRIX) * (copySize > Model::s_maxBoneCount ? Model::s_maxBoneCount : copySize));
						std::memcpy(m_renderData.testAnimationBuffer.boneLocations, model->m_boneLocations.data(),
								sizeof(XMVECTOR) * (copyVecLocSize > Model::s_maxBoneLocCount ? Model::s_maxBoneLocCount : copyVecLocSize)
								* (unsigned char)model->m_debugDrawEnabled);

						m_renderData.m_pImmediateContext->UpdateSubresource(m_renderData.testAnimationConstBuffer,
								0, NULL, &m_renderData.testAnimationBuffer, 0, 0);
						m_renderData.m_pImmediateContext->VSSetConstantBuffers(2, 1, &m_renderData.testAnimationConstBuffer);

						//Draw each mesh entry, it's all one big VBuffer and IBufer though
						for (auto& meshEntry : model->m_meshEntryList)
						{
								if (meshEntry.meshMaterial.m_materialProperties.useAlphaBlending)
										continue;

								//Set the material information
								m_renderData.testMeshMaterialBuffer = meshEntry.meshMaterial;
								m_renderData.m_pImmediateContext->UpdateSubresource(m_renderData.testMeshMaterialConstBuffer,
										0, NULL, &m_renderData.testMeshMaterialBuffer.m_materialProperties, 0, 0);
								m_renderData.m_pImmediateContext->PSSetConstantBuffers(6, 1, &m_renderData.testMeshMaterialConstBuffer);

								auto& textures2D = graphicsResources.at((int)ObjectType::TEXTURE_2D);
								//Set the diffuse texture
								if (meshEntry.meshMaterial.m_materialProperties.useDiffuseTexture)
								{
										const auto it = textures2D.find(meshEntry.diffTextureName);
										if (it != textures2D.end())
										{
												const auto& diffTextSRV = m_renderData.textures2D[*it->second].srv;
												m_renderData.m_pImmediateContext->PSSetShaderResources(0, 1, &diffTextSRV);
										}
										else
										{
												if (const auto newTexture2D = m_renderer->GetTexture2D("../MeshRenderer_Lite_Framework/Assets/Textures/" + meshEntry.diffTextureName))
												{
														m_renderData.m_pImmediateContext->PSSetShaderResources(0, 1, &m_renderData.textures2D[*newTexture2D].srv);
														textures2D[meshEntry.diffTextureName] = newTexture2D;
												}
										}
								}

								//Set the normal map
								if (meshEntry.meshMaterial.m_materialProperties.useNormalMap)
								{
										const auto it_np = textures2D.find(meshEntry.normalMapName);
										if (it_np != textures2D.end()) {
												const auto& diffTextSRV = m_renderData.textures2D[*it_np->second].srv;
												m_renderData.m_pImmediateContext->PSSetShaderResources(2, 1, &diffTextSRV);
										}
										else {
												if (const auto newNormalMap = m_renderer->GetTexture2D("../MeshRenderer_Lite_Framework/Assets/Textures/" + meshEntry.normalMapName)) {
														m_renderData.m_pImmediateContext->PSSetShaderResources(2, 1, &m_renderData.textures2D[*newNormalMap].srv);

														textures2D[meshEntry.normalMapName] = newNormalMap;
												}
										}
								}

								m_renderer->DrawIndexed(meshEntry.numIndices, meshEntry.baseIndex, meshEntry.baseVertex);
						}
				}
		}
}

void GBufferStage::PostRender()
{
}
