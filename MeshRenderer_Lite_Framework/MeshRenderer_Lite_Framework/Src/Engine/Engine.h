#pragma once


#define WIN32_LEAN_AND_MEAN	
#include <Windows.h>

#include<memory>
#include<vector>
#include<unordered_map>
#include<string>
#include<Engine/GameClock.h>

//Forward declarations
class ISystem;
class IMessage;
class InputSystem;
class WindowSystem;
class GraphicsSystem;
class GameObject;

using std::string;
using std::vector;
using std::unordered_map;

class Engine
{
public:
	Engine(void);
	~Engine(void);

	bool Initialize(HINSTANCE hInstance);
	void Run(void);
	void Shutdown(void);

	ISystem* GetSystem(const string& systemName) const;
	virtual void SendMessageToSystems(const IMessage* const msg);

	bool IsRunning() const;
	LRESULT WINAPI MessageHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	void OnMouseDown(WPARAM buttonState, int x, int y);
	void OnMouseUp(WPARAM buttonState, int x, int y);
	void OnMouseMove(WPARAM buttonState, int x, int y);

	//GAME CLOCK
	const GameClock& GetClock(void) const;

	static Engine* s_engineApp;

protected:
	bool AddSystemHelper(ISystem* const sys = nullptr);

	void CalculateFrameTime(void);
	static const float s_fps60;
	unsigned long long m_frameCount = 0;
	float m_timeElapsed = 0.0f;

	bool m_isPaused;
	bool m_isMinimized;
	bool m_isMaximized;
	bool m_isResized;
	bool m_isRunning;

	GameClock m_clock;
	//POINT m_MousePosition;

	//////////////////////////////////////////////////////////////////////////
	// FOR TEST!
	std::unique_ptr<GameObject> testObj;

	//Engine system pointers
	vector<ISystem*> m_systems;
	unordered_map<string, ISystem*> m_systemsMap;
	InputSystem* m_inputSystem;
	WindowSystem* m_windowSystem;
	GraphicsSystem* m_graphicsSystem;
};
