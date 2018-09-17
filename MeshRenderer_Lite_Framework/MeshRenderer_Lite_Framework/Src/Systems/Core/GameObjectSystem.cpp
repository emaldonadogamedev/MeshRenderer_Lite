#include <Utilities/precompiled.h>
#include <Systems/Core/GameObjectSystem.h>

#include <Systems/Core/Components/Transform/Transform.h>

bool GameObjectSystem::Initialize()
{
		return true;
}

void GameObjectSystem::Update(const float dt)
{
		for (auto* component : m_transformComponents)
		{
				Transform* transform = (Transform*)component;
				transform->UpdateWorldMatrix();
		}
}

void GameObjectSystem::Shutdown()
{
}

void GameObjectSystem::ReceiveMessage(const IMessage& msg)
{
}

void GameObjectSystem::AddComponent(IComponent* component)
{
		if (component->GetComponentType() == ComponentType::TRANSFORM)
		{
				m_transformComponents.emplace_back(component);
		}
}