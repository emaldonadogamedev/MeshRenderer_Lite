#pragma once

#include <DirectXMath.h>

using DirectX::XMFLOAT2;

class InputSystem;

class MouseEvent
{

public:
	MouseEvent(InputSystem* inputSystem);
	~MouseEvent();

	enum MOUSESTATES { LEFT, MIDDLE, RIGHT };

	void Update(void);
	void Clear(void);

	bool IsLeftMouseButtonPressed();
	bool IsLeftMouseButtonHeld();
	bool IsLeftMouseButtonReleased();

	bool IsMiddleMouseButtonPressed();
	bool IsMiddleMouseButtonHeld();
	bool IsMiddleMouseButtonReleased();

	bool IsRightMouseButtonPressed();
	bool IsRightMouseButtonHeld();
	bool IsRightMouseButtonReleased();

	int GetScrollDelta();
	void SetScollDelta(int delta);
	void SetMousePosition(int x, int y);

	const XMFLOAT2& GetPrevMousePosition() const;

	const XMFLOAT2& GetMousePosition() const;
	int GetMouseXPosition();
	int GetMouseYPosition();

	const XMFLOAT2& GetMouseDelta() const;

	void SetLeftMouseButton(bool val);
	void SetMiddleMouseButton(bool val);
	void SetRightMouseButton(bool val);

private:
	bool m_CurrentKeyStates[3];
	bool m_PreviousKeyStates[3];
	int m_ScrollDelta;
	XMFLOAT2 m_prevMousePos;
	XMFLOAT2 m_MousePos;
	XMFLOAT2 m_MouseDelta;

	InputSystem* m_inputSystem;
};