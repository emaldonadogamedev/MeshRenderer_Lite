#pragma once

#include <vector>

class GameObject;

class IScene
{
public:
	IScene();
	~IScene();

private:
	std::vector<GameObject*> m_gameObjects;
};