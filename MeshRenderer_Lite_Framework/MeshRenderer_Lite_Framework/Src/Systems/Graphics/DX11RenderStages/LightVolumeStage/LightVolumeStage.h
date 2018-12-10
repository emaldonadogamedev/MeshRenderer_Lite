#pragma once

#include <Systems/Graphics/DX11RenderStages/IRenderStage.h>

class GraphicsSystem;

class LightVolumeStage : public IRenderStage
{

public:
	LightVolumeStage(DX11Renderer* const renderer, RenderCompUmap* const gfxComponents, const ObjectHandle& fsqIndexBuffer);
	virtual ~LightVolumeStage();

	virtual void PreRender() override;
	virtual void Render(HandleDictionaryVec& graphicsResources, const float dt) override;
	virtual void PostRender() override;

protected:
	ObjectHandle m_fsqIndexBuffer;

	friend GraphicsSystem;
};