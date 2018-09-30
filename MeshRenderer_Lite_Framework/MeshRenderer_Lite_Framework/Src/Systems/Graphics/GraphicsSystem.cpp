#include <Utilities/precompiled.h>

#include <Systems/Graphics/GraphicsSystem.h>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/matrix4x4.h>

#include <Engine/Applications/IApplication/IApplication.h>
#include <Imgui/imgui.h>
#include <Imgui/imgui_impl_dx11.h>
#include <Systems/Core/Components/Transform/Transform.h>
#include <Systems/Core/GameObject/GameObject.h>
#include <Systems/Graphics/CameraClasses/Camera.h>
#include <Systems/Graphics/Components/ModelComponent/ModelComponent.h>
#include <Systems/Graphics/Components/CurvePathComponent/CurvePathComponent.h>
#include <Systems/Graphics/Components/LightComponents/Light.h>
#include <Systems/Graphics/Components/LightComponents/ShadowLightComponent/ShadowLightComponent.h>
#include <Systems/Graphics/Components/SimpleCCD/SuperSimpleCCD.h>
#include <Systems/Graphics/Components/SimpleCloth/SimpleClothComponent.h>
#include <Systems/Graphics/DX11Renderer/DX11Renderer.h>
#include <Systems/Graphics/DX11Renderer/DX11RendererData.h>
#include <Systems/Graphics/DX11RenderStages/ForwardDebugStage/PathWalkDebugStage.h>
#include <Systems/Graphics/DX11RenderStages/ForwardRenderStage/ForwardRenderStage.h>
#include <Systems/Graphics/DX11RenderStages/DeferredRenderingStages/AmbientLightStage.h>
#include <Systems/Graphics/DX11RenderStages/DeferredRenderingStages/DeferredShadowLightStage.h>
#include <Systems/Graphics/DX11RenderStages/DeferredRenderingStages/DeferredSimpleLightStage.h>
#include <Systems/Graphics/DX11RenderStages/DeferredRenderingStages/GBufferStage.h>
#include <Systems/Graphics/DX11RenderStages/ShadowMapStage/ShadowMapStage.h>
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
	,m_modelManager(std::make_unique<ModelManager>(m_dx11Renderer.get()))
	,m_textureManager(std::make_unique<TextureManager>(m_dx11Renderer.get()))
{
	//m_renderComponents.resize((size_t)ComponentType::COUNT);
	m_resources.resize((size_t)ObjectType::COUNT);
}

GraphicsSystem::~GraphicsSystem()
{
}

bool GraphicsSystem::Initialize()
{
	const WindowSystem* const window = (WindowSystem*)m_engineOwner->GetSystem(SystemType::ST_WINDOW);

	bool result = m_dx11Renderer->InitializeRenderer(window->GetWindowWidth(), window->GetWindowHeight(), window->GetWindowsHandler());

	if (result) {
		//Load resources
		LoadPrimitiveShapes();
		LoadBasicShaders();
		AddRenderStages();
		InitializeImGui();
	}

	return result;
}

void GraphicsSystem::Update(const float dt)
{
	//Update components
	UpdateModelComponents(dt);
	UpdateCurvePathComponents(dt);
	UpdateSimpleClothComponents(dt);
	UpdateLightComponents(dt);

	//TEST - Current camera update
	TestUpdateCamera(dt);

	//iterate through all render stages(Including UI)
	for (const auto renderStage : m_renderStages)
	{
			if (renderStage->GetIsActive())
			{
					renderStage->PreRender();
					renderStage->Render(m_resources, dt);
					renderStage->PostRender();
			}
	}

	//Finally present image to the screen
	m_dx11Renderer->SwapBuffers();
}

