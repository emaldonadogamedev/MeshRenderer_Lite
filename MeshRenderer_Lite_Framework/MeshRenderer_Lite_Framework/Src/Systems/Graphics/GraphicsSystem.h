#pragma once

#include <Systems/Core/ISystem.h>
#include <memory>

class Camera;
class CameraManager;
class DX11Renderer;
class IRenderComponent;
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

	void Resize(const int w, const int h);

protected:
	void AddRenderStageHelper(IRenderStage* renderStage);

	void TestUpdateCamera(const float dt);

	//Cameras
	std::unique_ptr<Camera> testCamera;

	std::unique_ptr<DX11Renderer> m_dx11Renderer;
	std::vector<IRenderComponent*> m_renderComponents;
	std::vector<IRenderStage*> m_renderStages;
};