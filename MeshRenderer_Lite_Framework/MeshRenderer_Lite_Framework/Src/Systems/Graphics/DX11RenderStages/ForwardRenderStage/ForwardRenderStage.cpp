#include <Utilities/precompiled.h>
#include <Systems/Graphics/DX11RenderStages/ForwardRenderStage/ForwardRenderStage.h>

#include <Systems/Graphics/DX11Renderer/DX11Renderer.h>
#include <Systems/Graphics/DX11Renderer/DX11RendererData.h>

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
	//todo:
	//bind main render target
}

void ForwardRenderStage::Render(const HandleDictionaryVec& graphicsResources)
{
	auto& renderData = m_renderer->GetRendererData();

	//TODO:
	//For all models
		//forward render model

	renderData.m_pImmediateContext->VSSetConstantBuffers(0, 1, &renderData.testPerObjectConstBuffer);
	//renderData.m_pImmediateContext->VSSetConstantBuffers(1, 1, &renderData.testViewProjConstBuffer);
	renderData.m_pImmediateContext->PSSetShader(renderData.testPixelShader, NULL, 0);

	m_renderer->Draw(3, 0);
}

void ForwardRenderStage::PostRender()
{
}
