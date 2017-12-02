#include <Utilities/precompiled.h>
#include <Systems/Graphics/Components/CurvePathComponent/CurvePathComponent.h>

#include <Systems/Graphics/DX11Renderer/DX11Renderer.h>

using DirectX::XMVectorSet;

CurvePathComponent::CurvePathComponent(const GameObject* owner)
	:IComponent(ComponentType::RENDERABLE_PATH, owner)
	, m_pathCenterPos(XMVectorSet(0,0,0,1.0f))
{
	srand(time(NULL));

	DefaultPointSet();
	PrepareDrawPoints();
}

CurvePathComponent::~CurvePathComponent()
{

}

const XMVECTOR CurvePathComponent::GetCurrentSplinePoint()
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

float CurvePathComponent::GetCurrentAngle() const
{
	const float dx = m_currentPos.m128_f32[0] - m_prevPos.m128_f32[0];
	const float dz = m_currentPos.m128_f32[2] - m_prevPos.m128_f32[2];

	const float angle = atan2(dz, dx);

	return (angle < 0 ? DirectX::XM_2PI + angle : angle);
}

void CurvePathComponent::DefaultPointSet()
{
	m_forwardDiffTable.clear();
	m_forwardDiffTable.resize((size_t)s_defaultAmountOfPoints);
	m_pointInterval = 1.0f / s_defaultAmountOfEntries;
	m_totalLengthOfCurve = 0.f;

	for (int p = 0; p < s_defaultAmountOfPoints; ++p)
	{
		float p0_t, p1_t, curveSegmentLength = 0.f;
		m_forwardDiffTable[p].resize(s_defaultAmountOfEntries + 1);

		const XMVECTOR& P0 = m_controlPoints[m_currentP0_index];
		const XMVECTOR& P1 = m_controlPoints[m_currentP1_index];
		const XMVECTOR& P2 = m_controlPoints[m_currentP2_index];
		const XMVECTOR& P3 = m_controlPoints[m_currentP3_index];

		for (size_t i = 1; i < m_forwardDiffTable[p].size(); ++i) 
		{
			p0_t = m_forwardDiffTable[p][i - 1].parametricValue;
			p1_t = p0_t + m_pointInterval;

			const float x0 = GetSplinePointComponent(p0_t, 0, P0, P1, P2, P3);
			const float y0 = GetSplinePointComponent(p0_t, 1, P0, P1, P2, P3);
			const float z0 = GetSplinePointComponent(p0_t, 2, P0, P1, P2, P3);

			const float x1 = GetSplinePointComponent(p1_t, 0, P0, P1, P2, P3);
			const float y1 = GetSplinePointComponent(p1_t, 1, P0, P1, P2, P3);
			const float z1 = GetSplinePointComponent(p1_t, 2, P0, P1, P2, P3);

			XMVECTOR P0 = XMVectorSet(x0, y0, z0, 1.0f);
			XMVECTOR P1 = XMVectorSet(x1, y1, z1, 1.0f);

			curveSegmentLength += LengthBetween2Points(P0, P1);

			m_forwardDiffTable[p][i].parametricValue = p1_t;
		}
	}
}

void CurvePathComponent::PrepareDrawPoints()
{
	m_drawPoints.clear();
	for (size_t i = 0; i < m_controlPoints.size(); ++i)
	{
		m_tValue = 0;
		while (m_tValue <= 1.0f)
		{
			m_drawPoints.emplace_back(GetCurrentSplinePoint());

			m_tValue += m_pointInterval;
		}

		ShiftPointIndices();
	}

	//Return current t value to 0 for updating
	m_tValue = 0;
}

void CurvePathComponent::GenerateVertexBuffer(DX11Renderer* renderContext)
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

ObjectHandle CurvePathComponent::GetPathVBuffer() const
{
	return m_drawPointsVBuffer;
}

void CurvePathComponent::UpdatePath(const float dt)
{
	m_tValue += dt * m_pointInterval;
	if (m_tValue > 1.0f)
	{
		m_tValue = 0;
		ShiftPointIndices();
	}
}

int CurvePathComponent::GetPathVertexCount() const
{
	return m_drawPoints.size();
}

float CurvePathComponent::Clamp(const float value, const float minValue, const float maxValue) const
{
	return min( max(minValue, value), maxValue);
}

float CurvePathComponent::RandFloat(float minValue, float maxValue) const
{
	return minValue + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX /(maxValue - minValue)));
}

void CurvePathComponent::ShiftPointIndices()
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

int CurvePathComponent::GetPointIndex(const float v) const
{
	//Use (int) to floor the value down
	return (int)(v / m_pointInterval);
}

const float CurvePathComponent::LengthBetween2Points(const XMVECTOR& a, const XMVECTOR& b)
{
	return sqrt( LengthSquaredBetween2Points(a, b) );
}

const float CurvePathComponent::LengthSquaredBetween2Points(const XMVECTOR& a, const XMVECTOR& b)
{
	const float dx = a.m128_f32[0] - b.m128_f32[0];
	const float dy = a.m128_f32[1] - b.m128_f32[1];
	const float dz = a.m128_f32[2] - b.m128_f32[2];

	return (dx*dx) + (dy*dy) + (dz*dz);
}

const float CurvePathComponent::GetSplinePointComponent(const float t, const int i, const XMVECTOR& P0, const XMVECTOR& P1, const XMVECTOR& P2, const XMVECTOR& P3)
{
	const float result = 0.5f * (
		(2.0f * P1.m128_f32[i]) +
		((-P0.m128_f32[i] + P2.m128_f32[i])  * t) +
		(((2.0f * P0.m128_f32[i]) - (5.0f * P1.m128_f32[i]) + (4.0f * P2.m128_f32[i]) - P3.m128_f32[i]) * (t * t)) +
		((-P0.m128_f32[i] + (3.0f * P1.m128_f32[i]) - (3.0f * P2.m128_f32[i]) + P3.m128_f32[i]) * (t * t * t))
		);

	return result;
}

const float CurvePathComponent::s_defaultAmountOfEntries = 20;

const int CurvePathComponent::s_defaultAmountOfPoints = 10;
