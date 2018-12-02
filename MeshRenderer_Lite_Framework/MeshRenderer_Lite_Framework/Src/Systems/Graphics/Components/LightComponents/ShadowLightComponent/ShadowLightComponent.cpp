#include<Utilities/precompiled.h>
#include<Systems/Graphics/Components/LightComponents/Light.h>
#include<Systems/Graphics/Components/LightComponents/ShadowLightComponent/ShadowLightComponent.h>
#include <Systems/Graphics/GraphicsUtilities/ObjectHandle.h>

ShadowLightComponent::ShadowLightComponent(const GameObject* owner, bool isActive, bool useShadows, bool useSoftShadows,
		const int shadowWidthHeight) :
	IComponent(ComponentType::RENDERABLE_LIGHT_WITH_SHADOW, owner, isActive)
	, m_useShadows(useShadows)
  , m_useSoftShadows(useSoftShadows)
	, m_shadowDepthMapHandle()
	, m_shadowMapWidthHeight(shadowWidthHeight)
	, m_softShadowMapKernelHalfWidth(6)
{
	int textureIdx = 5;
	for (unsigned int i = 0; i < s_maxShadowLights; ++i)
	{
		if (!sceneLightsWithShadows[i].isTaken)
		{
			m_light = &sceneLightsWithShadows[i];
			m_light->isTaken = 1;
			m_light->isUsingShadows = static_cast<int>(m_useShadows);
			m_light->isUsingSoftShadows = static_cast<int>(m_useSoftShadows);
			
			m_shadowDepthMapHandle = &shadowMapHandles[i];
			m_shadowDepthMapHandle->MakeNull();
			
			m_softShadowDepthMapHandle = &softShadowMapHandles[i];
			m_softShadowDepthMapHandle->MakeNull();

			m_softShadowKernelWeightsHandle = &softShadowKernelWeightHandles[i];
			m_softShadowKernelWeightsHandle->MakeNull();
			
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

const XMVECTOR& ShadowLightComponent::GetUpVector() const
{
	return m_upVec;
}

const XMVECTOR& ShadowLightComponent::GetRightVector() const
{
	return m_rightVec;
}

const XMVECTOR& ShadowLightComponent::GetLookAtVector() const
{
	return m_lookAtVec;
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

ObjectHandle& ShadowLightComponent::GetSoftShadowDepthMapHandle()
{
	return *m_softShadowDepthMapHandle;
}

const ObjectHandle& ShadowLightComponent::GetSoftShadowDepthMapHandle() const
{
	return *m_softShadowDepthMapHandle;
}

ObjectHandle& ShadowLightComponent::GetSoftShadowMapKernelWeightHandle()
{
		return *m_softShadowKernelWeightsHandle;
}

const ObjectHandle& ShadowLightComponent::GetSoftShadowMapKernelWeightHandle() const
{
		return *m_softShadowKernelWeightsHandle;
}

int ShadowLightComponent::GetSoftShadowMapKernelHalfWidth() const
{
		return m_softShadowMapKernelHalfWidth;
}

const int ShadowLightComponent::GetShadowTextureIdx() const
{
		return m_shadowTextureIdx;
}

void ShadowLightComponent::SetUseShadows(const bool v)
{
	m_useShadows = v;
}

bool ShadowLightComponent::IsUsingSoftShadows() const
{
		return m_useSoftShadows;
}

void ShadowLightComponent::SetUseSoftShadows(const bool v)
{
		m_useSoftShadows = v;
}

int ShadowLightComponent::GetShadowMapDimension() const
{
		return m_shadowMapWidthHeight;
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

ObjectHandle ShadowLightComponent::softShadowKernelWeightHandles[s_maxShadowLights] = { ObjectHandle::Null() };
ObjectHandle ShadowLightComponent::shadowMapHandles[s_maxShadowLights] = { ObjectHandle::Null() };
ObjectHandle ShadowLightComponent::softShadowMapHandles[s_maxShadowLights] = { ObjectHandle::Null() };
Light ShadowLightComponent::sceneLightsWithShadows[s_maxShadowLights];