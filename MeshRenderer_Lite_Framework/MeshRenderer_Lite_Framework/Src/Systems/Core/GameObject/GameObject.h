#pragma once

#include <Systems/Core/Components/IComponent.h>

#include <vector>

class GameObject
{
public:
	GameObject(bool active = true, const std::string& name = "", const std::string& tag = "", GameObject* const parent = nullptr);
	~GameObject();

	//public methods
	IComponent * GetComponent(const ComponentType type) const;
	const GameObject* const GetParent()const;

	void AddComponent(IComponent* component);

	bool m_isActive;
	int m_layerNumber;

	std::string m_name;

	bool m_useTag;
	std::string m_tag;

private:
	IComponent* m_components[(int)ComponentType::COUNT] = { nullptr };
	std::vector<GameObject*> m_children;

	GameObject* m_parent;
};