#pragma once

#include <Systems/Graphics/DX11RenderStages/IRenderStage.h>

class GraphicsSystem;
class Model;

class GBufferStage : IRenderStage
{
public:
	GBufferStage(DX11Renderer* const renderer, RenderCompUmap* const gfxComponents, const Model* const boxGeometry);
	virtual ~GBufferStage();

	virtual void PreRender() override;
	virtual void Render(HandleDictionaryVec& graphicsResources, const float dt) override;
	virtual void PostRender() override;

protected:
	const Model* const m_boxModel;
	friend GraphicsSystem;
};