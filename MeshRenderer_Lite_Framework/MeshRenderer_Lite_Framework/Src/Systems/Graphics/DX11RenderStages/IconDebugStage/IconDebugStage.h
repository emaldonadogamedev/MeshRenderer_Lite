#pragma once

#include <Systems/Graphics/DX11RenderStages/IRenderStage.h>

class IconDebugStage : public IRenderStage 
{
public:
	IconDebugStage(DX11Renderer* const rendererData, RenderCompVec* const gfxComponents);
	virtual ~IconDebugStage();

	virtual void PreRender() override;
	virtual void Render(const HandleDictionaryVec& graphicsResources, const float dt) override;
	virtual void PostRender() override;

protected:
};