#pragma once


#define WIN32_LEAN_AND_MEAN	
#include <Windows.h>

#include<Systems/Core/ISystem.h>

#include<memory>
#include<array>
#include<vector>
#include<unordered_map>
#include<string>

class InputSystem;
class WindowSystem;
class GraphicsSystem;
class GameObject;
class GameClock;

enum class SystemType : char;

using std::string;
using std::vector;
using std::unordered_map;

class IApplication
{
public:
	IApplication(void);
	virtual ~IApplication(void);

	virtual bool Initialize(HINSTANCE hInstance);
	void Run(void);
	virtual void Shutdown(void);

	ISystem* GetSystem(const SystemType systemType) const;
	virtual void SendMessageToSystems(const IMessage* const msg);

	bool IsRunning() const;
	virtual LRESULT MessageHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	void OnMouseDown(WPARAM buttonState, int x, int y);
	void OnMouseUp(WPARAM buttonState, int x, int y);
	void OnMouseMove(WPARAM buttonState, int x, int y);

	//GAME CLOCK
	const GameClock& GetClock(void) const;

	static IApplication* s_engineApp;

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

	std::unique_ptr<GameClock> m_clock;
	//POINT m_MousePosition;

	//////////////////////////////////////////////////////////////////////////
	// FOR TEST!
	std::unique_ptr<GameObject> testObj;

	//Engine system pointers
	std::vector<ISystem*> m_systems;;

	InputSystem* m_inputSystem;
	WindowSystem* m_windowSystem;
	GraphicsSystem* m_graphicsSystem;
};
