#include <Utilities/precompiled.h>

#include <Systems/Graphics/GraphicsSystem.h>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/matrix4x4.h>

#include <Engine/IApplication/IApplication.h>
#include <Imgui/imgui.h>
#include <Imgui/imgui_impl_dx11.h>
#include <Systems/Core/Components/Transform/Transform.h>
#include <Systems/Core/GameObject/GameObject.h>
#include <Systems/Graphics/CameraClasses/Camera.h>
#include <Systems/Graphics/Components/ModelComponent/ModelComponent.h>
#include <Systems/Graphics/Components/CurvePathComponent/CurvePathComponent.h>
#include <Systems/Graphics/Components/SimpleCCD/SuperSimpleCCD.h>
#include <Systems/Graphics/Components/SimpleCloth/SimpleClothComponent.h>
#include <Systems/Graphics/DX11Renderer/DX11Renderer.h>
#include <Systems/Graphics/DX11Renderer/DX11RendererData.h>
#include <Systems/Graphics/DX11RenderStages/ForwardDebugStage/PathWalkDebugStage.h>
#include <Systems/Graphics/DX11RenderStages/ForwardRenderStage/ForwardRenderStage.h>
#include <Systems/Graphics/DX11RenderStages/UI Stage/ImGuiStage.h>
#include <Systems/Graphics/ModelClasses/Model/Model.h>
#include <Systems/Graphics/ModelClasses/ModelManager/ModelManager.h>
#include <Systems/Graphics/TextureClasses/TextureManager/TextureManager.h>
#include <Systems/Input/InputSystem.h>
#include <Systems/Input/Keyboard.h>
#include <Systems/Input/Mouse.h>
#include <Systems/Window/WindowSystem.h>

using namespace DirectX;

GraphicsSystem::GraphicsSystem(IApplication* const eng) 
	:ISystem(SystemType::ST_GRAPHICS, eng)
	,m_dx11Renderer(std::make_unique<DX11Renderer>())
	,testCamera(std::make_unique<Camera>())
	,m_modelManager(std::make_unique<ModelManager>(m_dx11Renderer.get()))
	,m_textureManager(std::make_unique<TextureManager>(m_dx11Renderer.get()))
{
	m_renderComponents.resize((size_t)ComponentType::COUNT);
	m_resources.resize((size_t)ObjectType::COUNT);
}

GraphicsSystem::~GraphicsSystem()
{
}

bool GraphicsSystem::Initialize()
{
	const WindowSystem* const window = (WindowSystem*)m_engineOwner->GetSystem(SystemType::ST_WINDOW);

	bool result = m_dx11Renderer->InitializeRenderer(window->GetWindowWidth(), window->GetWindowHeight(), window->GetWindowsHandler());

	//Load resources
	LoadBasicShaders();
	LoadBasicModels();

	AddRenderStages();

	InitializeImGui();

	return result;
}

void GraphicsSystem::Update(const float dt)
{
	//Update components
	UpdateModelComponents(dt);
	UpdateCurvePathComponents(dt);
	UpdateSimpleClothComponents(dt);

	//TEST - Current camera update
	TestUpdateCamera(dt);
	testCamera->Update();
	m_dx11Renderer->m_renderData->testViewProjBuffer.viewMtx = testCamera->GetView();
	m_dx11Renderer->m_renderData->testViewProjBuffer.invViewMtx = 
		DirectX::XMMatrixInverse(nullptr, m_dx11Renderer->m_renderData->testViewProjBuffer.viewMtx);
	//m_dx11Renderer->testViewProjBuffer.projectionMtx = testCamera->GetProjection();
	m_dx11Renderer->m_renderData->m_pImmediateContext->UpdateSubresource(m_dx11Renderer->m_renderData->testViewProjConstBuffer, 
		0, NULL, &m_dx11Renderer->m_renderData->testViewProjBuffer, 0, 0);

	//iterate through all render stages(Including UI)
	for (const auto renderStage : m_renderStages)
	{
		renderStage->PreRender();
		renderStage->Render(m_resources, dt);
		renderStage->PostRender();
	}

	//Finally present image to the screen
	m_dx11Renderer->SwapBuffers();
}

void GraphicsSystem::UpdateModelComponents(const float dt)
{
	auto& modelComponents = m_renderComponents[(int)ComponentType::RENDERABLE_3D];

	for (auto& component : modelComponents)
	{
		auto model = (static_cast<const ModelComponent*>(component))->GetModel();
		
		//Update model running time
		if (model->m_animationEnabled && model->m_ticksPerSecond > 0.f)
		{
			UpdateAnimation(*model, dt);
		}
	}
}

