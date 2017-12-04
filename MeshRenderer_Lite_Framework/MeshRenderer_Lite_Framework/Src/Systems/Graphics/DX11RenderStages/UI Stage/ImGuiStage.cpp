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

ImGuiStage::ImGuiStage(DX11Renderer* const rendererData, RenderCompVec* const gfxComponents)
	:IRenderStage(rendererData, gfxComponents)
{

}

ImGuiStage::~ImGuiStage()
{

}

void ImGuiStage::PreRender()
{
}

void ImGuiStage::Render(const HandleDictionaryVec& graphicsResources, const float dt)
{
	ImGui_ImplDX11_NewFrame();

	auto& renderData = m_renderer->GetRendererData();
	renderData.m_pImmediateContext->RSSetState(renderData.m_d3dRasterStateImgui);

	bool drawit = true;
	//ImGui::ShowTestWindow(&drawit);

	const auto& modelComponent = (ModelComponent*)m_gfxSystemComponents->at(ComponentType::RENDERABLE_3D)[0];
	const auto& pathComponent = (CurvePathComponent*)m_gfxSystemComponents->at(ComponentType::RENDERABLE_CURVE_PATH)[0];

	if (modelComponent)
	{
		auto* const transform = (Transform*)modelComponent->GetOwner()->GetComponent(ComponentType::TRANSFORM);
		auto model = modelComponent->GetModel();
		if (ImGui::Begin("Animation Properties"))
		{
			ImGui::Text("FPS: %.3f", 1.0f/dt);
			ImGui::Checkbox("Play Animation", &model->m_animationEnabled);
			ImGui::SliderFloat("Ticks per second", &model->m_ticksPerSecond, 1.0f, 100.0f);

			ImGui::Checkbox("Draw Bones", &model->m_debugDrawEnabled);
			ImGui::Checkbox("Draw Skin", &model->m_drawSkin);
			ImGui::Separator();

			if (pathComponent)
			{
				ImGui::Checkbox("Use Path: ", &pathComponent->m_usePath);
				ImGui::DragFloat3("Center Position: ", pathComponent->m_pathCenterPos.m128_f32);
				if (ImGui::DragFloat("Path time duration: ", &pathComponent->m_pathDuration, 0.1f, 1.0f, 30.0f))
				{
					pathComponent->m_segmentDuration = pathComponent->m_pathDuration / pathComponent->m_segmentCount;
				}
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
