#pragma once

#include <Systems/Core/Components/IComponent.h>

enum class RenderComponentType : char
{
	RENDERABLE_3D,
	RENDERABLE_LIGHT,
	RENDERABLE_PARTICLE_SYSTEM,
	COUNT,
};

class IRenderComponent : public IComponent
{
public:
	IRenderComponent(RenderComponentType renderableType, const GameObject* owner)
		:IComponent(ComponentType::RENDERABLE, owner)
		,m_renderCompType(renderableType)
	{
	}

	virtual ~IRenderComponent() override
	{}

protected:
	const RenderComponentType m_renderCompType;

};