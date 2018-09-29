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

const unsigned int LightComponent::s_maxLights = 200;

const Light* const LightComponent::GetSceneLightsNoShadowPtr()
{
		return sceneLightsNoShadows;
}

int LightComponent::GetActiveLightsNoShadowCount()
{
		return s_takenLightCount;
}

int LightComponent::s_takenLightCount = 0;;

Light LightComponent::sceneLightsNoShadows[s_maxLights] = {};
