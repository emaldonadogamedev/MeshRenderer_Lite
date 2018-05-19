#include<Utilities/precompiled.h>
#include<Systems/Graphics/Components/LightComponent/LightComponent.h>

LightComponent::LightComponent(const GameObject* owner, bool isActive /*= true*/, bool useShadows /*= false*/) : IComponent(ComponentType::RENDERABLE_LIGHT, owner, isActive)
	, m_light(nullptr)
{
	for (unsigned int i = 0; i < s_maxLights; ++i) 
	{
		if (!sceneLights[i].isTaken) 
		{
			sceneLights[i].isTaken = 1;

			m_light = &sceneLights[i];
			return;
		}
	}
	
	throw std::exception("Allocated more lights than possible!");
}

LightComponent::~LightComponent()
{
	m_light->isTaken = 0;
}

Light* LightComponent::GetLight() const
{
	return m_light;
}

bool LightComponent::IsUsingShadows() const
{
	return m_useShadows;
}

void LightComponent::SetUseShadows(const bool v)
{
	m_useShadows = v;
}

const unsigned int LightComponent::s_maxLights = 15;

Light* const LightComponent::GetSceneLightsPtr()
{
	return sceneLights;
}

Light LightComponent::sceneLights[s_maxLights];
