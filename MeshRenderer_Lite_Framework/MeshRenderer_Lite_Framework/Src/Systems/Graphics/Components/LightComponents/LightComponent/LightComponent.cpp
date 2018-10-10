#include<Utilities/precompiled.h>
#include<Systems/Graphics/Components/LightComponents/Light.h>
#include<Systems/Graphics/Components/LightComponents/LightComponent/LightComponent.h>

#include<Systems/Core/GameObject/GameObject.h>
#include<Systems/Core/Components/Transform/Transform.h>

LightComponent::LightComponent(const GameObject* owner, bool isActive)
		:IComponent(ComponentType::RENDERABLE_LIGHT, owner, isActive)
{
		for (unsigned int i = 0; i < s_maxSimpleLights; ++i)
		{
				if (!sceneLightsNoShadows[i].isTaken)
				{
						m_light = &sceneLightsNoShadows[i];
						m_light->isTaken = 1;
						m_light->isActive = isActive;
						++s_takenLightCount;
						return;
				}
		}

		throw std::exception("Allocated more simple lights than possible!");
}

LightComponent::~LightComponent()
{
		--s_takenLightCount;
		m_light->isTaken = 0;
}

SimpleLight* LightComponent::GetLight() const
{
		return m_light;
}

void LightComponent::SetLightRange(const float r)
{
		const auto lightTransform = (Transform*)GetOwner()->GetComponent(ComponentType::TRANSFORM);
		lightTransform->SetScale(r,r,r);

		m_light->m_range = r;
}

const SimpleLight* const LightComponent::GetSceneLightsNoShadowPtr()
{
		return sceneLightsNoShadows;
}

int LightComponent::GetActiveLightsNoShadowCount()
{
		return s_takenLightCount;
}

int LightComponent::s_takenLightCount = 0;
SimpleLight LightComponent::sceneLightsNoShadows[s_maxSimpleLights] = {};
