#include<Utilities/precompiled.h>
#include<Systems/Graphics/DX11RenderStages/ShadowMapStage/ShadowMapStage.h>

ShadowMapStage::ShadowMapStage(DX11Renderer* const renderer, RenderCompVec* const gfxComponents)
	:IRenderStage(renderer, gfxComponents)
{

}

ShadowMapStage::~ShadowMapStage()
{

}

void ShadowMapStage::PreRender()
{
	throw std::logic_error("The method or operation is not implemented.");
}

void ShadowMapStage::Render(const HandleDictionaryVec& graphicsResources, const float dt)
{
	throw std::logic_error("The method or operation is not implemented.");
}

void ShadowMapStage::PostRender()
{
	throw std::logic_error("The method or operation is not implemented.");
}
