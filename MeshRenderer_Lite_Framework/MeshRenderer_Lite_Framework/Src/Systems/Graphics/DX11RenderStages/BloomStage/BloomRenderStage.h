#pragma once

#include <Systems/Graphics/DX11RenderStages/IRenderStage.h>

class GraphicsSystem;

class BloomRenderStage : IRenderStage
{
public:
	BloomRenderStage(DX11Renderer* const renderer, RenderCompUmap* const gfxComponents);
	~BloomRenderStage();

	virtual void PreRender() override;
	virtual void Render(HandleDictionaryVec& graphicsResources, const float dt) override;
	virtual void PostRender() override;

protected:

	ObjectHandle m_bloomTextureHandle;

	friend GraphicsSystem;
};