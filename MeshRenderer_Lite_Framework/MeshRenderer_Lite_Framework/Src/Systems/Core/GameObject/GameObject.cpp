#include <Utilities/precompiled.h>
#include <Systems/Core/GameObject/GameObject.h>

GameObject::GameObject(bool active, const std::string& name, const std::string& tag):
	m_isActive(active),
	m_name(name),
	m_tag(tag)
{
	for (char i = 0; i < (char)ComponentType::COUNT; ++i)
		m_components[i] = nullptr;
}

GameObject::~GameObject()
{
	for (char i = 0; i < (char)ComponentType::COUNT; ++i)
	{
		SafeDelete(m_components[i]);
	}
}

IComponent* GameObject::GetComponent(ComponentType mType) const
{
	return m_components[(char)mType];
}

void GameObject::AddComponent(IComponent* component)
{
	if (component)
	{
		m_components[(char)component->GetComponentType()] = component;
	}
}
