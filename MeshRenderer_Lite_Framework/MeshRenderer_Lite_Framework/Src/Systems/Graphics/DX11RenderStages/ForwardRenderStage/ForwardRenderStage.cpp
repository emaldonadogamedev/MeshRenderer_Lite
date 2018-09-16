#include<Utilities/precompiled.h>
#include<Systems/Graphics/DX11RenderStages/ForwardRenderStage/ForwardRenderStage.h>

#include <Systems/Core/Components/Transform/Transform.h>
#include <Systems/Core/GameObject/GameObject.h>
#include<Systems/Graphics/Components/LightComponents/Light.h>
#include<Systems/Graphics/Components/LightComponents/ShadowLightComponent/ShadowLightComponent.h>
#include<Systems/Graphics/Components/ModelComponent/ModelComponent.h>
#include<Systems/Graphics/Components/SimpleCloth/SimpleClothComponent.h>
#include<Systems/Graphics/DX11Renderer/DX11Renderer.h>
#include<Systems/Graphics/DX11Renderer/DX11RendererData.h>
#include<Systems/Graphics/ModelClasses/Model/Model.h>

#include <d3d11.h>

ForwardRenderStage::ForwardRenderStage(DX11Renderer* const renderer, RenderCompUmap* const gfxComponents) 
		:IRenderStage(renderer, gfxComponents)
{

}

ForwardRenderStage::~ForwardRenderStage()
{

}

void ForwardRenderStage::PreRender()
{
	//bind main render target and clear it
	auto& renderData = m_renderer->GetRendererData();
	renderData.m_pImmediateContext->OMSetRenderTargets(1, &renderData.m_pMainRenderTargetView, renderData.m_DepthStencilView);
	renderData.m_pImmediateContext->ClearRenderTargetView(renderData.m_pMainRenderTargetView, renderData.m_clearColor.m128_f32);
	renderData.m_pImmediateContext->ClearDepthStencilView(renderData.m_DepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);
	renderData.m_pImmediateContext->RSSetState(renderData.m_currentRasterState);
	renderData.m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//m_renderer->EnableAlphaBlending();

	static ID3D11SamplerState* const samplerStates[4] = {
		renderData.m_pWrapSamplerState,
		renderData.m_pMirrorSamplerState,
		renderData.m_pClampSamplerState,
		renderData.m_pBorderSamplerState
	};
	renderData.m_pImmediateContext->PSSetSamplers(0, 4, samplerStates);

	renderData.testCamera->Update();
	renderData.testViewProjBuffer.cameraPosition.m128_f32[0] = renderData.testCamera->m_Position.m128_f32[0];
	renderData.testViewProjBuffer.cameraPosition.m128_f32[1] = renderData.testCamera->m_Position.m128_f32[1];
	renderData.testViewProjBuffer.cameraPosition.m128_f32[2] = renderData.testCamera->m_Position.m128_f32[2];
	renderData.testViewProjBuffer.cameraPosition.m128_f32[3] = renderData.m_debugIdx;

	renderData.testViewProjBuffer.viewMtx = renderData.testCamera->GetView();
	renderData.testViewProjBuffer.invViewMtx =
			DirectX::XMMatrixInverse(nullptr, renderData.testViewProjBuffer.viewMtx);
	//m_dx11Renderer->m_renderData->testViewProjBuffer.projectionMtx = testCamera->GetProjection();
	renderData.m_pImmediateContext->UpdateSubresource(renderData.testViewProjConstBuffer,
			0, NULL, &renderData.testViewProjBuffer, 0, 0);

	//Bind all of the shadow maps
	const auto& lightComponents = (*m_gfxSystemComponents)[ComponentType::RENDERABLE_LIGHT];
	for (const auto& component : lightComponents)
	{
			ShadowLightComponent* lightComp = (ShadowLightComponent*)component;
			m_renderer->SetPixelShaderResource(ObjectType::PIXEL_SHADER, lightComp->GetShadowTextureIdx(), 1,
					lightComp->GetShadowRThandle());
	}
}

