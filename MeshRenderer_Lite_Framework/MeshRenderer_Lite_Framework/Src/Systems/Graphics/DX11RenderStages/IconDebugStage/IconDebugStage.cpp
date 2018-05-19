#include<Utilities/precompiled.h>
#include <Systems/Graphics/DX11RenderStages/IconDebugStage/IconDebugStage.h>

IconDebugStage::IconDebugStage(DX11Renderer* const rendererData, RenderCompVec* const gfxComponents)
	:IRenderStage(rendererData, gfxComponents)
{

}

IconDebugStage::~IconDebugStage()
{

}

void IconDebugStage::PreRender()
{
}

void IconDebugStage::Render(HandleDictionaryVec& graphicsResources, const float dt)
{
}

void IconDebugStage::PostRender()
{
}
