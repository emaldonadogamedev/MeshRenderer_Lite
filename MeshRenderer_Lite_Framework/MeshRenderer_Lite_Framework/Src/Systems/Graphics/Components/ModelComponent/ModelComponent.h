#pragma once

#include <Systems/Graphics/Components/IRenderComponent.h>

class ModelComponent : public IRenderComponent
{
public:
	ModelComponent(const GameObject* owner);
	virtual ~ModelComponent();

protected:
};