void ForwardRenderStage::Render(HandleDictionaryVec& graphicsResources, const float dt)
{
	auto& renderData = m_renderer->GetRendererData();

	//Set shaders
	ObjectHandle handle = (graphicsResources[(int)ObjectType::VERTEX_SHADER]).at("defaultVS");
	m_renderer->BindVertexShader(handle);

	if (m_renderer->IsDebugInfoEnabled()) {
		handle = (graphicsResources[(int)ObjectType::PIXEL_SHADER]).at("ShowDebugInfoPS");
		m_renderer->DisableAlphaBlending();
	}
	else if (m_renderer->IsLightingEnabled()) {
		handle = (graphicsResources[(int)ObjectType::PIXEL_SHADER]).at("BRDFLighting");
	}
	else
		handle = (graphicsResources[(int)ObjectType::PIXEL_SHADER]).at("defaultPS");

	m_renderer->BindPixelShader(handle);

	//Update / Set const buffers
	renderData.m_pImmediateContext->VSSetConstantBuffers(1, 1, &renderData.testViewProjConstBuffer);
	renderData.m_pImmediateContext->PSSetConstantBuffers(1, 1, &renderData.testViewProjConstBuffer);

		//update lights const buffer
	static const Light* const sceneLights = ShadowLightComponent::GetSceneLightsWithShadowPtr();
	renderData.m_pImmediateContext->UpdateSubresource(renderData.testLightConstBuffer, 0, nullptr, 
		sceneLights, 0, 0);
	renderData.m_pImmediateContext->PSSetConstantBuffers(5, 1, &renderData.testLightConstBuffer);

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
			transform->UpdateWorldMatrix();
			renderData.testPerObjectBuffer.worldMtx = transform->GetWorldTransform();

			renderData.testPerObjectBuffer.isAnimated = model->m_modelType == ModelType::MODEL_SKINNED;
			renderData.m_pImmediateContext->UpdateSubresource(renderData.testPerObjectConstBuffer,
				0, NULL, &renderData.testPerObjectBuffer, 0, 0);

			renderData.m_pImmediateContext->VSSetConstantBuffers(0, 1, &renderData.testPerObjectConstBuffer);
			renderData.m_pImmediateContext->PSSetConstantBuffers(0, 1, &renderData.testPerObjectConstBuffer);

			//Update bone anim. const buffer
			const int copySize = model->m_boneFinalTransformMtxVec.size();
			const int copyVecLocSize = model->m_boneLocations.size();
			std::memcpy(renderData.testAnimationBuffer.boneMatrices, model->m_boneFinalTransformMtxVec.data(),
				sizeof(XMMATRIX) * (copySize > Model::s_maxBoneCount ? Model::s_maxBoneCount : copySize ));
			std::memcpy(renderData.testAnimationBuffer.boneLocations, model->m_boneLocations.data(),
				sizeof(XMVECTOR) * (copyVecLocSize > Model::s_maxBoneLocCount ? Model::s_maxBoneLocCount : copyVecLocSize) 
								 * (unsigned char)model->m_debugDrawEnabled);

			renderData.m_pImmediateContext->UpdateSubresource(renderData.testAnimationConstBuffer,
				0, NULL, &renderData.testAnimationBuffer, 0, 0);
			renderData.m_pImmediateContext->VSSetConstantBuffers(2, 1, &renderData.testAnimationConstBuffer);

			//Draw each mesh entry, it's all one big VBuffer and IBufer though
			for (auto& meshEntry : model->m_meshEntryList)
			{
				auto& textures2D = graphicsResources.at((int)ObjectType::TEXTURE_2D);

				//Set the diffuse texture
				const auto it = textures2D.find(meshEntry.diffTextureName);
				if (it != textures2D.end()) 
				{
					const auto& diffTextSRV = renderData.textures2D[*it->second].srv;
					renderData.m_pImmediateContext->PSSetShaderResources(0, 1, &diffTextSRV);
				}
				else 
				{
					if (const auto newTexture2D = m_renderer->GetTexture2D("../MeshRenderer_Lite_Framework/Assets/Textures/" + meshEntry.diffTextureName)) {
						renderData.m_pImmediateContext->PSSetShaderResources(0, 1, &renderData.textures2D[*newTexture2D].srv);

						textures2D[meshEntry.diffTextureName] = newTexture2D;
					}
				}

				//Set the normal map
				const auto it_np = textures2D.find(meshEntry.normalMapName);
				if (it_np != textures2D.end()) {
					const auto& diffTextSRV = renderData.textures2D[*it_np->second].srv;
					renderData.m_pImmediateContext->PSSetShaderResources(2, 1, &diffTextSRV);
				}
				else {
					if (const auto newNormalMap = m_renderer->GetTexture2D("../MeshRenderer_Lite_Framework/Assets/Textures/" + meshEntry.normalMapName)) {
						renderData.m_pImmediateContext->PSSetShaderResources(2, 1, &renderData.textures2D[*newNormalMap].srv);

						textures2D[meshEntry.normalMapName] = newNormalMap;
					}
				}

				m_renderer->DrawIndexed(meshEntry.numIndices, meshEntry.baseIndex, meshEntry.baseVertex);
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	//forward render all of cloth objects
	//Set shaders
	handle = (graphicsResources[(int)ObjectType::VERTEX_SHADER]).at("SimpleClothVS");
	m_renderer->BindVertexShader(handle);

	handle = (graphicsResources[(int)ObjectType::PIXEL_SHADER]).at("SimplePS");
	m_renderer->BindPixelShader(handle);

	renderData.testPerObjectBuffer.worldMtx = XMMatrixIdentity();
	renderData.m_pImmediateContext->UpdateSubresource(renderData.testPerObjectConstBuffer,
		0, NULL, &renderData.testPerObjectBuffer, 0, 0);

	renderData.m_pImmediateContext->VSSetConstantBuffers(0, 1, &renderData.testPerObjectConstBuffer);
	renderData.m_pImmediateContext->PSSetConstantBuffers(0, 1, &renderData.testPerObjectConstBuffer);

	const auto& clothComponents = (*m_gfxSystemComponents)[ComponentType::PHYSICS_SIMPLE_CLOTH];
	for (auto component : clothComponents)
	{
		if (component->GetIsActive())
		{
			SimpleClothComponent* clothComp = (SimpleClothComponent*)component;

			m_renderer->BindVertexBuffer(clothComp->m_drawPointsVB, sizeof(VertexAnimation));
			m_renderer->BindIndexBuffer(clothComp->m_drawPointsIB);

			auto& particles = clothComp->particles;
			for (int i = 0; i < particles.size(); ++i)
			{
				renderData.testSimpleClothBuffer.particleData[i].position = particles[i].getPos();
			}

			renderData.m_pImmediateContext->UpdateSubresource(renderData.testSimpleClothConstBuffer,
				0, NULL, &renderData.testSimpleClothBuffer, 0, 0);
			renderData.m_pImmediateContext->VSSetConstantBuffers(4, 1, &renderData.testSimpleClothConstBuffer);

			//m_renderer->Draw(clothComp->particles.size());// clothComp->m_indexCount, 0, 0);
			m_renderer->DrawIndexed(clothComp->m_indexCount, 0, 0);
		}
	}

	////////////////////////////////////////////////////////////////////////////
	////DEBUG!!!
	////Set shaders
	//handle = (graphicsResources[(int)ObjectType::VERTEX_SHADER]).at("AnimationDebugVS");
	//m_renderer->BindVertexShader(handle);
	//handle = (graphicsResources[(int)ObjectType::PIXEL_SHADER]).at("AnimationDebugPS");
	//m_renderer->BindPixelShader(handle);
	//
	//renderData.m_pImmediateContext->VSSetConstantBuffers(1, 1, &renderData.testViewProjConstBuffer);
	//renderData.m_pImmediateContext->VSSetConstantBuffers(2, 1, &renderData.testAnimationConstBuffer);
	//
	//m_renderer->BindNullVertexBuffer();
	//
	//for (const auto* component : modelComponents)
	//{
	//	if (component->GetIsActive())
	//	{
	//		const auto model = (static_cast<const ModelComponent*>(component))->GetModel();
	//		if (model->m_debugDrawEnabled)
	//		{
	//			m_renderer->BindIndexBuffer(model->m_boneLocIndBufferHandle);
	//			m_renderer->DrawIndexed(model->m_boneLocIndBuff.size(), 0, 0);
	//		}
	//	}
	//}
}

void ForwardRenderStage::PostRender()
{
}
