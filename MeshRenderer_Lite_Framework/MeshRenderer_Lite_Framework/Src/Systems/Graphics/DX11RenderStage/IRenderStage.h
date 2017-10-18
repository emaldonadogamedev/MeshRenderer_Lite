#pragma once

class DX11RendererData;

class IRenderStage
{
public:
	IRenderStage(const DX11RendererData* const rendererData):m_rendererData(rendererData) {}
	virtual ~IRenderStage() = 0;

	virtual void PreRender() = 0;
	virtual void Render() = 0;
	virtual void PostRender() = 0;

protected:
	const DX11RendererData* const m_rendererData;
};