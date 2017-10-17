#include<Utilities/precompiled.h>
#include<Systems/Input/Keyboard.h>

void KeyboardEvent::Update(void)
{
	//Stores the previous key state
	memcpy(m_PreviousKeyStates, m_CurrentKeyStates, sizeof(bool) * VirtualKey::NUM_KEY_CODES);
}

void KeyboardEvent::Clear(void)
{
	memset(m_CurrentKeyStates, false, sizeof(bool) * VirtualKey::NUM_KEY_CODES);
	memset(m_PreviousKeyStates, false, sizeof(bool) * VirtualKey::NUM_KEY_CODES);
}

bool KeyboardEvent::IsKeyPressed(VirtualKey key)
{
	return (!m_PreviousKeyStates[key] && m_CurrentKeyStates[key]);
}

bool KeyboardEvent::IsKeyHeld(VirtualKey key)
{
	return (m_PreviousKeyStates[key] && m_CurrentKeyStates[key]);
}

bool KeyboardEvent::IsKeyReleased(VirtualKey key)
{
	return (m_PreviousKeyStates[key] && !m_CurrentKeyStates[key]);
}

bool KeyboardEvent::IsKeyPressed(int key)
{
	return IsKeyPressed(static_cast<VirtualKey>(key));
}

bool KeyboardEvent::IsKeyHeld(int key)
{
	return  IsKeyHeld(static_cast<VirtualKey>(key));
}

bool KeyboardEvent::IsKeyReleased(int key)
{
	return  IsKeyReleased(static_cast<VirtualKey>(key));
}

void KeyboardEvent::SetKey(int key, bool val)
{
	m_CurrentKeyStates[key] = val;
}