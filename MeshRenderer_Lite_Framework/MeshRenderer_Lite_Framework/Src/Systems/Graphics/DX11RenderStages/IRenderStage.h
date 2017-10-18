#pragma once

#include <vector>

class DX11RendererData;
class IRenderComponent;

class IRenderStage
{
public:
	IRenderStage(DX11RendererData* const rendererData):m_rendererData(rendererData) {}
	virtual ~IRenderStage() {};

	virtual void PreRender() = 0;
	virtual void Render(const std::vector<IRenderComponent*>&) = 0;
	virtual void PostRender() = 0;

protected:
	DX11RendererData* const m_rendererData;
};