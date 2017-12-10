#pragma once

#include <Systems/Core/Components/IComponent.h>
#include <Systems/Graphics/GraphicsUtilities/ObjectHandle.h>
#include <DirectXMath.h>

using DirectX::XMVECTOR;
struct aiScene;

class SuperSimpleCCD : public IComponent
{
public:
	SuperSimpleCCD(const GameObject* owner);
	~SuperSimpleCCD();

private:
	void Update(const float dt);

	float RandFloat(float minValue = 0, float maxValue = 1.0f) const;

	XMVECTOR m_eePos;
	float m_jointRotationSpeed = 1.0f;

	friend class ImGuiStage;
	friend class GraphicsSystem;
};