#pragma  region ANIMATION HELPERS
const aiNodeAnim* FindNodeAnim(const aiAnimation* const pAnimation, const string& NodeName)
{
	for (int i = 0; i < pAnimation->mNumChannels; i++) {
		const aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];

		if (string(pNodeAnim->mNodeName.data) == NodeName) {
			return pNodeAnim;
		}
	}

	return NULL;
}

int FindScaling(float AnimationTime, const aiNodeAnim* const pNodeAnim)
{
	assert(pNodeAnim->mNumScalingKeys > 0);

	for (int i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mScalingKeys[i + 1].mTime) {
			return i;
		}
	}

	return pNodeAnim->mNumScalingKeys - 1;

	//assert(0);
	//
	//return 0;
}
void CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* const pNodeAnim)
{
	if (pNodeAnim->mNumScalingKeys == 1) {
		Out = pNodeAnim->mScalingKeys[0].mValue;
		return;
	}

	int ScalingIndex = FindScaling(AnimationTime, pNodeAnim);
    //Hack so the cylinder will loop
	int NextScalingIndex = (ScalingIndex + 1) % pNodeAnim->mNumScalingKeys;
	//assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);
	float DeltaTime = (float)(pNodeAnim->mScalingKeys[NextScalingIndex].mTime - pNodeAnim->mScalingKeys[ScalingIndex].mTime);
	float Factor = std::fabs( (AnimationTime - (float)pNodeAnim->mScalingKeys[ScalingIndex].mTime) / DeltaTime);
	//assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiVector3D& Start = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
	const aiVector3D& End = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
	const aiVector3D Delta = End - Start;
	Out = Start + Factor * Delta;
}

int FindPosition(float AnimationTime, const aiNodeAnim* const pNodeAnim)
{
	for (int i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++) 
	{
		if (AnimationTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime)
			return i;
	}

	return pNodeAnim->mNumPositionKeys - 1;

	//assert(0);
	//
	//return 0;
}
void CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* const pNodeAnim)
{
	if (pNodeAnim->mNumPositionKeys == 1) 
	{
		Out = pNodeAnim->mPositionKeys[0].mValue;
		return;
	}

	int PositionIndex = FindPosition(AnimationTime, pNodeAnim);
    //hack so cylinder will loop
	int NextPositionIndex = (PositionIndex + 1) % pNodeAnim->mNumPositionKeys;
	//assert(NextPositionIndex < pNodeAnim->mNumPositionKeys);
	float DeltaTime = (float)(pNodeAnim->mPositionKeys[NextPositionIndex].mTime - pNodeAnim->mPositionKeys[PositionIndex].mTime);
	float Factor = std::fabs((AnimationTime - (float)pNodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime);
	//assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiVector3D& Start = pNodeAnim->mPositionKeys[PositionIndex].mValue;
	const aiVector3D& End = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
	aiVector3D Delta = End - Start;
	Out = Start + Factor * Delta;
}

int FindRotation(float AnimationTime, const aiNodeAnim* const pNodeAnim)
{
	//assert(pNodeAnim->mNumRotationKeys > 0);

	for (int i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++) 
	{
		if (AnimationTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime) 
			return i;
	}

	return pNodeAnim->mNumRotationKeys - 1;
}
void CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	// we need at least two values to interpolate...
	if (pNodeAnim->mNumRotationKeys == 1) 
	{
		Out = pNodeAnim->mRotationKeys[0].mValue;
		return;
	}

	int RotationIndex = FindRotation(AnimationTime, pNodeAnim);
  //Hack so the cylinder will loop
	int NextRotationIndex = (RotationIndex + 1) % pNodeAnim->mNumRotationKeys;
	//assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
	float DeltaTime = (float)(pNodeAnim->mRotationKeys[NextRotationIndex].mTime - pNodeAnim->mRotationKeys[RotationIndex].mTime);
	float Factor = std::fabs((AnimationTime - (float)pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime);
	//assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
	const aiQuaternion& EndRotationQ = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
	aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);
	Out = Out.Normalize();
}

