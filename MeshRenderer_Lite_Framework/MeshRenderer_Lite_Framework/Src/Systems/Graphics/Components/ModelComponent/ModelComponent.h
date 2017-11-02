#pragma once

#include <Systems/Graphics/Components/IRenderComponent.h>

#include<DirectXMath.h>
using DirectX::XMMATRIX;

class Model;

class ModelComponent : public IRenderComponent
{
public:
	ModelComponent(const GameObject* owner);
	virtual ~ModelComponent();

	const XMMATRIX& GetWorldTransform() const;
	
	const Model* GetModel() const;
	void SetModel(const Model* model);

protected:

	const Model* m_model = nullptr;
};
