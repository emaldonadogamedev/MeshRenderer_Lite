#include <Utilities/precompiled.h>
#include <Systems/Graphics/DX11RenderStages/UI Stage/ImGuiStage.h>

#include <Imgui/imgui.h>
#include <Imgui/imgui_impl_dx11.h>
#include <Systems/Core/Components/Transform/Transform.h>
#include <Systems/Core/GameObject/GameObject.h>
#include <Systems/Graphics/Components/ModelComponent/ModelComponent.h>
#include <Systems/Graphics/Components/CurvePathComponent/CurvePathComponent.h>
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
	auto& renderData = m_renderer->GetRendererData();
	renderData.m_pImmediateContext->RSSetState(renderData.m_d3dRasterStateImgui);
	m_renderer->DisableAlphaBlending();
}

void ImGuiStage::Render(HandleDictionaryVec& graphicsResources, const float dt)
{
	ImGui_ImplDX11_NewFrame();

	//static bool drawit = true;
	//ImGui::ShowTestWindow(&drawit);
	ShowGraphicsSettings();

	const auto& modelComponent = (ModelComponent*)GetComponentHelper(ComponentType::RENDERABLE_3D, 0);
	const auto& curvePathComponent = (CurvePathComponent*) GetComponentHelper(ComponentType::RENDERABLE_CURVE_PATH, 0);

	if (modelComponent)
	{
		auto* const transform = (Transform*)modelComponent->GetOwner()->GetComponent(ComponentType::TRANSFORM);
		auto model = modelComponent->GetModel();
		if (ImGui::Begin("Animation Properties"))
		{
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

		if (ImGui::Begin("Transform Properties"))
		{
			ImGui::DragFloat3("Position: ", transform->GetPosition().m128_f32, 0.1f);
			ImGui::DragFloat3("Rotation: ", transform->GetOrientation().m128_f32, 0.1f, 0, XM_2PI);
			ImGui::DragFloat3("Scale: ", transform->GetScale().m128_f32, 0.1f);

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
	auto& renderData = m_renderer->GetRendererData();

	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("Graphics Settings"))
		{
			if (ImGui::BeginMenu("Render States"))
			{
				if (ImGui::MenuItem("Solid, No Culling")) {
					renderData.m_currentRasterState = renderData.m_d3dRasterStateDefault;
				}
				if (ImGui::MenuItem("Solid, Front Culling")) {
					renderData.m_currentRasterState = renderData.m_d3dRasterStateSolCullFront;
				}
				if (ImGui::MenuItem("Solid, Back Culling")) {
					renderData.m_currentRasterState = renderData.m_d3dRasterStateSolCullBack;
				}
				if (ImGui::MenuItem("Wire-frame")) {
					renderData.m_currentRasterState = renderData.m_d3dRasterStateWireframe;
				}
				ImGui::EndMenu();
			}

			ImGui::Separator();
			if (ImGui::MenuItem("Lit")) {
				m_renderer->SetLightingEnabled(true);
			}
			if (ImGui::MenuItem("Unlit")) {
				m_renderer->SetLightingEnabled(false);
			}
			ImGui::Separator();
			if (ImGui::BeginMenu("Debug Menu..."))
			{
				ImGui::Checkbox("Show Debug Info...", &renderData.m_showDebugInfo);
				if (ImGui::MenuItem("Position")) {
					renderData.m_debugIdx = 0.f;
				}
				if (ImGui::MenuItem("Normals")) {
					renderData.m_debugIdx = 1.f;
				}
				if (ImGui::MenuItem("Tangent")) {
					renderData.m_debugIdx = 2.f;
				}
				if (ImGui::MenuItem("Depth Buffer")) {
					renderData.m_debugIdx = 3.f;
				}
				if (ImGui::MenuItem("UV coords")) {
					renderData.m_debugIdx = 4.f;
				}
				ImGui::EndMenu();
			}

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar(); //End main Menu Bar
	}
}
