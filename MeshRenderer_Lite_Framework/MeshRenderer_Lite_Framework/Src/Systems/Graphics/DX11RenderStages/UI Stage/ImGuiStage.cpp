#include <Utilities/precompiled.h>
#include <Systems/Graphics/DX11RenderStages/UI Stage/ImGuiStage.h>

#include <Imgui/imgui.h>
#include <Imgui/imgui_impl_dx11.h>
#include <Systems/Core/Components/Transform/Transform.h>
#include <Systems/Core/GameObject/GameObject.h>
#include <Systems/Graphics/Components/ModelComponent/ModelComponent.h>
#include <Systems/Graphics/Components/CurvePathComponent/CurvePathComponent.h>
#include <Systems/Graphics/Components/LightComponents/Light.h>
#include <Systems/Graphics/Components/LightComponents/ShadowLightComponent/ShadowLightComponent.h>
#include <Systems/Graphics/DX11Renderer/DX11Renderer.h>
#include <Systems/Graphics/DX11Renderer/DX11RendererData.h>
#include <Systems/Graphics/ModelClasses/Model/Model.h>
#include <Systems/Window/WindowSystem.h>

ImGuiStage::ImGuiStage(DX11Renderer* const rendererData, RenderCompUmap* const gfxComponents)
	:IRenderStage(rendererData, gfxComponents)
{

}

ImGuiStage::~ImGuiStage()
{

}

void ImGuiStage::PreRender()
{
	m_renderData.m_pImmediateContext->RSSetState(m_renderData.m_d3dRasterStateImgui);
	m_renderer->DisableColorBlending();
}

void ImGuiStage::Render(HandleDictionaryVec& graphicsResources, const float dt)
{
	ImGui_ImplDX11_NewFrame();

	//static bool drawit = true;
	//ImGui::ShowTestWindow(&drawit);
	ShowGraphicsSettings();

	auto& modelComponents = m_gfxSystemComponents->at(ComponentType::RENDERABLE_3D);
	const auto& curvePathComponent = (CurvePathComponent*) GetComponentHelper(ComponentType::RENDERABLE_CURVE_PATH, 1);

	if (!modelComponents.empty())
	{
			Transform* transform = nullptr;
			Model* model = nullptr;

		if (ImGui::Begin("Animation Properties"))
		{
			static int modelComponentIdx = 0;
			ImGui::SliderInt("Model number: ", &modelComponentIdx, 0, modelComponents.size() - 1);
			const auto& modelComponent = (ModelComponent*)GetComponentHelper(ComponentType::RENDERABLE_3D, modelComponentIdx);
			transform = (Transform*)modelComponent->GetOwner()->GetComponent(ComponentType::TRANSFORM);
			model = modelComponent->GetModel();

			ImGui::Text("FPS: %.3f", 1.0f/dt);
			
			if (model->GetModelType() == ModelType::MODEL_SKINNED)
			{
				ImGui::Checkbox("Play Animation", &model->m_animationEnabled);
				ImGui::ListBox("Available animations:", &model->m_currentAnimIndex, model->m_animationNameCharPtrs.data(), model->m_animationNameCharPtrs.size(), 2);
			}
			ImGui::SliderFloat("Ticks per second", &model->m_ticksPerSecond, 1.0f, 3000.0f);

			ImGui::Checkbox("Draw Bones", &model->m_debugDrawEnabled);
			ImGui::Checkbox("Draw Skin", &model->m_drawSkin);
			ImGui::Separator();

			if (curvePathComponent)
			{
				ImGui::Checkbox("Use Path: ", &curvePathComponent->m_usePath);
				ImGui::DragFloat3("Center Position: ", curvePathComponent->m_pathCenterPos.m128_f32);

				ImGui::Text("Current Vel. X=%f, Z=%f", curvePathComponent->m_currVelDir.m128_f32[0], curvePathComponent->m_currVelDir.m128_f32[2]);

				if (ImGui::SliderFloat("Walk speed: ", &curvePathComponent->m_walkSpeed, 10.f, 900.f))
				{
					if (curvePathComponent->m_walkSpeed > 0)
						curvePathComponent->m_currentPathDuration = curvePathComponent->m_totalLengthOfCurve / curvePathComponent->m_walkSpeed;
					else
						curvePathComponent->m_currentPathDuration = 0;
				}
				ImGui::SliderFloat("Ease In Time(seconds): ", &curvePathComponent->m_easeInTime, 0, 5.f);
				ImGui::SliderFloat("Ease Out Start Rate: ", &curvePathComponent->m_easeOutRate, 0.5f, 1.0f);
				if (ImGui::Button("Generate New Path"))
				{
					curvePathComponent->DefaultPointSet();
					curvePathComponent->PrepareDrawPoints();
					curvePathComponent->GenerateVertexBuffer(m_renderer);
				}
				//ImGui::SliderFloat("Emergency!", &curvePathComponent->deleteThisAfterUsage, -DirectX::XM_PI, DirectX::XM_PI);
			}
			ImGui::End();
		}

		if (ImGui::Begin("Mesh properties"))
		{
			ImGui::DragFloat3("Position: ", transform->GetPosition().m128_f32, 0.1f);
			ImGui::DragFloat3("Rotation: ", transform->GetOrientation().m128_f32, 0.1f, 0, XM_2PI);
			ImGui::DragFloat3("Scale: ", transform->GetScale().m128_f32, 0.1f);
			ImGui::Separator();

			ImGui::LabelText("Material properties!", "");
			for (auto& meshEntry : model->m_meshEntryList)
			{
					ImGui::DragFloat3("Ambient: ", &meshEntry.meshMaterial.m_materialProperties.ambientKa.x, 0.1f);
					ImGui::DragFloat3("Diffuse: ", &meshEntry.meshMaterial.m_materialProperties.diffuseKd.x, 0.1f);
					ImGui::DragFloat3("Specular: ", &meshEntry.meshMaterial.m_materialProperties.specularKs.x, 0.1f);
					ImGui::DragFloat("NS: ", &meshEntry.meshMaterial.m_materialProperties.specularPowerNs, 0.1f);

					ImGui::SliderInt("Use Diffuse Texture", &meshEntry.meshMaterial.m_materialProperties.useDiffuseTexture, 0, 1);
					ImGui::SliderInt("Use Normal Map", &meshEntry.meshMaterial.m_materialProperties.useNormalMap, 0, 1);
			}

			ImGui::End();
		}

		if (ImGui::Begin("Shadow Light Properties"))
		{
				ImGui::DragFloat3("Global Ambient", &m_renderData.testGlobalShaderProperties.gGlobalAmbient.x, 0.001f, 0.f, 1.0f, "%.3f");

				const auto& shadowLight = m_gfxSystemComponents->at(ComponentType::RENDERABLE_LIGHT_WITH_SHADOW)[0];
				const ShadowLightComponent* shadowLighComp = (const ShadowLightComponent*)shadowLight;
				const auto light = shadowLighComp->GetLight();

				ImGui::SliderInt("Is Active", &light->isActive, 0, 1);
				ImGui::SliderInt("Light type", &light->m_lightType, 0, 2);
				ImGui::SliderInt("Is Using Shadows", &light->isUsingShadows, 0, 1);
				ImGui::SliderInt("Is Using Soft Shadows", &light->isUsingSoftShadows, 0, 1);

				auto* const transform = (Transform*)shadowLighComp->GetOwner()->GetComponent(ComponentType::TRANSFORM);
				ImGui::DragFloat3("Position", transform->GetPosition().m128_f32, 0.001f);

				ImGui::DragFloat3("Light Rotation", &transform->GetOrientation().m128_f32[0], 0.2f, -180, 180, "%.3f");
				ImGui::DragFloat3("Ambient", &light->m_Iambient.x, 0.001f, 0.f, 1.0f, "%.3f");
				ImGui::DragFloat3("Diffuse", light->m_Idiffuse.m128_f32, 0.001f, 0.f, 1.0f, "%.3f");
				ImGui::DragFloat3("Specular", &light->m_Ispecular.x, 0.001f, 0.f, 1.0f, "%.3f");

				ImGui::DragFloat("Constant Att: ", &light->m_ConstantAttenuation, 0.001f);
				ImGui::DragFloat("Linear Att: ", &light->m_LinearAttenuation, 0.001f);
				ImGui::DragFloat("Quadratics Att: ", &light->m_QuadraticAttenuation, 0.001f);

				ImGui::End();
		}
	}

	ImGui::Render();
}

