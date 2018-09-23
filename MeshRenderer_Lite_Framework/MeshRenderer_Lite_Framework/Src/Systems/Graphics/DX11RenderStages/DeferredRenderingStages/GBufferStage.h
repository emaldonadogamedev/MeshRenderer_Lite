#pragma once

#include <Systems/Graphics/DX11RenderStages/IRenderStage.h>

class GraphicsSystem;

class GBufferStage : IRenderStage
{
public:
		GBufferStage(DX11Renderer* const renderer, RenderCompUmap* const gfxComponents);
		virtual ~GBufferStage();

		virtual void PreRender() override;
		virtual void Render(HandleDictionaryVec& graphicsResources, const float dt) override;
		virtual void PostRender() override;

protected:

		friend GraphicsSystem;
};