#pragma once

class GameObject;

enum class ComponentType : char 
{
	//List the components here after creating them 
	//Core
	TRANSFORM,

	//Graphics
	RENDERABLE,
	
	//Physics

	//Scripting
	LUA_SCRIPT,

	COUNT
};

class IComponent 
{
public:
	//constructors/destructors
	IComponent(const ComponentType type, const GameObject* owner):m_componentType(type), m_owner(owner){}
	virtual ~IComponent() {};

	//Getters/Setters
	const ComponentType GetComponentType() const 
	{ 
		return m_componentType; 
	}

	const GameObject* GetOwner() const 
	{ 
		return m_owner; 
	}

	bool GetIsActive() const 
	{ 
		return m_isActive; 
	}
	void SetIsActive(const bool active) 
	{ 
		m_isActive = active; 
	}

protected:
	const ComponentType m_componentType;
	const GameObject* m_owner;
	bool m_isActive;
};