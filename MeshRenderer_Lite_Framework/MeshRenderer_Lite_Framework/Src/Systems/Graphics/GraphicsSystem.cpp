#include <Utilities/precompiled.h>

#include <Systems/Graphics/GraphicsSystem.h>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/matrix4x4.h>

#include <Engine/Engine.h>
#include <Imgui/imgui.h>
#include <Imgui/imgui_impl_dx11.h>
#include <Systems/Graphics/CameraClasses/Camera.h>
#include <Systems/Graphics/Components/ModelComponent/ModelComponent.h>
#include <Systems/Graphics/DX11Renderer/DX11Renderer.h>
#include <Systems/Graphics/DX11Renderer/DX11RendererData.h>
#include <Systems/Graphics/DX11RenderStages/ForwardRenderStage/ForwardRenderStage.h>
#include <Systems/Graphics/DX11RenderStages/UI Stage/ImGuiStage.h>
#include <Systems/Graphics/ModelClasses/Model/Model.h>
#include <Systems/Graphics/ModelClasses/ModelManager/ModelManager.h>
#include <Systems/Input/InputSystem.h>
#include <Systems/Input/Keyboard.h>
#include <Systems/Input/Mouse.h>
#include <Systems/Window/WindowSystem.h>

GraphicsSystem::GraphicsSystem(Engine* const eng) 
	:ISystem(eng)
	,m_dx11Renderer(std::make_unique<DX11Renderer>())
	,testCamera(std::make_unique<Camera>())
	,m_modelManager(std::make_unique<ModelManager>(m_dx11Renderer.get()))
{
	m_renderComponents.resize((size_t)RenderComponentType::COUNT);
	m_resources.resize((size_t)ObjectType::COUNT);
}

GraphicsSystem::~GraphicsSystem()
{
}

bool GraphicsSystem::Initialize()
{
	const WindowSystem* const window = (WindowSystem*)m_engineOwner->GetSystem("Windows");

	bool result = m_dx11Renderer->InitializeRenderer(window->GetWindowWidth(), window->GetWindowHeight(), window->GetWindowsHandler());

	//Load resources
	LoadShadersShaders();
	LoadBasicModels();

	ModelComponent* test3DComp = new ModelComponent(nullptr);
	test3DComp->SetModel(m_modelManager->GetModel("gh_sample_animation.fbx"));
	//test3DComp->SetModel(m_modelManager->GetModel("simpleMan2.6.fbx"));
	m_renderComponents[(char)RenderComponentType::RENDERABLE_3D].emplace_back(std::move(test3DComp));

	AddRenderStages();

	InitializeImGui();

	return result;
}

void GraphicsSystem::Update(const float dt)
{
	UpdateModelComponents(dt);

	//TEST!!!
	TestUpdateCamera(dt);
	testCamera->Update();
	m_dx11Renderer->m_renderData->testViewProjBuffer.viewMtx = testCamera->GetView();
	//m_dx11Renderer->testViewProjBuffer.projectionMtx = testCamera->GetProjection();
	m_dx11Renderer->m_renderData->m_pImmediateContext->UpdateSubresource(m_dx11Renderer->m_renderData->testViewProjConstBuffer, 
		0, NULL, &m_dx11Renderer->m_renderData->testViewProjBuffer, 0, 0);

	for (const auto renderStage : m_renderStages)
	{
		renderStage->PreRender();
		renderStage->Render(m_resources);
		renderStage->PostRender();
	}

	m_dx11Renderer->SwapBuffers();
}

void GraphicsSystem::UpdateModelComponents(const float dt)
{
	auto& modelComponents = m_renderComponents.at((int)RenderComponentType::RENDERABLE_3D);

	for (auto& component : modelComponents)
	{
		auto model = (static_cast<const ModelComponent*>(component))->GetModel();
		
		//Update model running time 
		if (model->m_animationEnabled)
		{
			UpdateAnimation(*model, dt);
		}
	}
}

#pragma  region ANIMATION HELPERS
const aiNodeAnim* FindNodeAnim(const aiAnimation* pAnimation, const string NodeName)
{
	for (int i = 0; i < pAnimation->mNumChannels; i++) {
		const aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];

		if (string(pNodeAnim->mNodeName.data) == NodeName) {
			return pNodeAnim;
		}
	}

	return NULL;
}

int FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	assert(pNodeAnim->mNumScalingKeys > 0);

	for (int i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mScalingKeys[i + 1].mTime) {
			return i;
		}
	}

	assert(0);

	return 0;
}
void CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	if (pNodeAnim->mNumScalingKeys == 1) {
		Out = pNodeAnim->mScalingKeys[0].mValue;
		return;
	}

	int ScalingIndex = FindScaling(AnimationTime, pNodeAnim);
	int NextScalingIndex = (ScalingIndex + 1);
	assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);
	float DeltaTime = (float)(pNodeAnim->mScalingKeys[NextScalingIndex].mTime - pNodeAnim->mScalingKeys[ScalingIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mScalingKeys[ScalingIndex].mTime) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiVector3D& Start = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
	const aiVector3D& End = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
	aiVector3D Delta = End - Start;
	Out = Start + Factor * Delta;
}

int FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	for (int i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++) 
	{
		if (AnimationTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime)
			return i;
	}

	assert(0);

	return 0;
}
void CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	if (pNodeAnim->mNumPositionKeys == 1) 
	{
		Out = pNodeAnim->mPositionKeys[0].mValue;
		return;
	}

	int PositionIndex = FindPosition(AnimationTime, pNodeAnim);
	int NextPositionIndex = (PositionIndex + 1);
	assert(NextPositionIndex < pNodeAnim->mNumPositionKeys);
	float DeltaTime = (float)(pNodeAnim->mPositionKeys[NextPositionIndex].mTime - pNodeAnim->mPositionKeys[PositionIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiVector3D& Start = pNodeAnim->mPositionKeys[PositionIndex].mValue;
	const aiVector3D& End = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
	aiVector3D Delta = End - Start;
	Out = Start + Factor * Delta;
}

int FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	assert(pNodeAnim->mNumRotationKeys > 0);

	for (int i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++) 
	{
		if (AnimationTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime) 
			return i;
	}

	assert(0);

	return 0;
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
	int NextRotationIndex = (RotationIndex + 1);
	assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
	float DeltaTime = (float)(pNodeAnim->mRotationKeys[NextRotationIndex].mTime - pNodeAnim->mRotationKeys[RotationIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
	const aiQuaternion& EndRotationQ = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
	aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);
	Out = Out.Normalize();
}

void ReadNodeHeirarchy(Model& model, float AnimationTime, const aiNode* pNode, const aiAnimation* currentAnimation ,const XMMATRIX& ParentTransform)
{
	const string NodeName(pNode->mName.data);

	XMMATRIX NodeTransformation(&pNode->mTransformation.a1);

	const aiNodeAnim* const pNodeAnim = FindNodeAnim(currentAnimation, NodeName);

	if (pNodeAnim) 
	{
		// Interpolate scaling and generate scaling transformation matrix
		aiVector3D Scaling;
		CalcInterpolatedScaling(Scaling, AnimationTime, pNodeAnim);
		XMMATRIX ScalingM = DirectX::XMMatrixScaling(Scaling.x, Scaling.y, Scaling.z);

		// Interpolate rotation and generate rotation transformation matrix
		aiQuaternion RotationQ;
		CalcInterpolatedRotation(RotationQ, AnimationTime, pNodeAnim);
		aiMatrix3x3 ai3x3 = RotationQ.GetMatrix();
		XMMATRIX RotationM = XMMatrixIdentity();
		RotationM.r[0] = DirectX::XMVectorSet(ai3x3.a1, ai3x3.a2, ai3x3.a3, 0);
		RotationM.r[1] = DirectX::XMVectorSet(ai3x3.b1, ai3x3.b2, ai3x3.b3, 0);
		RotationM.r[2] = DirectX::XMVectorSet(ai3x3.c1, ai3x3.c2, ai3x3.c3, 0);

		// Interpolate translation and generate translation transformation matrix
		aiVector3D Translation;
		CalcInterpolatedPosition(Translation, AnimationTime, pNodeAnim);
		const XMMATRIX TranslationM = DirectX::XMMatrixTranslation(Translation.x, Translation.y, Translation.z);

		// Combine the above transformations
		NodeTransformation = ScalingM * RotationM * TranslationM;
	}

	const XMMATRIX GlobalTransformation = NodeTransformation * ParentTransform;
	
	auto it = model.m_boneMapping.find(NodeName);

	if (it != model.m_boneMapping.end()) 
	{
		const unsigned int BoneIndex = it->second;
		model.m_boneFinalTransformMtxVec[BoneIndex] = 
			//DirectX::XMMatrixTranspose(model.m_globalInverseTransform * GlobalTransformation * model.m_boneMatrices[BoneIndex].offsetMatrix);
			DirectX::XMMatrixTranspose(model.m_boneOffsetMtxVec[BoneIndex] *  GlobalTransformation * model.m_globalInverseTransform);
	}

	for (int i = 0; i < pNode->mNumChildren; i++) 
	{
		ReadNodeHeirarchy(model, AnimationTime, pNode->mChildren[i], currentAnimation, GlobalTransformation);
	}
}
#pragma endregion

void GraphicsSystem::UpdateAnimation(Model& model, const float dt)
{
	model.m_runningTime += dt;
	auto currentAnim = model.m_animations[model.m_currentAnimName];
	if (model.m_runningTime > (float)currentAnim->mDuration)
		model.m_runningTime = 0;

	const XMMATRIX identity = XMMatrixIdentity();

	float TicksPerSecond = (float)(currentAnim->mTicksPerSecond != 0 ? currentAnim->mTicksPerSecond : 25.0f);
	float TimeInTicks = model.m_runningTime * TicksPerSecond;
	float AnimationTime = fmod(TimeInTicks, (float)currentAnim->mDuration);

	ReadNodeHeirarchy(model, AnimationTime, model.m_assimpScene->mRootNode, currentAnim, identity);
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

void GraphicsSystem::InitializeImGui()
{
	const WindowSystem* window = reinterpret_cast<WindowSystem*>(m_engineOwner->GetSystem("Windows"));

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
	AddRenderStageHelper(new ImGuiStage(m_dx11Renderer.get(), &m_renderComponents));

}

void GraphicsSystem::AddRenderStageHelper(IRenderStage* const renderStage)
{
	if (renderStage)
	{
		m_renderStages.emplace_back(std::move(renderStage));
	}
}

void GraphicsSystem::LoadBasicModels()
{
	m_modelManager->LoadModel("box.obj");
	m_modelManager->LoadModel("sphere.obj");
	m_modelManager->LoadModel("spider.obj");
	m_modelManager->LoadModel("simpleMan2.6.fbx");
	m_modelManager->LoadModel("gh_sample_animation.fbx");
}

void GraphicsSystem::LoadShadersShaders()
{
	//////////////////////////////////////////////////////////////////////////
	// Default Vertex Shader
	ObjectHandle defaultVS;
	static const InputLayout defaultVS_inputLayout = {
		InputData("POSITION", DataFormat::FLOAT3, false),
		InputData("NORMAL", DataFormat::FLOAT3, false),
		InputData("TANGENT", DataFormat::FLOAT3, false),
		InputData("BITANGENT", DataFormat::FLOAT3, false),
		InputData("UV", DataFormat::FLOAT2, false),
		InputData("COLOR", DataFormat::FLOAT4, false),
		InputData("BONES", DataFormat::INT4, false),
		InputData("WEIGHTS", DataFormat::FLOAT4, false)
	};

	m_dx11Renderer->CreateVertexShader(defaultVS, s_vertexShaderDir + "defaultVS.hlsl", defaultVS_inputLayout, false);
	m_resources[(int)ObjectType::VERTEX_SHADER]["defaultVS"] = defaultVS;

	//////////////////////////////////////////////////////////////////////////
	// Default Pixel Shader
	ObjectHandle defaultPS;
	m_dx11Renderer->CreatePixelShader(defaultPS, s_pixelShaderDir + "defaultPS.hlsl", false);
	m_resources[(int)ObjectType::PIXEL_SHADER]["defaultPS"] = defaultPS;
}

void GraphicsSystem::TestUpdateCamera(const float dt)
{
	const InputSystem* const input = reinterpret_cast<InputSystem*>(m_engineOwner->GetSystem("Input"));

	if (input->m_keyboard->IsKeyHeld(KeyboardEvent::VirtualKey::KEY_W))
	{
		testCamera->Walk(dt);
	}

	if (input->m_keyboard->IsKeyHeld(KeyboardEvent::VirtualKey::KEY_S))
	{
		testCamera->Walk(-dt);
	}

	if (input->m_keyboard->IsKeyHeld(KeyboardEvent::VirtualKey::KEY_A))
	{
		testCamera->Strafe(dt);
	}

	if (input->m_keyboard->IsKeyHeld(KeyboardEvent::VirtualKey::KEY_D))
	{
		testCamera->Strafe(-dt);
	}

	if (input->m_keyboard->IsKeyHeld(KeyboardEvent::VirtualKey::KEY_Q))
	{
		testCamera->Elevate(dt);
	}

	if (input->m_keyboard->IsKeyHeld(KeyboardEvent::VirtualKey::KEY_E))
	{
		testCamera->Elevate(-dt);
	}
}

const string GraphicsSystem::s_shaderDir = "../MeshRenderer_Lite_Framework/Assets/Shaders/";
const string GraphicsSystem::s_vertexShaderDir = "../MeshRenderer_Lite_Framework/Assets/Shaders/VertexShaders/";
const string GraphicsSystem::s_pixelShaderDir = "../MeshRenderer_Lite_Framework/Assets/Shaders/PixelShaders/";
const string GraphicsSystem::s_textureDir = "../MeshRenderer_Lite_Framework/Assets/Shaders/";