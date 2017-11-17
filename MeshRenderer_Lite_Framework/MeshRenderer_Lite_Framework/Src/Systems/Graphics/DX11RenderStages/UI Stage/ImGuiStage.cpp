#include <Utilities/precompiled.h>
#include <Systems/Graphics/DX11RenderStages/UI Stage/ImGuiStage.h>

#include <Imgui/imgui.h>
#include <Imgui/imgui_impl_dx11.h>
#include <Systems/Core/Components/Transform/Transform.h>
#include <Systems/Core/GameObject/GameObject.h>
#include <Systems/Graphics/Components/ModelComponent/ModelComponent.h>
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

void ImGuiStage::Render(const HandleDictionaryVec& graphicsResources)
{
	ImGui_ImplDX11_NewFrame();

	auto& renderData = m_renderer->GetRendererData();
	renderData.m_pImmediateContext->RSSetState(renderData.m_d3dRasterStateImgui);

	bool drawit = true;

	const auto& modelComponent = (ModelComponent*)m_gfxSystemComponents->at(ComponentType::RENDERABLE_3D)[0];
	
	if (modelComponent)
	{
		auto* const transform = (Transform*)modelComponent->GetOwner()->GetComponent(ComponentType::TRANSFORM);
		auto model = modelComponent->GetModel();
		if (ImGui::Begin("Animation Properties"))
		{
			ImGui::Checkbox("Play Animation", &model->m_animationEnabled);
			ImGui::SliderFloat("Ticks per second", &model->m_ticksPerSecond, 1.0f, 100.0f);

			ImGui::Checkbox("Draw Bones", &model->m_debugDrawEnabled);
			ImGui::Checkbox("Draw Skin", &model->m_drawSkin);

			//if(ImGui::ListBox\\)

			ImGui::End();
		}

		if (ImGui::Begin("Transform Properties"))
		{
			ImGui::SliderFloat3("position: ", transform->GetPosition().m128_f32, -100.0f, 100.0f, "%.3f");
			ImGui::SliderFloat4("orientation: ", transform->GetOrientation().m128_f32, -1.0f, 1.0f, "%.3f");
			ImGui::SliderFloat3("scale: ", transform->GetScale().m128_f32, -100.0f, 100.0f, "%.3f");

			ImGui::End();
		}
	}

	ImGui::Render();
}

void ImGuiStage::PostRender()
{
}
