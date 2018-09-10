#pragma once

#include <Systems/Core/ISystem.h>
#include <Systems/Core/Components/IComponent.h>
#include <Systems/Graphics/GraphicsUtilities/ObjectHandle.h>
#include <memory>
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
typedef std::vector<std::vector<IComponent*>> RenderCompVec;
typedef std::unordered_map<ComponentType, std::vector<IComponent*>> RenderCompUmap;

class GraphicsSystem : public ISystem
{
public:
	GraphicsSystem(IApplication* const eng);
	virtual ~GraphicsSystem();

	virtual bool Initialize() override;
	virtual void Update(const float dt) override;
	virtual void Shutdown() override;
	virtual void ReceiveMessage(const IMessage& msg) override;
	virtual void AddComponent(IComponent* component) override;

	void Resize(const int w, const int h);

	//Getters
	DX11Renderer* GetRenderer() const;
	Model* GetModel(const std::string& modelName);

protected:
	//Initialize helpers
	void InitializeImGui();
	void AddRenderStages();
	void AddRenderStageHelper(IRenderStage* const renderStage);
	Model* LoadModelHelper(const std::string& fileName);
	void LoadBasicShaders();
	void LoadBasicShaderHelper(ObjectHandle& shaderHandle, const ObjectType shaderType, const std::string& fileName, 
			const std::string& fileExtension = ".hlsl");
	void LoadPrimitiveShapes();

	//Update helpers
	void TestUpdateCamera(const float dt);
	void UpdateModelComponents(const float dt);
	void UpdateLightComponents(const float dt);
	void UpdateAnimation(Model& model, const float dt);
	void UpdateCurvePathComponents(const float dt);
	void UpdateSimpleCCDComponents(const float dt);
	void UpdateSimpleClothComponents(const float dt);

	std::unique_ptr<DX11Renderer> m_dx11Renderer;
	RenderCompUmap m_renderComponents;
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

	friend IApplication;
};