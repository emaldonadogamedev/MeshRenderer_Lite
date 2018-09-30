#pragma once

#include <Systems/Graphics/DX11RenderStages/IRenderStage.h>

class GraphicsSystem;
class Model;

class DeferredSimpleLightStage : IRenderStage
{
		DeferredSimpleLightStage(DX11Renderer* const renderer, RenderCompUmap* const gfxComponents, 
				const ObjectHandle& fsqIndexBuffer, const Model* const sphereModel);
		virtual ~DeferredSimpleLightStage();

		virtual void PreRender() override;
		virtual void Render(HandleDictionaryVec& graphicsResources, const float dt) override;
		virtual void PostRender() override;

protected:
		ObjectHandle m_fsqIndexBuffer;
		const Model* const m_sphereModel;

		friend GraphicsSystem;
};