void GraphicsSystem::UpdateModelComponents(const float dt)
{
	auto& modelComponents = m_renderComponents[ComponentType::RENDERABLE_3D];

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

void GraphicsSystem::UpdateLightComponents(const float dt)
{
	auto& lightComponents = m_renderComponents[ComponentType::RENDERABLE_LIGHT];
	auto& shadowLightComponents = m_renderComponents[ComponentType::RENDERABLE_LIGHT_WITH_SHADOW];

	for (auto& component : lightComponents)
	{
		auto light = (static_cast<const LightComponent*>(component))->GetLight();
		auto& lightTransform = (static_cast<Transform* const>(component->GetOwner()->GetComponent(ComponentType::TRANSFORM)))->GetPosition();
		light->m_position.x = lightTransform.m128_f32[0];
		light->m_position.y = lightTransform.m128_f32[1];
		light->m_position.z = lightTransform.m128_f32[2];
	}

	for (auto& component : shadowLightComponents)
	{
			auto light = (static_cast<const ShadowLightComponent*>(component))->GetLight();
			auto& lightTransform = (static_cast<Transform* const>(component->GetOwner()->GetComponent(ComponentType::TRANSFORM)))->GetPosition();
			light->m_position.x = lightTransform.m128_f32[0];
			light->m_position.y = lightTransform.m128_f32[1];
			light->m_position.z = lightTransform.m128_f32[2];
	}

	auto& renderData = m_dx11Renderer->GetRendererData();
	renderData.m_pImmediateContext->UpdateSubresource(renderData.testLightWithShadowConstBuffer,
			0, NULL, ShadowLightComponent::GetSceneLightsWithShadowPtr(), 0, 0);
}

#pragma  region ANIMATION HELPERS
const aiNodeAnim* FindNodeAnim(const aiAnimation* const pAnimation, const string& NodeName)
{
	const aiNodeAnim* pNodeAnim;
	for (int i = 0; i < pAnimation->mNumChannels; i++) 
	{
		pNodeAnim = pAnimation->mChannels[i];

		if (string(pNodeAnim->mNodeName.data) == NodeName) 
		{
			return pNodeAnim;
		}
	}

	return nullptr;
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

int FindPosition(const float AnimationTime, const aiNodeAnim* const pNodeAnim)
{
	for (int i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++) 
	{
		if (AnimationTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime)
			return i;
	}

	return pNodeAnim->mNumPositionKeys - 1;

}
void CalcInterpolatedPosition(aiVector3D& Out, const float AnimationTime, const aiNodeAnim* const pNodeAnim)
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

	Out = Start + Factor * (End - Start);
}

int FindRotation(const float AnimationTime, const aiNodeAnim* const pNodeAnim)
{
	//assert(pNodeAnim->mNumRotationKeys > 0);

	for (int i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++) 
	{
		if (AnimationTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime) 
			return i;
	}

	return pNodeAnim->mNumRotationKeys - 1;
}
void CalcInterpolatedRotation(aiQuaternion& Out, const float AnimationTime, const aiNodeAnim* pNodeAnim)
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

void ReadNodeHeirarchy(Model& model, const float AnimationTime, const aiNode* const pNode, const aiAnimation* const currentAnimation ,const aiMatrix4x4& ParentTransform)
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

		//model.m_boneLocations[BoneIndex] = XMVectorSet(assFinal.a4, assFinal.b4, assFinal.c4, 1.0f);
		auto& currBoneLoc = model.m_boneLocations[BoneIndex];
		currBoneLoc.m128_f32[0] = assFinal.a4;
		currBoneLoc.m128_f32[1] = assFinal.b4;
		currBoneLoc.m128_f32[2] = assFinal.c4;
		//currBoneLoc.m128_f32[3] = 1.0f; //not needed

		assFinal *= model.m_boneOffsetMtxVec[BoneIndex];
		
		
		static XMMATRIX final;
		final.r[0] = XMVectorSet(assFinal.a1, assFinal.a2, assFinal.a3, assFinal.a4);
		final.r[1] = XMVectorSet(assFinal.b1, assFinal.b2, assFinal.b3, assFinal.b4);
		final.r[2] = XMVectorSet(assFinal.c1, assFinal.c2, assFinal.c3, assFinal.c4);
		final.r[3] = XMVectorSet(assFinal.d1, assFinal.d2, assFinal.d3, assFinal.d4);

		model.m_boneFinalTransformMtxVec[BoneIndex] = final;
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
	auto& pathComponents = m_renderComponents[ComponentType::RENDERABLE_CURVE_PATH];

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
	auto& simpleCcdComponents = m_renderComponents[ComponentType::PHYSICS_IK_CCD];

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
	auto& simpleCcdComponents = m_renderComponents[ComponentType::PHYSICS_SIMPLE_CLOTH];

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
		for (auto component : compVec.second)
		{
			SafeDelete(component);
		}
		compVec.second.clear();
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
	m_dx11Renderer->ResizeBuffers(w, h);
	m_dx11Renderer->m_renderData->testCamera->Resize(DirectX::XM_PIDIV4, (float)w / (float)h, 0.01f, 1000.0f);

	auto& projMtx = m_dx11Renderer->m_renderData->testCamera->GetProjection();
	m_dx11Renderer->GetRendererData().testViewProjBuffer.projectionMtx = projMtx;
	m_dx11Renderer->GetRendererData().testViewProjBuffer.invProjectionMtx = DirectX::XMMatrixInverse(nullptr, projMtx);
}

