#include <Utilities/precompiled.h>
#include <Systems/Graphics/DX11RenderStages/ForwardRenderStage/ForwardRenderStage.h>

#include <Systems/Graphics/DX11Renderer/DX11Renderer.h>
#include <Systems/Graphics/DX11Renderer/DX11RendererData.h>

ForwardRenderStage::ForwardRenderStage(DX11Renderer* const renderData):
	IRenderStage(renderData)
{

}

ForwardRenderStage::~ForwardRenderStage()
{

}

void ForwardRenderStage::PreRender()
{
}

void ForwardRenderStage::Render(const std::vector<IRenderComponent*>& modelComponents)
{
}

void ForwardRenderStage::PostRender()
{
}
