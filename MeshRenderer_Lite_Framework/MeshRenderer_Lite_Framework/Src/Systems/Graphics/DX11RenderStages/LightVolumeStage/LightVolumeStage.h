#pragma once

#include <Systems/Graphics/DX11RenderStages/IRenderStage.h>

class GraphicsSystem;

class LightVolumeStage : public IRenderStage
{

public:
	LightVolumeStage(DX11Renderer* const renderer, RenderCompUmap* const gfxComponents, const ObjectHandle& quadVertexBuffer);
	virtual ~LightVolumeStage();

	virtual void PreRender() override;
	virtual void Render(HandleDictionaryVec& graphicsResources, const float dt) override;
	virtual void PostRender() override;

protected:
	ObjectHandle m_quadVertexBuffer;

	ObjectHandle m_F_512, m_G_0, m_G_20;

	friend GraphicsSystem;
};