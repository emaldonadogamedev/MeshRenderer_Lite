#pragma once

#include <Systems/Graphics/DX11RenderStages/IRenderStage.h>

class GraphicsSystem;

class ForwardRenderStage : IRenderStage
{
public:
	ForwardRenderStage(DX11Renderer* const renderData );
	virtual ~ForwardRenderStage() override;

	virtual void PreRender() override;
	virtual void Render(const std::vector<IRenderComponent*>& modelComponents) override;
	virtual void PostRender() override;

protected:

	friend GraphicsSystem;
};