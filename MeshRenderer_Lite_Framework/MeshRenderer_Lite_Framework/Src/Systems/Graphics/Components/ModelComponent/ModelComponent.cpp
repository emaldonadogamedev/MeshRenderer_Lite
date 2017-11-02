#include<Utilities/precompiled.h>
#include<Systems/Graphics/Components/ModelComponent/ModelComponent.h>

#include<Systems/Core/Components/Transform/Transform.h>
#include<Systems/Core/GameObject/GameObject.h>


ModelComponent::ModelComponent(const GameObject* owner)
	:IRenderComponent(RenderComponentType::RENDERABLE_3D, owner)
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
		transform->UpdateWorldMatrix();
		return transform->GetWorldTransform();
	}

	return XMMatrixIdentity();
}

const Model* ModelComponent::GetModel() const
{
	return m_model;
}

void ModelComponent::SetModel(const Model* model)
{
	m_model = model;
}
