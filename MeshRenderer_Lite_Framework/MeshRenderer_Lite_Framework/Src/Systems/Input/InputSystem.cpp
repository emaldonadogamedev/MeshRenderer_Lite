#include <Utilities/precompiled.h>
#include <Systems/Input/InputSystem.h>

#include <Systems/Input/Mouse.h>
#include <Systems/Input/Keyboard.h>

InputSystem::InputSystem(IEngine* const eng):ISystem(SystemType::ST_INPUT, eng), m_mouse(nullptr), m_keyboard(nullptr)
{
}

InputSystem::~InputSystem(void)
{
}

bool InputSystem::Initialize(void)
{
	m_mouse = new MouseEvent(this);
	m_keyboard = new KeyboardEvent();

	if (!m_mouse || !m_keyboard)
		return false;

	return true;
}

void InputSystem::Shutdown(void)
{
	SafeDelete(m_mouse);
	SafeDelete(m_keyboard);
}

void InputSystem::Update(const float dt)
{
	m_mouse->Update();
	m_keyboard->Update();
}

void InputSystem::ToggleKeyInput()
{
	AllowKeyInput = !AllowKeyInput;
}

void InputSystem::SetKeyInput(bool input)
{
	AllowKeyInput = input;
}

bool InputSystem::GetKeyInput()
{
	return AllowKeyInput;
}

void InputSystem::ReceiveMessage(const IMessage& msg)
{
}
