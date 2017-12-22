#include <Utilities/precompiled.h>
#include <Engine/IApplication/IApplication.h>

#include <Imgui/imgui.h>
#include <Imgui/imgui_impl_dx11.h>

#define WIN32_LEAN_AND_MEAN
#include <windowsx.h>

#include<Engine/GameClock/GameClock.h>
#include <Systems/Core/Components/Transform/Transform.h>
#include <Systems/Core/GameObject/GameObject.h>
#include <Systems/Graphics/Components/ModelComponent/ModelComponent.h>
#include <Systems/Graphics/Components/CurvePathComponent/CurvePathComponent.h>
#include <Systems/Graphics/Components/SimpleCCD/SuperSimpleCCD.h>
#include <Systems/Graphics/Components/SimpleCloth/SimpleClothComponent.h>
#include <Systems/Graphics/GraphicsSystem.h>
#include <Systems/Input/InputSystem.h>
#include <Systems/Input/Keyboard.h>
#include <Systems/Input/Mouse.h>
#include <Systems/Window/WindowSystem.h>

IApplication::IApplication(void):
m_isPaused(false),
m_isMinimized(false),
m_isMaximized(false),
m_isResized(false),
m_isRunning(false),
m_windowSystem(nullptr),
m_clock(std::make_unique<GameClock>()),
m_systems((int)SystemType::COUNT, nullptr)
{
}

IApplication::~IApplication(void)
{

}

bool IApplication::Initialize(HINSTANCE hInstance)
{
	//TODO:
	//DON'T HAVE A STATIC VARIABLE!!
	// set the engine pointer to this class for the window system
	s_engineApp = this;
	
	//////////////////////////////////////////////////////////////////////////
	//Prepare the Window system
	if (!AddSystemHelper(m_windowSystem = new WindowSystem(this, hInstance)))
		return false;
	//////////////////////////////////////////////////////////////////////////
	//Prepare the Graphics system
	if (!AddSystemHelper(m_graphicsSystem = new GraphicsSystem(this)))
		return false;
	//////////////////////////////////////////////////////////////////////////
	//Prepare the Input system
	if (!AddSystemHelper(m_inputSystem = new InputSystem(this)))
		return false;
	
	//Initialize all of the systems
	for (auto& it : m_systems)
	{
		if (!it || !it->Initialize())
			return m_isRunning = false;
	}

	testObj = std::make_unique<GameObject>();
	testObj->AddComponent(new Transform(testObj.get()));
	ModelComponent* test3DComp = new ModelComponent(testObj.get());
	auto graphicsSystem = static_cast<GraphicsSystem*>(GetSystem(SystemType::ST_GRAPHICS));
	const auto& loadedModels = graphicsSystem->GetLoadedModels();

	//test3DComp->SetModel(loadedModels.at("dragon.obj").get());
	//test3DComp->SetModel(loadedModels.at("bunny.obj").get());
	test3DComp->SetModel(loadedModels.at("cylinder_skellmesh.fbx").get());
	//test3DComp->SetModel(loadedModels.at("tiny_4anim.x").get());
	//test3DComp->SetModel(loadedModels.at("boblampclean.md5mesh").get());

	testObj->AddComponent(test3DComp);
	graphicsSystem->AddComponent(test3DComp);

	//auto* testPathComp = new CurvePathComponent(testObj.get());
	//testPathComp->GenerateVertexBuffer(m_graphicsSystem->m_dx11Renderer.get());
	//testObj->AddComponent(testPathComp);
	//m_graphicsSystem->AddComponent(testPathComp);

	auto* testSimpleCloth = new SimpleClothComponent(testObj.get(), 12, 12, 33, 33 );
	testObj->AddComponent(testSimpleCloth);
	GetSystem(SystemType::ST_GRAPHICS)->AddComponent(testSimpleCloth);
	testSimpleCloth->generateVertexBuffers(graphicsSystem->m_dx11Renderer.get());

	return m_isRunning = true;
}

void IApplication::Run(void)
{
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	m_clock->ResetClock();

	//main game loop
	while (m_isRunning)
	{
		// If there are Window messages then process them.
		while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT)
			{
				m_isRunning = false;
			}
		}
		// Otherwise, do animation/game stuff.
		m_clock->Tick();

		if (!m_isPaused)
		{
			CalculateFrameTime();
			const float dt = m_clock->GetDeltaTime();

			for (const auto& it : m_systems)
			{
				it->Update(dt);
			}
#ifdef _DEBUG
			const int waitTimeMS = (int)((s_fps60 - dt) * 1000.0f);
			Sleep(waitTimeMS * (dt < s_fps60));
#endif
		}
		else
		{
			Sleep(100);
		}

	} // end while(IsRunning())
}

