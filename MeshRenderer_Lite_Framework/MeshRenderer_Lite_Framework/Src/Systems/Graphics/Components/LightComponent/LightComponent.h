#pragma once

#include <Systems/Core/Components/IComponent.h>
#include <DirectXMath.h>

class Light;

using DirectX::XMVECTOR;
using DirectX::XMFLOAT3;

class LightComponent : public IComponent
{
public:
	LightComponent(const GameObject* owner, bool isActive = true)
		:IComponent(ComponentType::RENDERABLE_LIGHT, owner, isActive)
	{
	}
	virtual ~LightComponent() {}

	friend class GraphicsSystem;
	friend class GameObject;
};