#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <Systems/Core/ISystem.h>

class IEngine;

using std::wstring;

class WindowSystem : public ISystem
{
public:
	WindowSystem(IEngine* const eng, HINSTANCE hInstance = nullptr);
	virtual ~WindowSystem();

	//Inherited methods
	virtual bool Initialize()override;
	virtual void Update(const float dt)override;
	virtual void Shutdown()override;
	virtual void ReceiveMessage(const IMessage& msg) override;

	HWND GetWindowsHandler() const;
	float GetAspectRatio(void) const;
	int GetWindowWidth() const;
	int GetWindowHeight() const;
	bool IsFullScreen() const;

	void SetWindowWidth(unsigned int width);
	void SetWindowHeight(unsigned int height);

protected:
	HINSTANCE m_hInstance;
	bool m_isFullScreen;
	HWND m_hwnd;
	int m_windowWidth;
	int m_windowHeight;
	wstring m_windowCaption;

	friend IEngine;
};