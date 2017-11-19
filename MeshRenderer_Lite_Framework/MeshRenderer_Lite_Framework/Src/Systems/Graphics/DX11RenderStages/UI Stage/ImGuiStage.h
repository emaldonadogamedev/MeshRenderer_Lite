#pragma once

#include <Systems/Graphics/DX11RenderStages/IRenderStage.h>

class GraphicsSystem;

class ImGuiStage : public IRenderStage
{
public:
	ImGuiStage(DX11Renderer* const rendererData, RenderCompVec* const gfxComponents);
	~ImGuiStage();


	virtual void PreRender() override;
	virtual void Render(const HandleDictionaryVec& graphicsResources, const float dt) override;
	virtual void PostRender() override;

protected:
};