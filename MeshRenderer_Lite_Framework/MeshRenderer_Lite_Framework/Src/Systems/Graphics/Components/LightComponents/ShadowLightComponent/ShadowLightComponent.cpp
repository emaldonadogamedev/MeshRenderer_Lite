#include<Utilities/precompiled.h>
#include<Systems/Graphics/Components/LightComponents/Light.h>
#include<Systems/Graphics/Components/LightComponents/ShadowLightComponent/ShadowLightComponent.h>

ShadowLightComponent::ShadowLightComponent(const GameObject* owner, bool isActive, bool useShadows, 
		const int shadowWidthHeight) :
	IComponent(ComponentType::RENDERABLE_LIGHT_WITH_SHADOW, owner, isActive)
	, m_useShadows(useShadows)
	, m_shadowRThandle()
	, m_shadowMapWidthHeight(shadowWidthHeight)
{
		int textureIdx = 5;
		for (unsigned int i = 0; i < s_maxLights; ++i)
		{
				if (!sceneLightsWithShadows[i].isTaken)
				{
						m_light = &sceneLightsWithShadows[i];
						m_light->isTaken = 1;
						m_light->isUsingShadows = static_cast<int>(m_useShadows);
						m_shadowRThandle = &shadowMapHandles[i];
						m_shadowTextureIdx = textureIdx;
						return;
				}

				++textureIdx;
		}

		throw std::exception("Allocated more lights than possible!");
}

ShadowLightComponent::~ShadowLightComponent()
{
	m_light->isTaken = 0;
}

Light* ShadowLightComponent::GetLight() const
{
		return m_light;
}

bool ShadowLightComponent::IsUsingShadows() const
{
	return m_useShadows;
}

ObjectHandle& ShadowLightComponent::GetShadowRThandle()
{
		return *m_shadowRThandle;
}

const ObjectHandle& ShadowLightComponent::GetShadowRThandle() const
{
	return *m_shadowRThandle;
}

const int ShadowLightComponent::GetShadowTextureIdx() const
{
		return m_shadowTextureIdx;
}

void ShadowLightComponent::SetUseShadows(const bool v)
{
	m_useShadows = v;
}

const unsigned int ShadowLightComponent::s_maxLights = 15;

const Light* const ShadowLightComponent::GetSceneLightsWithShadowPtr()
{
	return sceneLightsWithShadows;
}

int ShadowLightComponent::GetActiveLightsWithShadowCount()
{
	return s_takenLightCount;
}

int ShadowLightComponent::s_takenLightCount = 0;

ObjectHandle ShadowLightComponent::shadowMapHandles[s_maxLights] = { ObjectHandle::Null()};

Light ShadowLightComponent::sceneLightsWithShadows[s_maxLights];

