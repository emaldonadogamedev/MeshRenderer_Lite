#pragma once

#include <Systems/ISystem.h>

class GameObjectSystem : public ISystem
{
public:
		GameObjectSystem(IApplication* const eng);
		virtual ~GameObjectSystem();

		virtual bool Initialize() override;
		virtual void Update(const float dt) override;
		virtual void Shutdown() override;

		virtual void ReceiveMessage(const IMessage& msg) override;
		virtual void AddComponent(IComponent* component) override;

protected:
		std::vector<IComponent*> m_transformComponents;
};