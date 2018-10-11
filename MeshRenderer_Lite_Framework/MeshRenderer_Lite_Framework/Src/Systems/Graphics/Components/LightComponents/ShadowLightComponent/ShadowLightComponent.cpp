#include<Utilities/precompiled.h>
#include<Systems/Graphics/Components/LightComponents/Light.h>
#include<Systems/Graphics/Components/LightComponents/ShadowLightComponent/ShadowLightComponent.h>

ShadowLightComponent::ShadowLightComponent(const GameObject* owner, bool isActive, bool useShadows, 
		const int shadowWidthHeight) :
	IComponent(ComponentType::RENDERABLE_LIGHT_WITH_SHADOW, owner, isActive)
	, m_useShadows(useShadows)
	, m_shadowDepthMapHandle()
	, m_shadowMapWidthHeight(shadowWidthHeight)
{
		int textureIdx = 5;
		for (unsigned int i = 0; i < s_maxShadowLights; ++i)
		{
				if (!sceneLightsWithShadows[i].isTaken)
				{
						m_light = &sceneLightsWithShadows[i];
						m_light->isTaken = 1;
						m_light->isUsingShadows = static_cast<int>(m_useShadows);
						m_shadowDepthMapHandle = &shadowMapHandles[i];
						m_shadowTextureIdx = textureIdx;
						m_viewProj = &shadowLightViewProjBuffers[i];
						
						++s_takenLightCount;
						return;
				}

				++textureIdx;
		}

		throw std::exception("Allocated more shadow lights than possible!");
}

ShadowLightComponent::~ShadowLightComponent()
{
		m_light->isTaken = 0;
		m_light->isActive = 0;
		--s_takenLightCount;
}

Light* ShadowLightComponent::GetLight() const
{
		return m_light;
}

LightViewProj* ShadowLightComponent::GetLightViewProjBuffer() const
{
		return m_viewProj;
}

bool ShadowLightComponent::IsUsingShadows() const
{
	return m_useShadows;
}

ObjectHandle& ShadowLightComponent::GetShadowDepthMapHandle()
{
		return *m_shadowDepthMapHandle;
}

const ObjectHandle& ShadowLightComponent::GetShadowDepthMapHandle() const
{
	return *m_shadowDepthMapHandle;
}

const int ShadowLightComponent::GetShadowTextureIdx() const
{
		return m_shadowTextureIdx;
}

void ShadowLightComponent::SetUseShadows(const bool v)
{
	m_useShadows = v;
}

const Light* const ShadowLightComponent::GetSceneLightsWithShadowPtr()
{
	return sceneLightsWithShadows;
}

const LightViewProj* const ShadowLightComponent::GetShadowLightViewProjBuffersPtr()
{
		return shadowLightViewProjBuffers;
}

int ShadowLightComponent::GetActiveLightsWithShadowCount()
{
	return s_takenLightCount;
}

LightViewProj ShadowLightComponent::shadowLightViewProjBuffers[s_maxShadowLights];
int ShadowLightComponent::s_takenLightCount = 0;
ObjectHandle ShadowLightComponent::shadowMapHandles[s_maxShadowLights] = { ObjectHandle::Null()};
Light ShadowLightComponent::sceneLightsWithShadows[s_maxShadowLights];