void IApplication::Shutdown(void)
{
	for (auto& it : m_systems)
	{
		it->Shutdown();
		SafeDelete(it);
	}
}

ISystem* IApplication::GetSystem(const SystemType systemType) const
{
	return m_systems[(int)systemType];
}

void IApplication::SendMessageToSystems(const IMessage * const msg)
{
	if (msg)
	{
		for (auto& it : m_systems)
		{
			it->ReceiveMessage(*msg);
		}

		//delete the message pointer
		delete msg;
	}
}

bool IApplication::IsRunning() const
{
	return m_isRunning;
}

extern LRESULT ImGui_ImplDX11_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT IApplication::MessageHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	ImGui_ImplDX11_WndProcHandler(hwnd, msg, wParam, lParam);
		
	//handle input for the application
	switch (msg)
	{
	case WM_ACTIVATE:
		if (LOWORD(wParam) == WA_INACTIVE)
		{
			m_isPaused = true;
			m_clock->StopClock();
		}
		else
		{
			m_isPaused = false;
			m_clock->StartClock();
		}
		return 0;

	case WM_SIZE:
	{
		ImGui_ImplDX11_InvalidateDeviceObjects();

		// Save the new client area dimensions.
		const int w = LOWORD(lParam);
		const int h = HIWORD(lParam);
		m_windowSystem->SetWindowWidth(w);
		m_windowSystem->SetWindowHeight(h);

		m_graphicsSystem->Resize(w, h);

		ImGui_ImplDX11_CreateDeviceObjects();
		return 0;
	}

	case WM_ENTERSIZEMOVE:
		m_isPaused = true;
		m_isResized = true;
		m_clock->StopClock();
		return 0;

	case WM_EXITSIZEMOVE:
		m_isPaused = false;
		m_isResized = false;
		m_clock->StartClock();
		return 0;

		// WM_DESTROY is sent when the window is being destroyed.
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_MENUCHAR:
		// Don't beep when we alt-enter.
		return MAKELRESULT(0, MNC_CLOSE);
		break;

	case WM_LBUTTONDOWN:
		m_inputSystem->m_mouse->SetLeftMouseButton(true);
		break;
	case WM_MBUTTONDOWN:
		m_inputSystem->m_mouse->SetMiddleMouseButton(true);
		break;
	case WM_RBUTTONDOWN:
		m_inputSystem->m_mouse->SetRightMouseButton(true);
		break;
	case WM_LBUTTONUP:
		m_inputSystem->m_mouse->SetLeftMouseButton(false);
		//Physics::TestMouseIntersection(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), SkyeGraphics::GetInstance()->GetVolumeList());
		break;
	case WM_MBUTTONUP:
		m_inputSystem->m_mouse->SetMiddleMouseButton(false);
		break;
	case WM_RBUTTONUP:
		m_inputSystem->m_mouse->SetRightMouseButton(false);
		break;
	case WM_MOUSEMOVE:
		m_inputSystem->m_mouse->SetMousePosition(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		break;

	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
		{
			PostQuitMessage(0);
			return 0;
		}
		m_inputSystem->m_keyboard->SetKey(wParam, true);
		break;

	case WM_KEYUP:
		m_inputSystem->m_keyboard->SetKey(wParam, false);
		break;

	case VK_LMENU:
		return 0;
		break;

	case VK_RMENU:
		break;

	}// end switch(msg)

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

void IApplication::OnMouseDown(WPARAM buttonState, int x, int y)
{

}

void IApplication::OnMouseUp(WPARAM buttonState, int x, int y)
{

}

void IApplication::OnMouseMove(WPARAM buttonState, int x, int y)
{

}

const GameClock& IApplication::GetClock(void) const
{
	return *m_clock;
}

IApplication* IApplication::s_engineApp;

bool IApplication::AddSystemHelper(ISystem* const sys /*= nullptr*/)
{
	if (!sys)
		return false;

	const int type = (int)sys->GetType();
	m_systems[type] = sys;

	return true;
}

void IApplication::CalculateFrameTime(void)
{
	m_frameCount++;

	// Compute averages owner one second period.
	if ((m_clock->GetTotalTime() - m_timeElapsed) >= 1.0f)
	{
		const float fps = (float)m_frameCount; // fps = frameCnt / 1
		const float mspf = 1000.0f / fps;

		//std::wostringstream outs;
		//outs.precision(6);

		// Reset for next average.
		m_frameCount = 0;
		m_timeElapsed += 1.0f;
	}
}

const float IApplication::s_fps60 = 1.0f / 60.0f;