void GraphicsSystem::AddComponent(IComponent* component)
{
	if (component)
	{
		m_renderComponents[component->GetComponentType()].emplace_back(component);

		//If it's a light component, create the shadow map
		if (component->GetComponentType() == ComponentType::RENDERABLE_LIGHT_WITH_SHADOW)
		{
				ShadowLightComponent* shadowLightComp = (ShadowLightComponent*)component;
				m_dx11Renderer->CreateRenderTarget(shadowLightComp->GetShadowRThandle(), shadowLightComp->m_shadowMapWidthHeight,
						shadowLightComp->m_shadowMapWidthHeight, DataFormat::FLOAT4);

				auto& renderData = m_dx11Renderer->GetRendererData();

				auto srv = renderData.renderTargets[*shadowLightComp->GetShadowRThandle()].srv;
				renderData.m_pImmediateContext->PSSetShaderResources(shadowLightComp->GetShadowTextureIdx(), 1, &srv);
		}
	}
}

DX11Renderer* GraphicsSystem::GetRenderer() const
{
	return m_dx11Renderer.get();
}

Model* GraphicsSystem::GetModel(const std::string& modelName)
{
	return LoadModelHelper(modelName);
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
	AddRenderStageHelper(new ShadowMapStage(m_dx11Renderer.get(), &m_renderComponents));
	//TODO: Add reflection map stage
	AddRenderStageHelper(new GBufferStage(m_dx11Renderer.get(), &m_renderComponents));

	const Model* const quadModel = GetModel("quad");
	const Model* const sphereModel = GetModel("sphere");
	AddRenderStageHelper(new AmbientLightStage(m_dx11Renderer.get(), &m_renderComponents, quadModel->GetIBufferHandle()));
	AddRenderStageHelper(new DeferredShadowLightStage(m_dx11Renderer.get(), &m_renderComponents, quadModel->GetIBufferHandle()));
	AddRenderStageHelper(new DeferredSimpleLightStage(m_dx11Renderer.get(), &m_renderComponents, quadModel->GetIBufferHandle(), 
			sphereModel));

	AddRenderStageHelper(new ForwardRenderStage(m_dx11Renderer.get(), &m_renderComponents), false);
	AddRenderStageHelper(new PathWalkDebugStage(m_dx11Renderer.get(), &m_renderComponents), false);
	AddRenderStageHelper(new ImGuiStage(m_dx11Renderer.get(), &m_renderComponents));
}

void GraphicsSystem::AddRenderStageHelper(IRenderStage* const renderStage, const bool isActive)
{
	if (renderStage)
	{
		m_renderStages.emplace_back(std::move(renderStage));
		renderStage->SetIsActive(isActive);
	}
}

Model* GraphicsSystem::LoadModelHelper(const std::string& fileName)
{
	const auto newModel = m_modelManager->LoadModel(fileName);

	if (!newModel)
		return nullptr;

	for (auto& meshEntry : newModel->m_meshEntryList)
	{
		if (!meshEntry.diffTextureName.empty())
		{
			ObjectHandle textHandle;
			m_dx11Renderer->CreateTexture2D(textHandle, m_textureManager->s_textureDir + meshEntry.diffTextureName);
			if (textHandle)
				m_resources[(int)ObjectType::TEXTURE_2D][meshEntry.diffTextureName] = textHandle;
		}
	}

	return newModel;
}

