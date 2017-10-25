#pragma once

#include <vector>

class DX11Renderer;
class IRenderComponent;

class IRenderStage
{
public:
	IRenderStage(DX11Renderer* const rendererData, std::vector<IRenderComponent*> * const gfxComponents)
		:m_rendererData(rendererData)
		,m_gfxSystemComponents(gfxComponents) {}
	virtual ~IRenderStage() {};

	virtual void PreRender() = 0;
	virtual void Render() = 0;
	virtual void PostRender() = 0;

protected:
	DX11Renderer* const m_rendererData;
	std::vector<IRenderComponent*>* const m_gfxSystemComponents;
};