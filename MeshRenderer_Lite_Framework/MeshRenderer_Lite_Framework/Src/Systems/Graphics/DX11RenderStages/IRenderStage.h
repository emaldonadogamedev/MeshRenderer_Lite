#pragma once

class DX11RendererData;

class IRenderStage
{
public:
	IRenderStage(DX11RendererData* const rendererData):m_rendererData(rendererData) {}
	virtual ~IRenderStage() {};

	virtual void PreRender() = 0;
	virtual void Render() = 0;
	virtual void PostRender() = 0;

protected:
	DX11RendererData* const m_rendererData;
};