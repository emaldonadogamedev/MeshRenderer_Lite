#pragma once

#include <Systems/Core/ISystem.h>
#include <memory>

class DX11Renderer;
class IRenderStage;

class GraphicsSystem : public ISystem
{
public:
	GraphicsSystem(Engine* const eng);
	virtual ~GraphicsSystem();

	virtual bool Initialize() override;
	virtual void Update(const float dt) override;
	virtual void Shutdown() override;
	virtual void ReceiveMessage(const IMessage& msg) override;

protected:
	void AddRenderStageHelper(IRenderStage* renderStage);

	std::unique_ptr<DX11Renderer> m_dx11Renderer;
	std::vector<IRenderStage*> m_renderStages;
};