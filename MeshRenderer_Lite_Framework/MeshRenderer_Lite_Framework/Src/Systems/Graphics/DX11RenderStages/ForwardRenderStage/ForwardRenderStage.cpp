#include<Utilities/precompiled.h>
#include<Systems/Graphics/DX11RenderStages/ForwardRenderStage/ForwardRenderStage.h>

#include <Systems/Core/Components/Transform/Transform.h>
#include <Systems/Core/GameObject/GameObject.h>
#include<Systems/Graphics/Components/ModelComponent/ModelComponent.h>
#include<Systems/Graphics/Components/SimpleCCD/SuperSimpleCCD.h>
#include<Systems/Graphics/Components/SimpleCloth/SimpleClothComponent.h>
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
	renderData.m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	static ID3D11SamplerState* const samplerStates[4] = {
		renderData.m_pWrapSamplerState,
		renderData.m_pMirrorSamplerState,
		renderData.m_pClampSamplerState,
		renderData.m_pBorderSamplerState
	};
	renderData.m_pImmediateContext->PSSetSamplers(0, 4, samplerStates);
}

void ForwardRenderStage::Render(const HandleDictionaryVec& graphicsResources, const float dt)
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
	const auto& modelComponents = (*m_gfxSystemComponents)[(int)ComponentType::RENDERABLE_3D];
	for (auto component : modelComponents)
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
	//forward render all of the simple CCD
	const auto& ccdComponents = (*m_gfxSystemComponents)[(int)ComponentType::PHYSICS_IK_CCD];

	for (auto component : ccdComponents)
	{
		if (component->GetIsActive())
		{
			SuperSimpleCCD* ccdComp = (SuperSimpleCCD*)component;

			m_renderer->BindNullVertexBuffer();
			m_renderer->BindIndexBuffer(ccdComp->m_drawPoints_IB);

			//Set shaders
			handle = (graphicsResources[(int)ObjectType::VERTEX_SHADER]).at("SimpleCCD_VS");
			m_renderer->BindVertexShader(handle);

			handle = (graphicsResources[(int)ObjectType::PIXEL_SHADER]).at("SimplePS");
			m_renderer->BindPixelShader(handle);

			for (int i = 0; i < SuperSimpleCCD::s_MADE_UP_JOINT_AMOUNT; ++i)
			{
				renderData.testSimpleCCDBuffer.jointPositions[i].m128_f32[0] = ccdComp->m_jointPositions[i].x;
				renderData.testSimpleCCDBuffer.jointPositions[i].m128_f32[1] = ccdComp->m_jointPositions[i].y;
				renderData.testSimpleCCDBuffer.jointPositions[i].m128_f32[2] = ccdComp->m_jointPositions[i].z;
			}

			renderData.m_pImmediateContext->UpdateSubresource(renderData.testSimpleCCD_ConstBuffer,
				0, NULL, &renderData.testSimpleCCDBuffer, 0, 0);
			renderData.m_pImmediateContext->VSSetConstantBuffers(5, 1, &renderData.testSimpleCCD_ConstBuffer);

			renderData.m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
			m_renderer->DrawIndexed(SuperSimpleCCD::s_MADE_UP_JOINT_AMOUNT, 0, 0);

			renderData.m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
			m_renderer->DrawIndexed(SuperSimpleCCD::s_MADE_UP_JOINT_AMOUNT, 0, 0);

			////////////////////////////////////////////////////////////////////////////
			////Now draw the "target" and the end effector...
			m_renderer->BindVertexBuffer(ccdComp->m_drawBoxTargetVB, sizeof(VertexAnimation));
			m_renderer->BindIndexBuffer(ccdComp->m_drawBoxTargetIB);
			
			handle = (graphicsResources[(int)ObjectType::VERTEX_SHADER]).at("SimpleVS");
			m_renderer->BindVertexShader(handle);
			
			renderData.testPerObjectBuffer.isAnimated = false;
			renderData.testPerObjectBuffer.color = XMVectorSet(1, 1, 1, 1);
			renderData.testPerObjectBuffer.worldMtx = XMMatrixTranspose(
				XMMatrixTranslation(ccdComp->m_targetPos.x, ccdComp->m_targetPos.y, ccdComp->m_targetPos.z) *
				XMMatrixScaling(0.4, 0.4, 0.4)
			);
			renderData.m_pImmediateContext->UpdateSubresource(renderData.testPerObjectConstBuffer,
				0, NULL, &renderData.testPerObjectBuffer, 0, 0);
			
			renderData.m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			renderData.m_pImmediateContext->VSSetConstantBuffers(0, 1, &renderData.testPerObjectConstBuffer);
			m_renderer->DrawIndexed(36, 0, 0);

			//Now draw the enf-effector
			renderData.testPerObjectBuffer.color = XMVectorSet(1, 0, 0, 1);
			renderData.testPerObjectBuffer.worldMtx = XMMatrixTranspose(
				XMMatrixTranslation(ccdComp->m_endEffectorPos.x, ccdComp->m_endEffectorPos.y, ccdComp->m_endEffectorPos.z) *
				XMMatrixScaling(0.4, 0.4, 0.4)
			);
			renderData.m_pImmediateContext->UpdateSubresource(renderData.testPerObjectConstBuffer,
				0, NULL, &renderData.testPerObjectBuffer, 0, 0);
			renderData.m_pImmediateContext->PSSetConstantBuffers(0, 1, &renderData.testPerObjectConstBuffer);
			m_renderer->DrawIndexed(36, 0, 0);
		}
	}

	//////////////////////////////////////////////////////////////////////////
	//forward render all of cloth objects
	renderData.m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	const auto& clothComponents = (*m_gfxSystemComponents)[(int)ComponentType::PHYSICS_SIMPLE_CLOTH];
	for (auto component : clothComponents)
	{
		if (component->GetIsActive())
		{
			SimpleClothComponent* clothComp = (SimpleClothComponent*)component;

			//Set shaders
			handle = (graphicsResources[(int)ObjectType::VERTEX_SHADER]).at("SimpleClothVS");
			m_renderer->BindVertexShader(handle);

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

			//////////////////////////////////////////////////////////////////////////
			//Now draw the "fan"

			m_renderer->BindVertexBuffer(clothComp->m_drawFanVB, sizeof(VertexAnimation));
			m_renderer->BindIndexBuffer(clothComp->m_drawFanIB);

			handle = (graphicsResources[(int)ObjectType::VERTEX_SHADER]).at("SimpleVS");
			m_renderer->BindVertexShader(handle);

			renderData.testPerObjectBuffer.worldMtx = XMMatrixTranspose( XMMatrixTranslationFromVector(clothComp->m_fanPos));
			renderData.m_pImmediateContext->UpdateSubresource(renderData.testPerObjectConstBuffer,
				0, NULL, &renderData.testPerObjectBuffer, 0, 0);

			renderData.m_pImmediateContext->VSSetConstantBuffers(0, 1, &renderData.testPerObjectConstBuffer);
			m_renderer->DrawIndexed(36, 0, 0);
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
