#include <Utilities/precompiled.h>
#include <Systems/Core/GameObject/GameObject.h>

GameObject::GameObject(bool active, const std::string& name, const std::string& tag, GameObject* const parent):
	m_isActive(active),
	m_name(name),
	m_tag(tag)
{

}

GameObject::~GameObject()
{
}

IComponent* GameObject::GetComponent(const ComponentType type) const
{
	return m_components[(unsigned char)type];
}

const GameObject* const GameObject::GetParent() const
{
	return m_parent;
}

void GameObject::AddComponent(IComponent* component)
{
	if (component)
	{
		m_components[(unsigned char)component->GetComponentType()] = component;
	}
}
