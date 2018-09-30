#pragma once

#include <Systems/Core/Components/IComponent.h>

struct SimpleLight;

class LightComponent : public IComponent
{
public:
		LightComponent(const GameObject* owner, bool isActive = true);
		virtual ~LightComponent();

		SimpleLight* GetLight() const;
		void SetLightRange(const float r);

		static const unsigned int s_maxLights;

		static const SimpleLight* const GetSceneLightsNoShadowPtr();
		static int GetActiveLightsNoShadowCount();
protected:		
		SimpleLight* m_light;

		static int s_takenLightCount;
		static SimpleLight sceneLightsNoShadows[];
};