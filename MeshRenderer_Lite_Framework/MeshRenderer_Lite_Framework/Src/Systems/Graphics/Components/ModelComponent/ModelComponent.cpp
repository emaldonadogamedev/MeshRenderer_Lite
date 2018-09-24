#include<Utilities/precompiled.h>
#include<Systems/Graphics/Components/ModelComponent/ModelComponent.h>

#include<Systems/Core/Components/Transform/Transform.h>
#include<Systems/Core/GameObject/GameObject.h>


ModelComponent::ModelComponent(const GameObject* owner)
	:IComponent(ComponentType::RENDERABLE_3D, owner)
{

}

ModelComponent::~ModelComponent()
{

}

const XMMATRIX& ModelComponent::GetWorldTransform() const
{
	auto* const transform = static_cast<Transform*>(m_owner->GetComponent(ComponentType::TRANSFORM));

	if (transform)
	{
		return transform->GetWorldTransform();
	}

	return XMMatrixIdentity();
}

Model* ModelComponent::GetModel() const
{
	return m_model;
}

void ModelComponent::SetModel(Model* model)
{
	m_model = model;
}

const bool ModelComponent::GetIsVisible() const
{
	return m_isVisible;
}

void ModelComponent::SetIsVisible(const bool visible)
{
	m_isVisible = visible;
}