void ReadNodeHeirarchy(Model& model, const float AnimationTime, const aiNode* pNode, const aiAnimation* currentAnimation ,const aiMatrix4x4& ParentTransform)
{
	const string NodeName(pNode->mName.data);

	aiMatrix4x4 NodeTransformation(pNode->mTransformation);

	//Get the node animation, or determine if this animation uses the node/bone at all
	const aiNodeAnim* const pNodeAnim = FindNodeAnim(currentAnimation, NodeName);

	if (pNodeAnim) 
	{
		// Interpolate scaling and generate scaling transformation matrix
		aiVector3D Scaling;
		CalcInterpolatedScaling(Scaling, AnimationTime, pNodeAnim);
		aiMatrix4x4 assScale;
		aiMatrix4x4::Scaling(Scaling, assScale);

		// Interpolate rotation and generate rotation transformation matrix
		aiQuaternion RotationQ;
		CalcInterpolatedRotation(RotationQ, AnimationTime, pNodeAnim);
		aiMatrix4x4 assOrientation(RotationQ.GetMatrix());

		// Interpolate translation and generate translation transformation matrix
		aiVector3D Translation;
		CalcInterpolatedPosition(Translation, AnimationTime, pNodeAnim);
		aiMatrix4x4 assTrans;
		aiMatrix4x4::Translation(Translation, assTrans);

		// Combine the above transformations
		NodeTransformation = assTrans * assOrientation * assScale;
	}

	const aiMatrix4x4 GlobalTransformation = ParentTransform * NodeTransformation;

	auto it = model.m_boneMapping.find(NodeName);

	if (it != model.m_boneMapping.end()) 
	{
		const unsigned int BoneIndex = it->second;
		aiMatrix4x4 assFinal = model.m_globalInverseTransform * GlobalTransformation;

		model.m_boneLocations[BoneIndex] = XMVectorSet(assFinal.a4, assFinal.b4, assFinal.c4, 1.0f);
		
		assFinal *= model.m_boneOffsetMtxVec[BoneIndex];
		
		
		static XMMATRIX final;
		final.r[0] = XMVectorSet(assFinal.a1, assFinal.a2, assFinal.a3, assFinal.a4);
		final.r[1] = XMVectorSet(assFinal.b1, assFinal.b2, assFinal.b3, assFinal.b4);
		final.r[2] = XMVectorSet(assFinal.c1, assFinal.c2, assFinal.c3, assFinal.c4);
		final.r[3] = XMVectorSet(assFinal.d1, assFinal.d2, assFinal.d3, assFinal.d4);

		model.m_boneFinalTransformMtxVec[BoneIndex] = std::move(final);
	}

	for (int i = 0; i < pNode->mNumChildren; i++) 
		ReadNodeHeirarchy(model, AnimationTime, pNode->mChildren[i], currentAnimation, GlobalTransformation);

}
#pragma endregion

void GraphicsSystem::UpdateAnimation(Model& model, const float dt)
{
	const auto currentAnim = model.m_animations[model.m_currentAnimIndex];
	model.m_runningTime += dt * model.m_ticksPerSecond;
	if (model.m_runningTime >= (float)currentAnim->mDuration)
		model.m_runningTime = 0;

	ReadNodeHeirarchy(model, model.m_runningTime, model.m_assimpScene->mRootNode, currentAnim, aiMatrix4x4());
}

void GraphicsSystem::UpdateCurvePathComponents(const float dt)
{
	auto& pathComponents = m_renderComponents[(int)ComponentType::RENDERABLE_CURVE_PATH];

	for (auto& component : pathComponents)
	{
		if (component->GetIsActive())
		{
			auto path = (static_cast<CurvePathComponent*>(component));
			if (path->m_usePath)
			{
				auto transform = (Transform*)path->GetOwner()->GetComponent(ComponentType::TRANSFORM);
				path->UpdatePath(dt);
				transform->SetPositionn(path->GetCurrentSplinePoint());
				const float angle = path->GetCurrentAngle();
				transform->SetRotationY(angle);
			}
		}
	}
}

void GraphicsSystem::UpdateSimpleCCDComponents(const float dt)
{
	auto& simpleCcdComponents = m_renderComponents[(int)ComponentType::PHYSICS_IK_CCD];

	for (auto& component : simpleCcdComponents)
	{
		if (component->GetIsActive())
		{
			auto ccdComp = (SuperSimpleCCD*)component;
			ccdComp->Update(dt);
		}
	}
}

