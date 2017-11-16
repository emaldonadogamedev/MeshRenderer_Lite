#pragma once

#include <Systems/Core/Components/IComponent.h>
#include <DirectXMath.h>
#include <deque>

using DirectX::XMVECTOR;

class PathComponent : public IComponent
{
public:
	PathComponent(const GameObject* owner);
	virtual ~PathComponent();

	const XMVECTOR UpdateSplinePoint(const float dt);

	float m_tValueIncrease = 0.33f;
	float m_speed = 3.0f;
	std::deque<XMVECTOR> m_controlPoints;

private:
	void DefaultPointSet();
	float Clamp(const float value, const float minValue = 0.0f, const float maxValue = 1.0f);
	void ShiftPointIndices();

	float m_tValue = 0.0f;
	size_t m_currentP0_index = 0;
	size_t m_currentP1_index = 1;
	size_t m_currentP2_index = 2;
	size_t m_currentP3_index = 3;
};