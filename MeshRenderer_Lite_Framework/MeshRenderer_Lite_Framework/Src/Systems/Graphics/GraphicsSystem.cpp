#include <Utilities/precompiled.h>

#include <Systems/Graphics/GraphicsSystem.h>

#include <Engine/Engine.h>
#include <Systems/Graphics/CameraClasses/Camera.h>
#include <Systems/Graphics/DX11Renderer/DX11Renderer.h>
#include <Systems/Graphics/DX11Renderer/DX11RendererData.h>
#include <Systems/Graphics/DX11RenderStages/ForwardRenderStage/ForwardRenderStage.h>
#include <Systems/Input/InputSystem.h>
#include <Systems/Input/Keyboard.h>
#include <Systems/Input/Mouse.h>
#include <Systems/Window/WindowSystem.h>

GraphicsSystem::GraphicsSystem(Engine* const eng) :
	ISystem(eng),
	m_dx11Renderer(std::make_unique<DX11Renderer>())
{
	testCamera = new Camera();
}

GraphicsSystem::~GraphicsSystem()
{
	SafeDelete(testCamera);
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
	TestUpdateCamera(dt);
	testCamera->Update();
	m_dx11Renderer->testViewProjBuffer.viewMtx = testCamera->GetView();
	//m_dx11Renderer->testViewProjBuffer.projectionMtx = testCamera->GetProjection();

	m_dx11Renderer->m_renderData->m_pImmediateContext->UpdateSubresource(m_dx11Renderer->m_renderData->testViewProjConstBuffer, 
		0, NULL, &m_dx11Renderer->testViewProjBuffer, 0, 0);

	m_dx11Renderer->m_renderData->m_pImmediateContext->VSSetConstantBuffers(0, 1, &m_dx11Renderer->m_renderData->testPerObjectConstBuffer);
	m_dx11Renderer->m_renderData->m_pImmediateContext->VSSetConstantBuffers(1, 1, &m_dx11Renderer->m_renderData->testViewProjConstBuffer);
	m_dx11Renderer->m_renderData->m_pImmediateContext->PSSetShader(m_dx11Renderer->m_renderData->testPixelShader, NULL, 0);

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

void GraphicsSystem::TestUpdateCamera(const float dt)
{
	const InputSystem* input = reinterpret_cast<InputSystem*>(m_engineOwner->GetSystem("Input"));

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
