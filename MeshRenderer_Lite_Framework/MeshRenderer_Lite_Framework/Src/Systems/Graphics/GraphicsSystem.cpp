#include <Utilities/precompiled.h>

#include <Systems/Graphics/GraphicsSystem.h>

#include <Engine/Engine.h>
#include <Systems/Graphics/DX11Renderer/DX11Renderer.h>
#include <Systems/Graphics/DX11RenderStages/ForwardRenderStage/ForwardRenderStage.h>
#include <Systems/Window/WindowSystem.h>

GraphicsSystem::GraphicsSystem(Engine* const eng) :
	ISystem(eng),
	m_dx11Renderer(std::make_unique<DX11Renderer>())
{
}

GraphicsSystem::~GraphicsSystem()
{

}

bool GraphicsSystem::Initialize()
{
	const WindowSystem* const window = (WindowSystem*)m_engineOwner->GetSystem("Windows");

	bool result = m_dx11Renderer->InitializeRenderer(window->GetWindowWidth(), window->GetWindowHeight(), window->GetWindowsHandler());

	AddRenderStageHelper(new ForwardRenderStage(m_dx11Renderer.get()));



	return result;
}

void GraphicsSystem::Update(const float dt)
{
	m_dx11Renderer->ClearBuffer();

	//TEST!!!
	m_dx11Renderer->Draw(3, 0);

	for (const auto renderStage : m_renderStages)
	{
		renderStage->PreRender();
		//////////////////////////////////////////////////////////////////////////
		//TODO: Render stuff!
		//renderStage->Render();
		renderStage->PostRender();
	}

	m_dx11Renderer->SwapBuffers();
}

void GraphicsSystem::Shutdown()
{
	for (auto renderStage : m_renderStages)
	{
		SafeDelete(renderStage);
	}
	m_renderStages.clear();

	m_dx11Renderer->ReleaseData();
}

void GraphicsSystem::ReceiveMessage(const IMessage& msg)
{
}

void GraphicsSystem::AddRenderStageHelper(IRenderStage* renderStage)
{
	if (renderStage)
	{
		m_renderStages.emplace_back(renderStage);
	}
}