#pragma once

#include <Systems/Graphics/DX11RenderStages/IRenderStage.h>

class GraphicsSystem;
class Model;

class DeferredSimpleLightStage : IRenderStage
{
	DeferredSimpleLightStage(DX11Renderer* const renderer, RenderCompUmap* const gfxComponents, const Model* const lightGeometry);
	virtual ~DeferredSimpleLightStage();

	virtual void PreRender() override;
	virtual void Render(HandleDictionaryVec& graphicsResources, const float dt) override;
	virtual void PostRender() override;

protected:
	const Model* const m_boxModel;
	
	ID3D11RenderTargetView* m_passRTs[2] = { nullptr, nullptr };

	friend GraphicsSystem;
};