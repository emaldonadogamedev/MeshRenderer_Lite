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
	//bind main render target and clear it
	auto& renderData = m_renderer->GetRendererData();
	renderData.m_pImmediateContext->OMSetRenderTargets(1, &renderData.m_pMainRenderTargetView, renderData.m_DepthStencilView);
	renderData.m_pImmediateContext->ClearRenderTargetView(renderData.m_pMainRenderTargetView, renderData.m_clearColor.m128_f32);
	renderData.m_pImmediateContext->ClearDepthStencilView(renderData.m_DepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);
	renderData.m_pImmediateContext->RSSetState(renderData.m_d3dRasterStateDefault);

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
	const auto& modelComponents = m_gfxSystemComponents->at((int)RenderComponentType::RENDERABLE_3D);
	for (const IRenderComponent* component : modelComponents)
	{
		const auto model = (static_cast<const ModelComponent*>(component))->GetModel();
		m_renderer->BindVertexBuffer(model->GetVBufferHandle(), sizeof(VertexAnimation));
		m_renderer->BindIndexBuffer(model->GetIBufferHandle());

		//TODO:
		//UPDATE Per Object Const Buffer with current component transform
		// before we bind the it to the shaders

		renderData.m_pImmediateContext->VSSetConstantBuffers(0, 1, &renderData.testPerObjectConstBuffer);
		renderData.m_pImmediateContext->PSSetConstantBuffers(0, 1, &renderData.testPerObjectConstBuffer);

		for (auto& meshEntry : model->m_meshEntryList)
		{
			m_renderer->DrawIndexed(meshEntry.numIndices, meshEntry.baseIndex, meshEntry.baseVertex);
		}
	}
}

void ForwardRenderStage::PostRender()
{
}
