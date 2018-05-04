#pragma once

#include <Systems/Graphics/DX11RenderStages/IRenderStage.h>

class GraphicsSystem;

class ForwardRenderStage : IRenderStage
{
public:
	ForwardRenderStage(DX11Renderer* const renderer, RenderCompVec* const gfxComponents);
	virtual ~ForwardRenderStage() override;

	virtual void PreRender() override;
	virtual void Render(const HandleDictionaryVec& graphicsResources, const float dt) override;
	virtual void PostRender() override;

protected:

	friend GraphicsSystem;
};