void ImGuiStage::PostRender()
{

}

void ImGuiStage::ShowGraphicsSettings()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("Graphics Settings"))
		{
			if (ImGui::BeginMenu("Render States"))
			{
				if (ImGui::MenuItem("Solid, No Culling")) {
					m_renderData.m_currentRasterState = m_renderData.m_d3dRasterStateSolCullNone;
				}
				if (ImGui::MenuItem("Solid, Front Culling")) {
					m_renderData.m_currentRasterState = m_renderData.m_d3dRasterStateSolCullFront;
				}
				if (ImGui::MenuItem("Solid, Back Culling")) {
					m_renderData.m_currentRasterState = m_renderData.m_d3dRasterStateSolCullBack;
				}
				if (ImGui::MenuItem("Wire-frame")) {
					m_renderData.m_currentRasterState = m_renderData.m_d3dRasterStateWireframe;
				}
				ImGui::EndMenu();
			}

			ImGui::Separator();
			if (ImGui::BeginMenu("Debug Menu..."))
			{
				if (ImGui::MenuItem("No Debug Info")) {
						m_renderData.testGlobalShaderProperties.gDebugInfoType = 
								(int)GlobalGraphicsDebugType::G_DEBUG_NONE;
				}
				if (ImGui::MenuItem("Position")) {
						m_renderData.testGlobalShaderProperties.gDebugInfoType = 
								(int)GlobalGraphicsDebugType::G_DEBUG_POSITION;
				}
				if (ImGui::MenuItem("Normals")) {
					m_renderData.testGlobalShaderProperties.gDebugInfoType = 
							(int)GlobalGraphicsDebugType::G_DEBUG_NORMALS;
				}
				if (ImGui::MenuItem("Depth Buffer")) {
						m_renderData.testGlobalShaderProperties.gDebugInfoType =
								(int)GlobalGraphicsDebugType::G_DEBUG_DEPTH;
				}
				if (ImGui::MenuItem("UV coords")) {
						m_renderData.testGlobalShaderProperties.gDebugInfoType =
								(int)GlobalGraphicsDebugType::G_DEBUG_UV_COORDS;
				}
				ImGui::EndMenu();
			}

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar(); //End main Menu Bar
	}
}
