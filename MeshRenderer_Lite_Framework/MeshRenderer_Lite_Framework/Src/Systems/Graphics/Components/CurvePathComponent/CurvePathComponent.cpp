#include <Utilities/precompiled.h>
#include <Systems/Graphics/Components/CurvePathComponent/CurvePathComponent.h>

#include <Systems/Graphics/DX11Renderer/DX11Renderer.h>

using DirectX::XMVectorSet;

CurvePathComponent::CurvePathComponent(const GameObject* owner)
	:IComponent(ComponentType::RENDERABLE_CURVE_PATH, owner)
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

	float timeRatio = 0;
	if (m_segmentStatus == SegmentStatus::EASE_IN)
	{
		timeRatio = m_currentTime / m_segmentDuration;
	}

	else if (m_segmentStatus == SegmentStatus::EASE_OUT)
	{
		float t2 = (m_segmentDuration * (float)(m_segmentCount - 1));
		float t = t2 + m_currentTime;
		timeRatio = 1.0 - ((t - t2) / (m_pathDuration - t2));
	}

	else
	{
		timeRatio = m_currentTime / m_segmentDuration;
	}

	const int splineIndex = GetSplineIndex(timeRatio);
	const float t = m_forwardDiffTable[m_currentSegmentIndex][splineIndex].parametricValue;

	const float x = GetSplinePointComponent(t, 0, P0, P1, P2, P3);
	const float y = GetSplinePointComponent(t, 1, P0, P1, P2, P3);
	const float z = GetSplinePointComponent(t, 2, P0, P1, P2, P3);

	return m_currentPos = DirectX::XMVectorAdd(m_pathCenterPos, XMVectorSet(x,y,z,0));
}

float CurvePathComponent::GetCurrentAngle() const
{
	const float dx = m_currentPos.m128_f32[0] - m_prevPos.m128_f32[0];
	const float dz = m_currentPos.m128_f32[2] - m_prevPos.m128_f32[2];

	const XMVECTOR vec = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(m_currentPos, m_prevPos));
	return acos(vec.m128_f32[0]);


	const float angle = atan2(dz, dx);

	return angle < 0 ? DirectX::XM_2PI + angle : angle;
}

void CurvePathComponent::DefaultPointSet()
{
	m_forwardDiffTable.clear();
	m_controlPoints.resize((size_t)s_defaultAmountOfPoints);
	m_forwardDiffTable.resize(m_segmentCount = (size_t)(s_defaultAmountOfPoints - 2));
	m_segmentDuration = m_pathDuration / (float)m_segmentCount;
	m_pointInterval = 1.0f / s_defaultAmountOfEntries;
	m_totalLengthOfCurve = 0.f;

	//prepare the control points
	const float increment_X = 400.f / s_defaultAmountOfPoints;
	float x = -200.0f;
	for (int p = 0; p < s_defaultAmountOfPoints; ++p)
	{
		m_controlPoints[p] = XMVectorSet(x, 0, RandFloat(-100.f, 100.f), 1.0f);
		x += increment_X;
	}

	for (int p = 0; p < m_forwardDiffTable.size(); ++p)
	{
		float p0_t = 0.f, p1_t = 0.f, curveSegmentLength = 0.f;
		m_forwardDiffTable[p].resize(s_defaultAmountOfEntries + 1);

		const XMVECTOR& P0 = m_controlPoints[m_currentP0_index];
		const XMVECTOR& P1 = m_controlPoints[m_currentP1_index];
		const XMVECTOR& P2 = m_controlPoints[m_currentP2_index];
		const XMVECTOR& P3 = m_controlPoints[m_currentP3_index];

		const float x0 = GetSplinePointComponent(p0_t, 0, P0, P1, P2, P3);
		const float y0 = GetSplinePointComponent(p0_t, 1, P0, P1, P2, P3);
		const float z0 = GetSplinePointComponent(p0_t, 2, P0, P1, P2, P3);

		XMVECTOR current_P0 = XMVectorSet(x0, y0, z0, 1.0f);
		XMVECTOR current_P1;

		for (size_t i = 1; i < m_forwardDiffTable[p].size(); ++i)
		{
			p1_t = p0_t + m_pointInterval;

			//calculate the new point
			const float x1 = GetSplinePointComponent(p1_t, 0, P0, P1, P2, P3);
			const float y1 = GetSplinePointComponent(p1_t, 1, P0, P1, P2, P3);
			const float z1 = GetSplinePointComponent(p1_t, 2, P0, P1, P2, P3);
			current_P1 = XMVectorSet(x1, y1, z1, 1.0f);

			curveSegmentLength += LengthBetween2Points(current_P0, current_P1);
			m_forwardDiffTable[p][i].arclength = curveSegmentLength;

			//update the P0
			current_P0 = current_P1;

			//Update the t0 value
			m_forwardDiffTable[p][i].parametricValue = p1_t;
			p0_t = p1_t;
		}

		//normalize all of the arc lengths for the current segment
		for (size_t i = 1; i < m_forwardDiffTable[p].size(); ++i)
		{
			m_forwardDiffTable[p][i].arclength /= curveSegmentLength;
		}

		ShiftPointIndices();
	}

	ResetSplineSamplers();
}

void CurvePathComponent::PrepareDrawPoints()
{
	m_drawPoints.clear();
	for (size_t i = 0; i < m_forwardDiffTable.size(); ++i)
	{
		float tValue = 0;
		const XMVECTOR& P0 = m_controlPoints[m_currentP0_index];
		const XMVECTOR& P1 = m_controlPoints[m_currentP1_index];
		const XMVECTOR& P2 = m_controlPoints[m_currentP2_index];
		const XMVECTOR& P3 = m_controlPoints[m_currentP3_index];

		while (tValue <= 1.0f)
		{

			const float x = GetSplinePointComponent(tValue, 0, P0, P1, P2, P3);
			const float y = GetSplinePointComponent(tValue, 1, P0, P1, P2, P3);
			const float z = GetSplinePointComponent(tValue, 2, P0, P1, P2, P3);
			m_drawPoints.emplace_back(XMVectorSet(x, y, z, 1.0f));

			tValue += m_pointInterval;
		}

		ShiftPointIndices();
	}

	ResetSplineSamplers();

	//Return current t value to 0 for updating
	m_currentTime = 0;
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
	m_currentTime += dt;
	if (m_currentTime > m_segmentDuration)
	{
		m_currentTime = 0;
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
	++m_currentP3_index;
	if (m_currentP3_index >= m_controlPoints.size())
	{
		m_currentP0_index = m_currentSegmentIndex = 0;
		m_currentP1_index = 1;
		m_currentP2_index = 2;
		m_currentP3_index = 3;
		m_segmentStatus = SegmentStatus::EASE_IN;

		return;
	}

	m_currentP2_index = m_currentP3_index - 1;
	m_currentP1_index = m_currentP3_index - 2;
	m_currentP0_index = m_currentP3_index - 3;
	m_segmentStatus = m_currentSegmentIndex == m_segmentCount - 1 ? SegmentStatus::EASE_OUT : SegmentStatus::NORMAL;

	++m_currentSegmentIndex;
}

void CurvePathComponent::ResetSplineSamplers()
{
	m_currentP0_index = m_currentSegmentIndex = 0;
	m_currentP1_index = 1;
	m_currentP2_index = 2;
	m_currentP3_index = 3;
}

int CurvePathComponent::GetSplineIndex(const float u) const
{
	return (int)(u / m_pointInterval);
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

const int CurvePathComponent::s_defaultAmountOfPoints = 10 + (rand() % 5) ;
