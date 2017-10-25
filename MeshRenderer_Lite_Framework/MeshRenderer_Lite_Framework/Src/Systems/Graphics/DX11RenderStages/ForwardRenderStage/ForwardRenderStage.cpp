#include <Utilities/precompiled.h>
#include <Systems/Graphics/DX11RenderStages/ForwardRenderStage/ForwardRenderStage.h>

#include <Systems/Graphics/DX11Renderer/DX11Renderer.h>
#include <Systems/Graphics/DX11Renderer/DX11RendererData.h>

ForwardRenderStage::ForwardRenderStage(DX11Renderer* const renderData, std::vector<IRenderComponent*> * const gfxComponents):
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

void ForwardRenderStage::Render()
{
}

void ForwardRenderStage::PostRender()
{
}