void GraphicsSystem::LoadBasicShaders()
{
	ObjectHandle shaderHandle;
	//////////////////////////////////////////////////////////////////////////
	// Default Vertex Shaders
	LoadBasicShaderHelper(shaderHandle, ObjectType::VERTEX_SHADER, "defaultVS");
	LoadBasicShaderHelper(shaderHandle, ObjectType::VERTEX_SHADER, "AnimationDebugVS");
	LoadBasicShaderHelper(shaderHandle, ObjectType::VERTEX_SHADER, "SimpleVS");
	LoadBasicShaderHelper(shaderHandle, ObjectType::VERTEX_SHADER, "SimpleClothVS");
	LoadBasicShaderHelper(shaderHandle, ObjectType::VERTEX_SHADER, "ShadowVS");
	LoadBasicShaderHelper(shaderHandle, ObjectType::VERTEX_SHADER, "FullScreenQuadVS");

	//////////////////////////////////////////////////////////////////////////
	// Default Pixel Shaders
	LoadBasicShaderHelper(shaderHandle, ObjectType::PIXEL_SHADER, "defaultPS");
	LoadBasicShaderHelper(shaderHandle, ObjectType::PIXEL_SHADER, "phongLighting");
	LoadBasicShaderHelper(shaderHandle, ObjectType::PIXEL_SHADER, "BRDFLighting");
	LoadBasicShaderHelper(shaderHandle, ObjectType::PIXEL_SHADER, "ShowDebugInfoPS");
	LoadBasicShaderHelper(shaderHandle, ObjectType::PIXEL_SHADER, "SimplePS");
	LoadBasicShaderHelper(shaderHandle, ObjectType::PIXEL_SHADER, "DepthPS");
	LoadBasicShaderHelper(shaderHandle, ObjectType::PIXEL_SHADER, "ShadowPS");
	LoadBasicShaderHelper(shaderHandle, ObjectType::PIXEL_SHADER, "GbufferPS");
	LoadBasicShaderHelper(shaderHandle, ObjectType::PIXEL_SHADER, "DeferredAmbientStagePS");
	LoadBasicShaderHelper(shaderHandle, ObjectType::PIXEL_SHADER, "DeferredShadowLightStagePS");
	LoadBasicShaderHelper(shaderHandle, ObjectType::PIXEL_SHADER, "DeferredSimpleLightStagePS");
}

void GraphicsSystem::LoadBasicShaderHelper(ObjectHandle& shaderHandle, const ObjectType shaderType, const std::string & fileName, const std::string & fileExtension)
{
		if (shaderType == ObjectType::VERTEX_SHADER)
		{
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

				shaderHandle.MakeNull();
				m_dx11Renderer->CreateVertexShader(shaderHandle, s_vertexShaderDir + fileName + fileExtension, defaultVS_inputLayout, false);
				m_resources[(int)ObjectType::VERTEX_SHADER][fileName] = shaderHandle;
		}

		else if (shaderType == ObjectType::PIXEL_SHADER)
		{
				shaderHandle.MakeNull();
				m_dx11Renderer->CreatePixelShader(shaderHandle, s_pixelShaderDir + fileName + fileExtension, false);
				m_resources[(int)ObjectType::PIXEL_SHADER][fileName] = shaderHandle;
		}
}

void GraphicsSystem::LoadPrimitiveShapes()
{
	m_modelManager->LoadPrimitiveShapes();
}

