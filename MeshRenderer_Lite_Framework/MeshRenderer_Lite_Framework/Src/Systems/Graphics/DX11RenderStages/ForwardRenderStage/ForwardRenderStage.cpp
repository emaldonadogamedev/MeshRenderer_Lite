#include <Utilities/precompiled.h>
#include <Systems/Graphics/DX11RenderStages/ForwardRenderStage/ForwardRenderStage.h>

ForwardRenderStage::ForwardRenderStage(DX11RendererData* const renderData):
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
