#pragma once

#include <Systems/Graphics/DX11RenderStages/IRenderStage.h>

class GraphicsSystem;

class DeferredShadowLightStage : IRenderStage
{
		DeferredShadowLightStage(DX11Renderer* const renderer, RenderCompUmap* const gfxComponents, const ObjectHandle& fsqIndexBuffer);
		virtual ~DeferredShadowLightStage();

		virtual void PreRender() override;
		virtual void Render(HandleDictionaryVec& graphicsResources, const float dt) override;
		virtual void PostRender() override;

protected:
		ObjectHandle m_fsqIndexBuffer;

		friend GraphicsSystem;
};