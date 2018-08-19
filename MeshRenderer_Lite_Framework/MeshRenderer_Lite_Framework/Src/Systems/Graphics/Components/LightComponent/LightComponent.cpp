#include<Utilities/precompiled.h>
#include<Systems/Graphics/Components/LightComponent/LightComponent.h>

LightComponent::LightComponent(const GameObject* owner, bool isActive, bool useShadows) : 
	IComponent(ComponentType::RENDERABLE_LIGHT, owner, isActive)
	, m_light(nullptr)
	, m_useShadows(useShadows)
	, m_shadowRThandle()
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

const ObjectHandle& LightComponent::GetShadowRThandle() const
{
	return m_shadowRThandle;
}

void LightComponent::SetUseShadows(const bool v)
{
	m_useShadows = v;
}

void LightComponent::SetShadowRThandle(const ObjectHandle& shadowRT)
{
	m_shadowRThandle = shadowRT;
}

const unsigned int LightComponent::s_maxLights = 15;

Light* const LightComponent::GetSceneLightsPtr()
{
	return sceneLights;
}

int LightComponent::GetTakenLightsCount()
{
	return s_takenLightCount;
}

int LightComponent::s_takenLightCount = 0;

const int LightComponent::ShadowMapIndices[s_maxLights] = {0};

Light LightComponent::sceneLights[s_maxLights];
