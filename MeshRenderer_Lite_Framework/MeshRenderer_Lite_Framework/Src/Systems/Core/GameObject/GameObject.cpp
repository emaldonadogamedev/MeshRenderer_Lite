#include <Utilities/precompiled.h>
#include <Systems/Core/GameObject/GameObject.h>

GameObject::GameObject(bool active, const std::string& name, const std::string& tag):
	m_isActive(active),
	m_name(name),
	m_tag(tag)
{

}

GameObject::~GameObject()
{
}

IComponent* GameObject::GetComponent(ComponentType mType) const
{
	if(m_components[(unsigned char)mType].size())
		return m_components[(unsigned char)mType][0];

	return nullptr;
}

const std::vector<IComponent*>& GameObject::GetComponents(ComponentType mType) const
{
	return m_components[(unsigned char)mType];
}

void GameObject::AddComponent(IComponent* component)
{
	if (component)
	{
		m_components[(unsigned char)component->GetComponentType()].emplace_back(component);
	}
}
