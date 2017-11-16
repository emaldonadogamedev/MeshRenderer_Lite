#pragma once

class GameObject;

enum class ComponentType : char 
{
	//List the components here after creating them 
	//Core
	TRANSFORM,

	//Graphics
	RENDERABLE_3D,
	RENDERABLE_LIGHT,
	RENDERABLE_PARTICLE_SYSTEM,
	RENDERABLE_PATH,
	
	//Physics

	//Scripting
	LUA_SCRIPT,

	COUNT
};

class IComponent 
{
public:
	//constructors/destructors
	IComponent(const ComponentType type, const GameObject* owner, bool isActive = true):m_componentType(type), m_owner(owner),m_isActive(isActive){}
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