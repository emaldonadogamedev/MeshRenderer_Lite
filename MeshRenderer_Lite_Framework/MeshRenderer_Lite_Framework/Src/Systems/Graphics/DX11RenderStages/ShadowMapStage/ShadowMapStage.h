#pragma once

#include <Systems/Graphics/DX11RenderStages/IRenderStage.h>

class GraphicsSystem;

class ShadowMapStage : public IRenderStage {

public:
	ShadowMapStage(DX11Renderer* const renderer, RenderCompUmap* const gfxComponents);
	virtual ~ShadowMapStage() override;

	virtual void PreRender() override;
	virtual void Render(HandleDictionaryVec& graphicsResources, const float dt) override;
	virtual void PostRender() override;

protected:
	friend GraphicsSystem;
};