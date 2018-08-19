#include<Utilities/precompiled.h>
#include<Systems/Graphics/DX11RenderStages/ShadowMapStage/ShadowMapStage.h>
#include <Systems/Graphics/Components/LightComponent/LightComponent.h>

ShadowMapStage::ShadowMapStage(DX11Renderer* const renderer, RenderCompVec* const gfxComponents)
	:IRenderStage(renderer, gfxComponents)
{

}

ShadowMapStage::~ShadowMapStage()
{
}

void ShadowMapStage::PreRender()
{

}

void ShadowMapStage::Render(HandleDictionaryVec& graphicsResources, const float dt)
{
}

void ShadowMapStage::PostRender()
{
}
