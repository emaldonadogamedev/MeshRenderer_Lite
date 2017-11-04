#include <Utilities/precompiled.h>
#include <Systems/Graphics/DX11RenderStages/UI Stage/ImGuiStage.h>

#include <Imgui/imgui.h>
#include <Imgui/imgui_impl_dx11.h>
#include <Systems/Graphics/DX11Renderer/DX11Renderer.h>
#include <Systems/Graphics/DX11Renderer/DX11RendererData.h>
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
	ImGui::ShowTestWindow(&drawit);

	ImGui::Render();
}

void ImGuiStage::PostRender()
{
}
