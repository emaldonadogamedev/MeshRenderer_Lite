#pragma once

#include <Systems/Core/ISystem.h>
#include <Systems/Graphics/Components/IRenderComponent.h>
#include <Systems/Graphics/GraphicsUtilities/ObjectHandle.h>
#include <memory>
#include <array>
#include <unordered_map>

class Model;
class Camera;
class CameraManager;
class DX11Renderer;
class IRenderStage;
class ModelManager;
class TextureManager;

//typedefs
typedef std::unordered_map<string, ObjectHandle> HandleDictionary;
typedef std::vector<HandleDictionary>  HandleDictionaryVec;
typedef std::vector<std::vector<IRenderComponent*>> RenderCompVec;

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
	//Initialize helpers
	void InitializeImGui();
	void AddRenderStages();
	void AddRenderStageHelper(IRenderStage* const renderStage);
	void LoadBasicModels();
	void LoadShadersShaders();

	//Update helpers
	void TestUpdateCamera(const float dt);
	void UpdateModelComponents(const float dt);
	void UpdateAnimation(Model& model, const float dt);

	//Cameras
	std::unique_ptr<Camera> testCamera;

	std::unique_ptr<DX11Renderer> m_dx11Renderer;
	RenderCompVec m_renderComponents;
	std::vector<IRenderStage*> m_renderStages;

	std::unique_ptr<ModelManager> m_modelManager;
	std::unique_ptr<TextureManager> m_textureManager;

	//Resource Collection
	HandleDictionaryVec m_resources;

	//static variables
	//directories
	static const string s_shaderDir;
	static const string s_vertexShaderDir;
	static const string s_pixelShaderDir;
	static const string s_textureDir;

	friend Engine;
};