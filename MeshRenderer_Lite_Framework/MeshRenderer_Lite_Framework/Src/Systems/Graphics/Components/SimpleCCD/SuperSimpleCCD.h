#pragma once

#include <Systems/Core/Components/IComponent.h>
#include <Systems/Graphics/GraphicsUtilities/ObjectHandle.h>
#include <DirectXMath.h>

using DirectX::XMVECTOR;
struct aiScene;
struct aiNode;

class SuperSimpleCCD : public IComponent
{
public:
	SuperSimpleCCD(const GameObject* owner);
	~SuperSimpleCCD();

	//This is fetched from the Model Component
	const aiScene* m_assimpScene = nullptr;
	const aiNode* m_endeEffectorNode = nullptr;

private:
	void Update(const float dt);

	void FindEndEffector();

	float Clamp(const float value, const float minValue = 0.0f, const float maxValue = 1.0f) const;
	float RandFloat(const float minValue = 0.f, const float maxValue = 1.0f) const;

	XMVECTOR m_targetPos;
	float m_walkSpeed = 1.0f;
	float m_jointRotationSpeed = 1.0f;

	friend class ImGuiStage;
	friend class GraphicsSystem;
};