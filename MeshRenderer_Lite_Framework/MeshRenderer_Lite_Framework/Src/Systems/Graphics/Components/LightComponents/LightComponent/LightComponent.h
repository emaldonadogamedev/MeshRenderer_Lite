#pragma once

#include <Systems/Core/Components/IComponent.h>

class Light;

class LightComponent : public IComponent
{
public:
		LightComponent(const GameObject* owner, bool isActive = true);
		virtual ~LightComponent();

		Light* GetLight() const;

protected:		
		Light* m_light;
};