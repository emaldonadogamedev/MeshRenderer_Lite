#pragma once

#include <Systems/Graphics/DX11RenderStages/IRenderStage.h>

class GraphicsSystem;

class AnimationDebugStage : IRenderStage
{
public:
	AnimationDebugStage(DX11Renderer* const renderData, RenderCompVec* const gfxComponents);
	virtual ~AnimationDebugStage();

	virtual void PreRender() override;
	virtual void Render(const HandleDictionaryVec& graphicsResources) override;
	virtual void PostRender() override;

protected:

	friend GraphicsSystem;
};