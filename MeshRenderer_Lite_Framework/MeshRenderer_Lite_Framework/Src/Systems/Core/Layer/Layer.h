#pragma once

#include<string>
#include<vector>

class GameObject;

class Layer
{
public:
	Layer();
	virtual ~Layer();

	//Getters
	bool GetIsActive()const;
	bool GetIsVisible()const;
	bool GetIsStatic() const;
	int GetLayerNumber() const;
	float GetOpacity()const;

	//Setters
	void SetIsActive(bool b);
	void SetIsVisible(bool b);
	void SetIsStastic(bool b);
	void SetOpacity(float t);

	//public methods
	void AddGameObject(GameObject* gameObject);
	void RemoveGameObject(GameObject *gameObject);

protected:
	std::string m_name;
	bool m_isActive;
	bool m_isVisible;
	bool m_isStatic;
	float m_opacity;
	const int m_layerNumber;

	std::vector<GameObject*> m_gameObjects;
	//std::multiset<GameObject*> m_gameObjects;
};