void GraphicsSystem::UpdateSimpleClothComponents(const float dt)
{
	auto& simpleCcdComponents = m_renderComponents[(int)ComponentType::PHYSICS_SIMPLE_CLOTH];

	for (auto& component : simpleCcdComponents)
	{
		if (component->GetIsActive())
		{
			auto clothComp = (SimpleClothComponent*)component;
			clothComp->addForce(XMVectorSet(0, -9.8, 0, 0) * dt); // add gravity each frame, pointing down
			clothComp->windForce(XMVectorSet(11.5, 2, .2, 0) * dt); // generate some wind each frame

			clothComp->timeStep(dt);
		}
	}
}

void GraphicsSystem::Shutdown()
{
	//Delete all render components
	for (auto& compVec : m_renderComponents)
	{
		for (auto component : compVec)
		{
			SafeDelete(component);
		}
		compVec.clear();
	}
	m_renderComponents.clear();

	//Delete all render stages
	for (const auto* renderStage : m_renderStages)
	{
		SafeDelete(renderStage);
	}
	m_renderStages.clear();

	//Uninitialize D3D
	m_dx11Renderer->ReleaseData();
}

void GraphicsSystem::ReceiveMessage(const IMessage& msg)
{
}

void GraphicsSystem::Resize(const int w, const int h)
{
	const float fov = (float)w / (float)h;
	testCamera->Resize(DirectX::XM_PIDIV4, fov, 0.01f, 1000.0f);
}

const std::unordered_map<std::string, std::unique_ptr<Model>>& GraphicsSystem::GetLoadedModels() const
{
	return m_modelManager->m_loadedModels;
}

void GraphicsSystem::AddComponent(IComponent* component)
{
	if (component)
	{
		m_renderComponents[(int)component->GetComponentType()].emplace_back(component);
	}
}

void GraphicsSystem::InitializeImGui()
{
	const WindowSystem* const window = reinterpret_cast<WindowSystem*>(m_engineOwner->GetSystem(SystemType::ST_WINDOW));

	// Setup ImGui binding
	ImGui_ImplDX11_Init(window->GetWindowsHandler(), m_dx11Renderer->GetRendererData().m_pDevice,
		m_dx11Renderer->GetRendererData().m_pImmediateContext);
}

void GraphicsSystem::AddRenderStages()
{
	//TODO: FINISH THESE
	//Add shadow map stage
	//Add reflection map stage
	AddRenderStageHelper(new ForwardRenderStage(m_dx11Renderer.get(), &m_renderComponents));
	AddRenderStageHelper(new PathWalkDebugStage(m_dx11Renderer.get(), &m_renderComponents));
	AddRenderStageHelper(new ImGuiStage(m_dx11Renderer.get(), &m_renderComponents));
}

void GraphicsSystem::AddRenderStageHelper(IRenderStage* const renderStage)
{
	if (renderStage)
	{
		m_renderStages.emplace_back(std::move(renderStage));
	}
}

void GraphicsSystem::LoadModelHelper(const std::string& fileName)
{
	const auto newModel = m_modelManager->LoadModel(fileName);

	if (!newModel)
		return;

	ObjectHandle textHandle;
	for (auto& meshEntry : newModel->m_meshEntryList)
	{
		if (!meshEntry.diffTextureName.empty())
		{
			m_dx11Renderer->CreateTexture2D(textHandle, m_textureManager->s_textureDir + meshEntry.diffTextureName);
			if (textHandle)
				m_resources[(int)ObjectType::TEXTURE_2D][meshEntry.diffTextureName] = textHandle;
		}
	}
}

void GraphicsSystem::LoadBasicModels()
{
	LoadModelHelper("box.obj");
	LoadModelHelper("bunny.obj");
	LoadModelHelper("sphere.obj");
	LoadModelHelper("dragon.obj");
	LoadModelHelper("Fan.fbx");
	LoadModelHelper("tiny_4anim.x");
	LoadModelHelper("gh_sample_animation.fbx");
	//LoadModelHelper("FiveJointCylinder.fbx"); //doesn't work, creates a scene pointer but does not read mesh data
	LoadModelHelper("cylinder_skellmesh.fbx");
	LoadModelHelper("boblampclean.md5mesh");
}

