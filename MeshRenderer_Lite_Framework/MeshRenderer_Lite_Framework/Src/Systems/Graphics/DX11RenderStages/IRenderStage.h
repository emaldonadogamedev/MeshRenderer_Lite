#pragma once

#include <vector>

class DX11Renderer;
class IRenderComponent;

class IRenderStage
{
public:
	IRenderStage(DX11Renderer* const rendererData):m_rendererData(rendererData) {}
	virtual ~IRenderStage() {};

	virtual void PreRender() = 0;
	virtual void Render(const std::vector<IRenderComponent*>&) = 0;
	virtual void PostRender() = 0;

protected:
	DX11Renderer* const m_rendererData;
};