#pragma once

#include <Systems/Graphics/DX11RenderStages/IRenderStage.h>

class GraphicsSystem;

class PathWalkDebugStage : IRenderStage
{
public:
	PathWalkDebugStage(DX11Renderer* const renderData, RenderCompUmap* const gfxComponents);
	virtual ~PathWalkDebugStage();

	virtual void PreRender() override;
	virtual void Render(HandleDictionaryVec& graphicsResources, const float dt) override;
	virtual void PostRender() override;

protected:

	friend GraphicsSystem;
};