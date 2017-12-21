#pragma once

#include <Systems/Core/ISystem.h>

class KeyboardEvent;
class MouseEvent;

class InputSystem : public ISystem
{
public:
	InputSystem(IEngine* const eng);
	virtual ~InputSystem(void);

	virtual bool Initialize(void) override;
	virtual void Shutdown(void) override;
	virtual void Update(const float dt) override;
	virtual void ReceiveMessage(const IMessage& msg) override;

	void ToggleKeyInput(void);
	void SetKeyInput(bool input);
	bool GetKeyInput(void);

	bool AllowKeyInput = false;

	KeyboardEvent* m_keyboard;
	MouseEvent* m_mouse;

	friend KeyboardEvent;
	friend MouseEvent;
};
