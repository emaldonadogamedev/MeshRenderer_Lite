#pragma once

#include <vector>

class GameObject;
class Layer;

class Scene
{
public:
	Scene();
	virtual ~Scene();

private:
	std::vector<Layer*> m_sceneLayers;
	std::vector<GameObject*> m_gameObjects;
};