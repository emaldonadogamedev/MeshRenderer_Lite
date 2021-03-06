#pragma once

class GameObject;

enum class ComponentType :int 
{
	//List the components here after creating them 
	//Core
	TRANSFORM,

	//Graphics
	RENDERABLE_3D,
	RENDERABLE_LIGHT,
	RENDERABLE_LIGHT_WITH_SHADOW,
	RENDERABLE_PARTICLE_SYSTEM,
	RENDERABLE_PATH,
	RENDERABLE_CURVE_PATH,

	//Physics
	PHYSICS_IK_CCD,
	PHYSICS_SIMPLE_CLOTH,

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