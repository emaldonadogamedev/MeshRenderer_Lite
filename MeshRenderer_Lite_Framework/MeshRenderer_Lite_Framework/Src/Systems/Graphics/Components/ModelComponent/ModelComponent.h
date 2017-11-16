#pragma once

#include <Systems/Core/Components/IComponent.h>

#include<DirectXMath.h>
using DirectX::XMMATRIX;

class Model;

class ModelComponent : public IComponent
{
public:
	ModelComponent(const GameObject* owner);
	virtual ~ModelComponent();

	const XMMATRIX& GetWorldTransform() const;
	
	Model* GetModel() const;
	void SetModel(Model* model);

	const bool GetIsVisible()const;
	void SetIsVisible(const bool visible);

protected:
	bool m_isVisible;
	Model* m_model = nullptr;
};