void GraphicsSystem::TestUpdateCamera(const float dt)
{
	const InputSystem* const input = reinterpret_cast<InputSystem*>(m_engineOwner->GetSystem(SystemType::ST_INPUT));

	if (input->m_keyboard->IsKeyHeld(KeyboardEvent::VirtualKey::KEY_W))
	{
		m_dx11Renderer->m_renderData->testCamera->Walk(dt);
	}

	if (input->m_keyboard->IsKeyHeld(KeyboardEvent::VirtualKey::KEY_S))
	{
		m_dx11Renderer->m_renderData->testCamera->Walk(-dt);
	}

	if (input->m_keyboard->IsKeyHeld(KeyboardEvent::VirtualKey::KEY_A))
	{
		m_dx11Renderer->m_renderData->testCamera->Strafe(-dt);
	}

	if (input->m_keyboard->IsKeyHeld(KeyboardEvent::VirtualKey::KEY_D))
	{
		m_dx11Renderer->m_renderData->testCamera->Strafe(dt);
	}

	if (input->m_keyboard->IsKeyHeld(KeyboardEvent::VirtualKey::KEY_Q))
	{
		m_dx11Renderer->m_renderData->testCamera->Elevate(dt);
	}

	if (input->m_keyboard->IsKeyHeld(KeyboardEvent::VirtualKey::KEY_E))
	{
		m_dx11Renderer->m_renderData->testCamera->Elevate(-dt);
	}

	if (input->m_mouse->IsLeftMouseButtonHeld()  && !ImGui::IsAnyItemActive()) {
		auto& delta = input->m_mouse->GetMouseDelta();
		if (std::fabsf(delta.y) > 0.00001f) {
			m_dx11Renderer->m_renderData->testCamera->RotateX(dt * (delta.y < 0.f ? -1.0f : 1.0f));
		}

		if (std::fabsf(delta.x) > 0.00001f) {
			m_dx11Renderer->m_renderData->testCamera->RotateY(dt * (delta.x < 0.f ? -1.0f : 1.0f));
		}
	}

	if (input->m_keyboard->IsKeyPressed(KeyboardEvent::VirtualKey::KEY_R))
	{
		m_dx11Renderer->m_renderData->testCamera->ResetAxis();
	}

	//Update view buffer
	m_dx11Renderer->m_renderData->testCamera->Update();
	m_dx11Renderer->m_renderData->testViewProjBuffer.cameraPosition.m128_f32[0] = m_dx11Renderer->m_renderData->testCamera->m_Position.m128_f32[0];
	m_dx11Renderer->m_renderData->testViewProjBuffer.cameraPosition.m128_f32[1] = m_dx11Renderer->m_renderData->testCamera->m_Position.m128_f32[1];
	m_dx11Renderer->m_renderData->testViewProjBuffer.cameraPosition.m128_f32[2] = m_dx11Renderer->m_renderData->testCamera->m_Position.m128_f32[2];
	m_dx11Renderer->m_renderData->testViewProjBuffer.cameraPosition.m128_f32[3] = m_dx11Renderer->m_renderData->m_debugIdx;

	m_dx11Renderer->m_renderData->testViewProjBuffer.cameraPosition = m_dx11Renderer->m_renderData->testCamera->m_Position;
	m_dx11Renderer->m_renderData->testViewProjBuffer.viewMtx = m_dx11Renderer->m_renderData->testCamera->GetView();
	m_dx11Renderer->m_renderData->testViewProjBuffer.invViewMtx =
			DirectX::XMMatrixInverse(nullptr, m_dx11Renderer->m_renderData->testViewProjBuffer.viewMtx);
	//m_dx11Renderer->m_renderData->testViewProjBuffer.projectionMtx = testCamera->GetProjection();
	m_dx11Renderer->m_renderData->m_pImmediateContext->UpdateSubresource(m_dx11Renderer->m_renderData->testViewProjConstBuffer,
			0, NULL, &m_dx11Renderer->m_renderData->testViewProjBuffer, 0, 0);
}

const string GraphicsSystem::s_shaderDir = "../MeshRenderer_Lite_Framework/Assets/Shaders/";
const string GraphicsSystem::s_vertexShaderDir = "../MeshRenderer_Lite_Framework/Assets/Shaders/VertexShaders/";
const string GraphicsSystem::s_pixelShaderDir = "../MeshRenderer_Lite_Framework/Assets/Shaders/PixelShaders/";
const string GraphicsSystem::s_textureDir = "../MeshRenderer_Lite_Framework/Assets/Shaders/";