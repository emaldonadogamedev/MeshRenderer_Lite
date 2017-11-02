#include<Utilities/precompiled.h>
#include<Systems/Graphics/DX11RenderStages/ForwardRenderStage/ForwardRenderStage.h>

#include<Systems/Graphics/Components/ModelComponent/ModelComponent.h>
#include<Systems/Graphics/DX11Renderer/DX11Renderer.h>
#include<Systems/Graphics/DX11Renderer/DX11RendererData.h>
#include<Systems/Graphics/ModelClasses/Model/Model.h>

#include <d3d11.h>

ForwardRenderStage::ForwardRenderStage(DX11Renderer* const renderData, RenderCompVec* const gfxComponents):
	IRenderStage(renderData, gfxComponents)
{

}

ForwardRenderStage::~ForwardRenderStage()
{

}

void ForwardRenderStage::PreRender()
{
	//bind main render target
	auto& renderData = m_renderer->GetRendererData();
	renderData.m_pImmediateContext->OMSetRenderTargets(1, &renderData.m_pMainRenderTargetView, renderData.m_DepthStencilView);
	m_renderer->ClearBuffer();

}

void ForwardRenderStage::Render(const HandleDictionaryVec& graphicsResources)
{
	auto& renderData = m_renderer->GetRendererData();

	//Set shaders
	const ObjectHandle handle = (graphicsResources[(int)ObjectType::VERTEX_SHADER]).at("defaultVS");
	m_renderer->BindVertexShader(handle);

	renderData.m_pImmediateContext->VSSetConstantBuffers(1, 1, &renderData.testViewProjConstBuffer);
	const auto& modelComponents = m_gfxSystemComponents->at((int)RenderComponentType::RENDERABLE_3D);
	for (const IRenderComponent* component : modelComponents)
	{
		const auto model = (static_cast<const ModelComponent*>(component))->GetModel();
		m_renderer->BindVertexBuffer(model->GetVBufferHandle(), sizeof(VertexAnimation));
		m_renderer->BindIndexBuffer(model->GetIBufferHandle());

		for (auto& meshEntry : model->m_meshEntryList)
		{
			renderData.m_pImmediateContext->VSSetConstantBuffers(0, 1, &renderData.testPerObjectConstBuffer);
			//renderData.m_pImmediateContext->VSSetConstantBuffers(1, 1, &renderData.testViewProjConstBuffer);

			m_renderer->DrawIndexed(meshEntry.numIndices, meshEntry.baseIndex, meshEntry.baseVertex);
		}
	}
}

void ForwardRenderStage::PostRender()
{
}
