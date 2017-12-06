#include <Utilities/precompiled.h>
#include <Systems/Graphics/DX11RenderStages/ForwardDebugStage/PathWalkDebugStage.h>

#include <Systems/Core/Components/Transform/Transform.h>
#include <Systems/Core/GameObject/GameObject.h>
#include<Systems/Graphics/Components/CurvePathComponent/CurvePathComponent.h>
#include<Systems/Graphics/Components/ModelComponent/ModelComponent.h>
#include<Systems/Graphics/DX11Renderer/DX11Renderer.h>
#include<Systems/Graphics/DX11Renderer/DX11RendererData.h>

PathWalkDebugStage::PathWalkDebugStage(DX11Renderer* const renderData, RenderCompVec* const gfxComponents)
	:IRenderStage(renderData, gfxComponents)
{

}

PathWalkDebugStage::~PathWalkDebugStage()
{

}

void PathWalkDebugStage::PreRender()
{
	
}

void PathWalkDebugStage::Render(const HandleDictionaryVec& graphicsResources, const float dt)
{
	auto& renderData = m_renderer->GetRendererData();
	renderData.m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);

	//Set shaders
	ObjectHandle handle = (graphicsResources[(int)ObjectType::VERTEX_SHADER]).at("SimpleVS");
	m_renderer->BindVertexShader(handle);
	handle = (graphicsResources[(int)ObjectType::PIXEL_SHADER]).at("SimplePS");
	m_renderer->BindPixelShader(handle);

	//forward render all of the objects
	const auto& pathComponents = m_gfxSystemComponents->at(ComponentType::RENDERABLE_CURVE_PATH);
	for (const auto* component : pathComponents)
	{
		if (component->GetIsActive())
		{
			const auto path = static_cast<const CurvePathComponent*>(component);
			renderData.testPerObjectBuffer.worldMtx = XMMatrixTranspose(XMMatrixTranslationFromVector(path->m_pathCenterPos));

			renderData.m_pImmediateContext->UpdateSubresource(renderData.testPerObjectConstBuffer,
				0, NULL, &renderData.testPerObjectBuffer, 0, 0);

			m_renderer->BindVertexBuffer(path->GetPathVBuffer(), sizeof(VertexAnimation));
			m_renderer->Draw(path->GetPathVertexCount(), 0);
		}
	}
}

void PathWalkDebugStage::PostRender()
{

}
