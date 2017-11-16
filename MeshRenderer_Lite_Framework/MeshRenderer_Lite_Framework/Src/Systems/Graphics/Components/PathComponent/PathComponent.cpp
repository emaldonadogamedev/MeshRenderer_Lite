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

const XMVECTOR PathComponent::UpdateSplinePoint(const float dt)
{
	m_tValue += m_tValueIncrease * dt;

	const XMVECTOR& P0 = m_controlPoints[m_currentP0_index];
	const XMVECTOR& P1 = m_controlPoints[m_currentP1_index];
	const XMVECTOR& P2 = m_controlPoints[m_currentP2_index];
	const XMVECTOR& P3 = m_controlPoints[m_currentP3_index];

	const float x = 0.5f * ( (2.0f, P1.m128_f32[0]) + \
		( -P0.m128_f32[0] + P2.m128_f32[0])  * + 
		);

	return result;
}

void PathComponent::DefaultPointSet()
{
	m_controlPoints.clear();
	m_controlPoints.resize(15);

	const float scale = 10.0f;
	const float angleIncrease = DirectX::XM_2PI / 15.0f;

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
	if (++m_currentP3_index >= m_controlPoints.size())
	{
		m_currentP3_index = 0;

	}
}