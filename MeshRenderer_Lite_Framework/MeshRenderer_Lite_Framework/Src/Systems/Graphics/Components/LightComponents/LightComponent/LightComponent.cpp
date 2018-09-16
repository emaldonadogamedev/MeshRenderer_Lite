#include<Utilities/precompiled.h>
#include<Systems/Graphics/Components/LightComponents/Light.h>
#include<Systems/Graphics/Components/LightComponents/LightComponent/LightComponent.h>

LightComponent::LightComponent(const GameObject* owner, bool isActive)
		:IComponent(ComponentType::RENDERABLE_LIGHT, owner, isActive)
{
		m_light = new Light();
		m_light->isTaken = 1;
		m_light->isUsingShadows = 0;
}

LightComponent::~LightComponent()
{
		SafeDelete(m_light);
}

Light* LightComponent::GetLight() const
{
		return m_light;
}