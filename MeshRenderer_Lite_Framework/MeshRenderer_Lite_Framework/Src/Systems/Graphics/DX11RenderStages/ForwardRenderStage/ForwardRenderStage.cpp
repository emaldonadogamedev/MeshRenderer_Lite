#include<Utilities/precompiled.h>
#include<Systems/Graphics/DX11RenderStages/ForwardRenderStage/ForwardRenderStage.h>

#include <Systems/Core/Components/Transform/Transform.h>
#include <Systems/Core/GameObject/GameObject.h>
#include<Systems/Graphics/Components/ModelComponent/ModelComponent.h>
#include<Systems/Graphics/DX11Renderer/DX11Renderer.h>
#include<Systems/Graphics/DX11Renderer/DX11RendererData.h>
#include<Systems/Graphics/ModelClasses/Model/Model.h>

#include <d3d11.h>

ForwardRenderStage::ForwardRenderStage(DX11Renderer* const renderData, RenderCompVec* const gfxComponents)
	:IRenderStage(renderData, gfxComponents)
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
	renderData.m_pImmediateContext->RSSetState(renderData.m_d3dRasterStateDefault);

	static ID3D11SamplerState* const samplerStates[4] = {
		renderData.m_pWrapSamplerState,
		renderData.m_pMirrorSamplerState,
		renderData.m_pClampSamplerState,
		renderData.m_pBorderSamplerState
	};
	renderData.m_pImmediateContext->PSSetSamplers(0, 4, samplerStates);
}

void ForwardRenderStage::Render(const HandleDictionaryVec& graphicsResources)
{
	auto& renderData = m_renderer->GetRendererData();

	//Set shaders
	ObjectHandle handle = (graphicsResources[(int)ObjectType::VERTEX_SHADER]).at("defaultVS");
	m_renderer->BindVertexShader(handle);
	handle = (graphicsResources[(int)ObjectType::PIXEL_SHADER]).at("defaultPS");
	m_renderer->BindPixelShader(handle);

	renderData.m_pImmediateContext->VSSetConstantBuffers(1, 1, &renderData.testViewProjConstBuffer);
	renderData.m_pImmediateContext->PSSetConstantBuffers(1, 1, &renderData.testViewProjConstBuffer);

	//forward render all of the objects
	const auto& modelComponents = m_gfxSystemComponents->at(ComponentType::RENDERABLE_3D);
	for (const auto* component : modelComponents)
	{
		if (component->GetIsActive())
		{
			const auto model = (static_cast<const ModelComponent*>(component))->GetModel();

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

			if (!model->m_drawSkin)
				break;

			//Draw each mesh entry, it's all one big VBuffer and IBufer though
			for (auto& meshEntry : model->m_meshEntryList)
			{
				const auto& textures2D = graphicsResources.at((int)ObjectType::TEXTURE_2D);
				const auto it = textures2D.find(meshEntry.diffTextureName);
				if (it != textures2D.end())
				{
					const Texture2D& diffText = renderData.textures2D[*it->second];
					renderData.m_pImmediateContext->PSSetShaderResources(0, 1, &diffText.srv);
				}

				m_renderer->DrawIndexed(meshEntry.numIndices, meshEntry.baseIndex, meshEntry.baseVertex);
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////
	//DEBUG!!!
	//Set shaders
	handle = (graphicsResources[(int)ObjectType::VERTEX_SHADER]).at("AnimationDebugVS");
	m_renderer->BindVertexShader(handle);
	handle = (graphicsResources[(int)ObjectType::PIXEL_SHADER]).at("AnimationDebugPS");
	m_renderer->BindPixelShader(handle);

	renderData.m_pImmediateContext->VSSetConstantBuffers(1, 1, &renderData.testViewProjConstBuffer);
	renderData.m_pImmediateContext->VSSetConstantBuffers(2, 1, &renderData.testAnimationConstBuffer);

	m_renderer->BindNullVertexBuffer();

	for (const auto* component : modelComponents)
	{
		if (component->GetIsActive())
		{
			const auto model = (static_cast<const ModelComponent*>(component))->GetModel();
			if (model->m_debugDrawEnabled)
			{
				m_renderer->BindIndexBuffer(model->m_boneLocIndBufferHandle);
				m_renderer->DrawIndexed(model->m_boneLocIndBuff.size(), 0, 0);
			}
		}
	}
}

void ForwardRenderStage::PostRender()
{
}
