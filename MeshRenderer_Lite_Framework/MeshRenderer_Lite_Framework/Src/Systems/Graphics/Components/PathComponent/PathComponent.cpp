#include <Utilities/precompiled.h>
#include <Systems/Graphics/Components/PathComponent/PathComponent.h>

using DirectX::XMVectorSet;

PathComponent::PathComponent(const GameObject* owner)
	:IComponent(ComponentType::RENDERABLE_PATH, owner)
{

}

PathComponent::~PathComponent()
{

}

const float UpdateSplinePointComponent(const float t, const int i, const XMVECTOR& P0, const XMVECTOR& P1, const XMVECTOR& P2, const XMVECTOR& P3)
{
	const float result = 0.5f * (
		(2.0f * P1.m128_f32[i]) + 
		((-P0.m128_f32[i] + P2.m128_f32[i])  * t) +
		(((2.0f * P0.m128_f32[i]) - (5.0f * P1.m128_f32[i]) + (4.0f * P2.m128_f32[i]) - P3.m128_f32[i]) * (t * t)) +
		((-P0.m128_f32[i] + (3.0f * P1.m128_f32[i]) - (3.0f * P2.m128_f32[i]) + P3.m128_f32[i]) * (t * t * t))
	);

	return result;
}

const XMVECTOR PathComponent::UpdateSplinePoint(const float dt)
{
	m_tValue += m_tValueIncrease * dt;

	const XMVECTOR& P0 = m_controlPoints[m_currentP0_index];
	const XMVECTOR& P1 = m_controlPoints[m_currentP1_index];
	const XMVECTOR& P2 = m_controlPoints[m_currentP2_index];
	const XMVECTOR& P3 = m_controlPoints[m_currentP3_index];

	const float x = UpdateSplinePointComponent(m_tValue, 0, P0, P1, P2, P3);
	const float y = UpdateSplinePointComponent(m_tValue, 1, P0, P1, P2, P3);
	const float z = UpdateSplinePointComponent(m_tValue, 2, P0, P1, P2, P3);

	if (m_tValue >= 1.0f)
		ShiftPointIndices();

	return XMVectorSet(x,y,z,1.0f);
}

void PathComponent::DefaultPointSet()
{
	m_controlPoints.clear();
	m_controlPoints.resize(15);

	const float scale = 10.0f;
	const float angleIncrease = DirectX::XM_2PI / (float)m_controlPoints.size();

	for (float angle = 0 ; angle < DirectX::XM_2PI; angle += angleIncrease)
	{
		XMVECTOR point = XMVectorSet(std::cos(angle) * scale, 0, std::sin(angle) * scale, 1.0f);
	}

	m_currentP0_index = 0;
	m_currentP3_index = 3;
}

float PathComponent::Clamp(const float value, const float minValue, const float maxValue)
{
	return min( max(minValue, value), maxValue);
}

void PathComponent::ShiftPointIndices()
{
	const int pointCount = m_controlPoints.size();
	++m_currentP3_index;
	if (m_currentP3_index >= pointCount)
	{
		m_currentP3_index = 0;
		m_currentP2_index = pointCount - 1;
		m_currentP1_index = pointCount - 2;
		m_currentP0_index = pointCount - 3;
		return;
	}

	else if (m_currentP3_index == 1)
	{
		m_currentP2_index = 0;
		m_currentP1_index = pointCount - 1;
		m_currentP0_index = pointCount - 2;
		return;
	}

	else if (m_currentP3_index == 2)
	{
		m_currentP2_index = 1;
		m_currentP1_index = 0;
		m_currentP0_index = pointCount - 1;
		return;
	}

	m_currentP2_index = m_currentP3_index - 1;
	m_currentP1_index = m_currentP3_index - 2;
	m_currentP0_index = m_currentP3_index - 3;

}