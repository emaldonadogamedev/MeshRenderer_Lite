#include <Utilities/precompiled.h>
#include <Systems/Input/Mouse.h>

#include <Engine/Applications/IApplication/IApplication.h>
#include <Systems/Input/InputSystem.h>
#include <Systems/Window/WindowSystem.h>

MouseEvent::MouseEvent(InputSystem* inputSystem) : m_ScrollDelta(0), m_inputSystem(inputSystem)
{
	memset(m_PreviousKeyStates, 0, sizeof(m_PreviousKeyStates));
	memset(m_CurrentKeyStates, 0, sizeof(m_CurrentKeyStates));
}

MouseEvent::~MouseEvent()
{

}

void MouseEvent::Update(void)
{
	memcpy(m_PreviousKeyStates, m_CurrentKeyStates, sizeof(bool) * 3);
	m_MouseDelta.x = m_MouseDelta.y = 0.f;
	m_ScrollDelta = 0;
}

void MouseEvent::Clear(void)
{
	memset(m_CurrentKeyStates, false, sizeof(bool) * 3);
	memset(m_PreviousKeyStates, false, sizeof(bool) * 3);
}

bool MouseEvent::IsLeftMouseButtonPressed()
{
	return (!m_PreviousKeyStates[LEFT] && m_CurrentKeyStates[LEFT]);
}

bool MouseEvent::IsLeftMouseButtonHeld()
{
	return (m_PreviousKeyStates[LEFT] && m_CurrentKeyStates[LEFT]);
}

bool MouseEvent::IsLeftMouseButtonReleased()
{
	return (m_PreviousKeyStates[LEFT] && !m_CurrentKeyStates[LEFT]);
}

bool MouseEvent::IsMiddleMouseButtonPressed()
{
	return (!m_PreviousKeyStates[MIDDLE] && m_CurrentKeyStates[MIDDLE]);
}

bool MouseEvent::IsMiddleMouseButtonHeld()
{
	return (m_PreviousKeyStates[MIDDLE] && m_CurrentKeyStates[MIDDLE]);
}

bool MouseEvent::IsMiddleMouseButtonReleased()
{
	return (m_PreviousKeyStates[MIDDLE] && !m_CurrentKeyStates[MIDDLE]);
}

bool MouseEvent::IsRightMouseButtonPressed()
{
	return (!m_PreviousKeyStates[RIGHT] && m_CurrentKeyStates[RIGHT]);
}

bool MouseEvent::IsRightMouseButtonHeld()
{
	return (m_PreviousKeyStates[RIGHT] && m_CurrentKeyStates[RIGHT]);
}

bool MouseEvent::IsRightMouseButtonReleased()
{
	return (m_PreviousKeyStates[RIGHT] && !m_CurrentKeyStates[RIGHT]);
}

int MouseEvent::GetScrollDelta()
{
	return m_ScrollDelta;
}

void MouseEvent::SetScollDelta(int delta)
{
	m_ScrollDelta = delta;
}

void MouseEvent::SetLeftMouseButton(bool val)
{
	m_CurrentKeyStates[LEFT] = val;
}

void MouseEvent::SetMiddleMouseButton(bool val)
{
	m_CurrentKeyStates[MIDDLE] = val;
}

void MouseEvent::SetRightMouseButton(bool val)
{
	m_CurrentKeyStates[RIGHT] = val;
}

void MouseEvent::SetMousePosition(int x, int y)
{
	m_MouseDelta.x = float(x) - m_MousePos.x;
	m_MouseDelta.y = float(y) - m_MousePos.y;

	m_prevMousePos = m_MousePos;

	m_MousePos.x = float(x);
	m_MousePos.y = float(y);
}

const XMFLOAT2& MouseEvent::GetPrevMousePosition() const
{
	return m_prevMousePos;
}

const XMFLOAT2& MouseEvent::GetMousePosition() const
{
	//const WindowSystem* windowSystem = reinterpret_cast<WindowSystem*>(m_inputSystem->m_engineOwner->GetSystem(SystemType::ST_WINDOW));
	//POINT p;
	//GetCursorPos(&p);
	//ScreenToClient(windowSystem->GetWindowsHandler(), &p);

	//m_MousePos.x = p.x;
	//m_MousePos.y = p.y;


	return m_MousePos;
}

int MouseEvent::GetMouseXPosition()
{
	return static_cast<int>(m_MousePos.x);
}

int MouseEvent::GetMouseYPosition()
{
	return static_cast<int>(m_MousePos.y);
}

const XMFLOAT2& MouseEvent::GetMouseDelta() const
{
	return m_MouseDelta;
}