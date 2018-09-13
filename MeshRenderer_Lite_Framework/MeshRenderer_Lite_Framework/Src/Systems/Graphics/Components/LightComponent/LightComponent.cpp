#include<Utilities/precompiled.h>
#include<Systems/Graphics/Components/LightComponent/LightComponent.h>

LightComponent::LightComponent(const GameObject* owner, bool isActive, bool useShadows, 
		const int shadowWidth, const int shadowHeight) :
	IComponent(ComponentType::RENDERABLE_LIGHT, owner, isActive)
	, m_light(nullptr)
	, m_useShadows(useShadows)
	, m_shadowRThandle()
	, m_shadowMapWidth(shadowWidth)
	, m_shadowMapHeight(shadowHeight)
{
	int textureIdx = 5;
	for (unsigned int i = 0; i < s_maxLights; ++i) 
	{
		if (!sceneLights[i].isTaken) 
		{
			sceneLights[i].isTaken = 1;
			m_light = &sceneLights[i];
			m_shadowRThandle = &shadowMapHandles[i];
			m_shadowTextureIdx = textureIdx;
			return;
		}

		textureIdx++;
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

ObjectHandle& LightComponent::GetShadowRThandle()
{
		return *m_shadowRThandle;
}

const ObjectHandle& LightComponent::GetShadowRThandle() const
{
	return *m_shadowRThandle;
}

const int LightComponent::GetShadowTextureIdx() const
{
		return m_shadowTextureIdx;
}

void LightComponent::SetUseShadows(const bool v)
{
	m_useShadows = v;
}

const unsigned int LightComponent::s_maxLights = 15;

const Light* const LightComponent::GetSceneLightsPtr()
{
	return sceneLights;
}

int LightComponent::GetTakenLightsCount()
{
	return s_takenLightCount;
}

int LightComponent::s_takenLightCount = 0;

ObjectHandle LightComponent::shadowMapHandles[s_maxLights] = { ObjectHandle::Null()};

Light LightComponent::sceneLights[s_maxLights];

