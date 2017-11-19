#pragma once

#include <Systems/Graphics/DX11RenderStages/IRenderStage.h>

class GraphicsSystem;

class PathWalkDebugStage : IRenderStage
{
public:
	PathWalkDebugStage(DX11Renderer* const renderData, RenderCompVec* const gfxComponents);
	virtual ~PathWalkDebugStage();

	virtual void PreRender() override;
	virtual void Render(const HandleDictionaryVec& graphicsResources, const float dt) override;
	virtual void PostRender() override;

protected:

	friend GraphicsSystem;
};