void GraphicsSystem::LoadBasicShaders()
{
	//////////////////////////////////////////////////////////////////////////
	// Default Vertex Shader
	ObjectHandle vsHandle;
	static const InputLayout defaultVS_inputLayout = 
	{
		InputData("POSITION", DataFormat::FLOAT3, false),
		InputData("NORMAL", DataFormat::FLOAT3, false),
		InputData("TANGENT", DataFormat::FLOAT3, false),
		InputData("BITANGENT", DataFormat::FLOAT3, false),
		InputData("UV", DataFormat::FLOAT2, false),
		InputData("COLOR", DataFormat::FLOAT4, false),
		InputData("BONES", DataFormat::INT4, false),
		InputData("WEIGHTS", DataFormat::FLOAT4, false)
	};

	m_dx11Renderer->CreateVertexShader(vsHandle, s_vertexShaderDir + "defaultVS.hlsl", defaultVS_inputLayout, false);
	m_resources[(int)ObjectType::VERTEX_SHADER]["defaultVS"] = vsHandle;

	vsHandle = ObjectHandle::Null();
	m_dx11Renderer->CreateVertexShader(vsHandle, s_vertexShaderDir + "AnimationDebugVS.hlsl", defaultVS_inputLayout, false);
	m_resources[(int)ObjectType::VERTEX_SHADER]["AnimationDebugVS"] = vsHandle;

	vsHandle = ObjectHandle::Null();
	m_dx11Renderer->CreateVertexShader(vsHandle, s_vertexShaderDir + "SimpleVS.hlsl", defaultVS_inputLayout, false);
	m_resources[(int)ObjectType::VERTEX_SHADER]["SimpleVS"] = vsHandle;

	vsHandle = ObjectHandle::Null();
	m_dx11Renderer->CreateVertexShader(vsHandle, s_vertexShaderDir + "SimpleClothVS.hlsl", defaultVS_inputLayout, false);
	m_resources[(int)ObjectType::VERTEX_SHADER]["SimpleClothVS"] = vsHandle;

	//////////////////////////////////////////////////////////////////////////
	// Default Pixel Shader
	ObjectHandle psHandle;
	m_dx11Renderer->CreatePixelShader(psHandle, s_pixelShaderDir + "defaultPS.hlsl", false);
	m_resources[(int)ObjectType::PIXEL_SHADER]["defaultPS"] = psHandle;
	
	psHandle = ObjectHandle::Null();
	m_dx11Renderer->CreatePixelShader(psHandle, s_pixelShaderDir + "AnimationDebugPS.hlsl", false);
	m_resources[(int)ObjectType::PIXEL_SHADER]["AnimationDebugPS"] = psHandle;

	psHandle = ObjectHandle::Null();
	m_dx11Renderer->CreatePixelShader(psHandle, s_pixelShaderDir + "SimplePS.hlsl", false);
	m_resources[(int)ObjectType::PIXEL_SHADER]["SimplePS"] = psHandle;

}

void GraphicsSystem::TestUpdateCamera(const float dt)
{
	const InputSystem* const input = reinterpret_cast<InputSystem*>(m_engineOwner->GetSystem(SystemType::ST_INPUT));

	if (input->m_keyboard->IsKeyHeld(KeyboardEvent::VirtualKey::KEY_W))
	{
		testCamera->Walk(dt * 10);
	}

	if (input->m_keyboard->IsKeyHeld(KeyboardEvent::VirtualKey::KEY_S))
	{
		testCamera->Walk(-dt * 10);
	}

	if (input->m_keyboard->IsKeyHeld(KeyboardEvent::VirtualKey::KEY_A))
	{
		testCamera->Strafe(-dt * 10);
	}

	if (input->m_keyboard->IsKeyHeld(KeyboardEvent::VirtualKey::KEY_D))
	{
		testCamera->Strafe(dt * 10);
	}

	if (input->m_keyboard->IsKeyHeld(KeyboardEvent::VirtualKey::KEY_Q))
	{
		testCamera->Elevate(dt * 10);
	}

	if (input->m_keyboard->IsKeyHeld(KeyboardEvent::VirtualKey::KEY_E))
	{
		testCamera->Elevate(-dt * 10);
	}
}

const string GraphicsSystem::s_shaderDir = "../MeshRenderer_Lite_Framework/Assets/Shaders/";
const string GraphicsSystem::s_vertexShaderDir = "../MeshRenderer_Lite_Framework/Assets/Shaders/VertexShaders/";
const string GraphicsSystem::s_pixelShaderDir = "../MeshRenderer_Lite_Framework/Assets/Shaders/PixelShaders/";
const string GraphicsSystem::s_textureDir = "../MeshRenderer_Lite_Framework/Assets/Shaders/";