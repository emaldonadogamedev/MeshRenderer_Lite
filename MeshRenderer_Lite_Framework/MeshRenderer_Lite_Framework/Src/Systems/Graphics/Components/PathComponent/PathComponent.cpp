#include <Utilities/precompiled.h>
#include <Systems/Graphics/Components/PathComponent/PathComponent.h>

#include <Systems/Graphics/DX11Renderer/DX11Renderer.h>

using DirectX::XMVectorSet;

PathComponent::PathComponent(const GameObject* owner)
	:IComponent(ComponentType::RENDERABLE_PATH, owner)
	, m_pathCenterPos(XMVectorSet(0,0,0,1.0f))
{
	srand(time(NULL));

	DefaultPointSet();
	PrepareDrawPoints();
}

PathComponent::~PathComponent()
{

}

const float GetSplinePointComponent(const float t, const int i, const XMVECTOR& P0, const XMVECTOR& P1, const XMVECTOR& P2, const XMVECTOR& P3)
{
	const float result = 0.5f * (
		(2.0f * P1.m128_f32[i]) + 
		((-P0.m128_f32[i] + P2.m128_f32[i])  * t) +
		(((2.0f * P0.m128_f32[i]) - (5.0f * P1.m128_f32[i]) + (4.0f * P2.m128_f32[i]) - P3.m128_f32[i]) * (t * t)) +
		((-P0.m128_f32[i] + (3.0f * P1.m128_f32[i]) - (3.0f * P2.m128_f32[i]) + P3.m128_f32[i]) * (t * t * t))
	);

	return result;
}

const XMVECTOR PathComponent::GetCurrentSplinePoint()
{
	m_prevPos = m_currentPos;

	const XMVECTOR& P0 = m_controlPoints[m_currentP0_index];
	const XMVECTOR& P1 = m_controlPoints[m_currentP1_index];
	const XMVECTOR& P2 = m_controlPoints[m_currentP2_index];
	const XMVECTOR& P3 = m_controlPoints[m_currentP3_index];

	const float x = GetSplinePointComponent(m_tValue, 0, P0, P1, P2, P3);
	const float y = GetSplinePointComponent(m_tValue, 1, P0, P1, P2, P3);
	const float z = GetSplinePointComponent(m_tValue, 2, P0, P1, P2, P3);

	return m_currentPos = DirectX::XMVectorAdd(m_pathCenterPos, XMVectorSet(x,y,z,1.0f));
}

float PathComponent::GetCurrentAngle() const
{
	const float dx = m_currentPos.m128_f32[0] - m_prevPos.m128_f32[0];
	const float dz = m_currentPos.m128_f32[2] - m_prevPos.m128_f32[2];

	const float angle = atan2(dz, dx);

	return (angle < 0 ? DirectX::XM_2PI + angle : angle);
}

void PathComponent::DefaultPointSet()
{
	m_controlPoints.clear();

	const float scale = 250.0f;
	const float angleIncrease = DirectX::XM_2PI / (float)s_defaultAmountOfPoints;

	for (float angle = 0 ; angle < DirectX::XM_2PI; angle += angleIncrease)
	{
		m_controlPoints.emplace_back(XMVectorSet(std::cos(angle) * scale * RandFloat(0.5f, 1.0f), 0, std::sin(angle) * scale * RandFloat(0.5f, 1.0f), 1.0f));
		//m_controlPoints.emplace_back(XMVectorSet(std::cos(angle) * scale, 0, std::sin(angle) * scale, 1.0f));
	}
}

void PathComponent::PrepareDrawPoints()
{
	m_drawPoints.clear();
	for (size_t i = 0; i < m_controlPoints.size(); ++i)
	{
		m_tValue = 0;
		while (m_tValue <= 1.0f)
		{
			m_drawPoints.emplace_back(GetCurrentSplinePoint());

			m_tValue += m_tValueIncrease;
		}

		ShiftPointIndices();
	}

	//Return current t value to 0 for updating
	m_tValue = 0;
}

void PathComponent::GenerateVertexBuffer(DX11Renderer* renderContext)
{
	m_vertices.clear();
	m_vertices.resize(m_drawPoints.size());
	for (int i = 0; i < m_vertices.size(); ++i)
	{
		m_vertices[i].position = XMFLOAT3(m_drawPoints[i].m128_f32);
		m_vertices[i].color = XMFLOAT4(1.0f, 0, 1.f, 1.0f);
	}

	renderContext->CreateVertexBuffer(m_drawPointsVBuffer, BufferUsage::USAGE_DEFAULT, sizeof(VertexAnimation) * m_vertices.size(), m_vertices.data());
}

ObjectHandle PathComponent::GetPathVBuffer() const
{
	return m_drawPointsVBuffer;
}

void PathComponent::UpdatePath(const float dt)
{
	m_tValue += dt * m_tValueIncrease;
	if (m_tValue > 1.0f)
	{
		m_tValue = 0;
		ShiftPointIndices();
	}
}

int PathComponent::GetPathVertexCount() const
{
	return m_drawPoints.size();
}

float PathComponent::Clamp(const float value, const float minValue, const float maxValue) const
{
	return min( max(minValue, value), maxValue);
}

float PathComponent::RandFloat(float minValue, float maxValue) const
{
	return minValue + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX /(maxValue - minValue)));
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

const int PathComponent::s_defaultAmountOfPoints = 15;
