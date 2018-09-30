#include <Utilities/precompiled.h>
#include <Systems/Graphics/Components/CurvePathComponent/CurvePathComponent.h>

#include <Systems/Graphics/DX11Renderer/DX11Renderer.h>

using DirectX::XMVectorSet;

CurvePathComponent::CurvePathComponent(const GameObject* owner)
	:IComponent(ComponentType::RENDERABLE_CURVE_PATH, owner)
	, m_pathCenterPos(XMVectorSet(0,0,0,1.0f))
	, m_currentPos(XMVectorSet(0,0,0,1.0f))
	, m_nextPos(XMVectorSet(0, 0, 0, 1.0f))
	, m_currVelDir(XMVectorSet(0, 0, 0, 1.0f))
	, m_easeOutRate(.78f)
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
	m_currentPos = m_nextPos;

	const int splineEntryIndex = GetSplineSegmentEntryIndex( Clamp(m_currentRate));
	float t = m_forwardDiffTable[splineEntryIndex].segmentParametricValue;

	MovePointIndices(Clamp(m_currentRate));

	const XMVECTOR& P0 = m_controlPoints[m_currentP0_index];
	const XMVECTOR& P1 = m_controlPoints[m_currentP1_index];
	const XMVECTOR& P2 = m_controlPoints[m_currentP2_index];
	const XMVECTOR& P3 = m_controlPoints[m_currentP3_index];

	float x = GetSplinePointComponent(t, 0, P0, P1, P2, P3);
	float y = GetSplinePointComponent(t, 1, P0, P1, P2, P3);
	float z = GetSplinePointComponent(t, 2, P0, P1, P2, P3);

	m_nextPos = DirectX::XMVectorAdd(m_pathCenterPos, XMVectorSet(x, y, z, 0));

	if (!DirectX::XMVector3Equal(m_nextPos, m_currentPos))
	{
		m_currVelDir = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(m_nextPos, m_currentPos));
	}

	return m_nextPos;
}

float CurvePathComponent::GetCurrentAngle()
{
	//const float dx = m_nextPos.m128_f32[0] - m_currentPos.m128_f32[0];
	//const float dz = m_nextPos.m128_f32[2] - m_currentPos.m128_f32[2];

	//const float atan2_r = atan2(dz, dx);
	//
	//m_currentAngle = atan2_r >= 0.f ? atan2_r : DirectX::XM_2PI + atan2_r;

	//return m_currentAngle;

	const int splineEntryIndex = GetSplineSegmentEntryIndex(Clamp(m_currentRate));
	float t = m_forwardDiffTable[splineEntryIndex].segmentParametricValue;
	
	const XMVECTOR& P0 = m_controlPoints[m_currentP0_index];
	const XMVECTOR& P1 = m_controlPoints[m_currentP1_index];
	const XMVECTOR& P2 = m_controlPoints[m_currentP2_index];
	const XMVECTOR& P3 = m_controlPoints[m_currentP3_index];
	
	float x = GetDerivedSplinePointComponent(t, 0, P0, P1, P2, P3);
	float y = GetDerivedSplinePointComponent(t, 1, P0, P1, P2, P3);
	float z = GetDerivedSplinePointComponent(t, 2, P0, P1, P2, P3);
	
	const auto derived = DirectX::XMVector3Normalize(XMVectorSet(x, y, z, 0));
	const float dot = (m_firstAngleVec.m128_f32[0] * derived.m128_f32[0])
		+ (m_firstAngleVec.m128_f32[1] * derived.m128_f32[1])
		+ (m_firstAngleVec.m128_f32[2] * derived.m128_f32[2]);

	//const float dot = (m_firstAngleVec.m128_f32[0] * m_currVelDir.m128_f32[0])
	//	+ (m_firstAngleVec.m128_f32[1] * m_currVelDir.m128_f32[1])
	//	+ (m_firstAngleVec.m128_f32[2] * m_currVelDir.m128_f32[2]);

	if (derived.m128_f32[0] == 0.0f)
		m_currentAngle = DirectX::XM_PI;
	else if(derived.m128_f32[2] == 0.0f)
		m_currentAngle = DirectX::XM_PI + DirectX::XM_PIDIV2;
	else if (m_currVelDir.m128_f32[0] >= 0.f && m_currVelDir.m128_f32[2] >= 0.f)
		m_currentAngle =  -acos(dot) + DirectX::XM_PI + DirectX::XM_PIDIV2;
	else if(m_currVelDir.m128_f32[0] >= 0.f && m_currVelDir.m128_f32[2] < 0.f)
		m_currentAngle = acos(dot) + DirectX::XM_PI + DirectX::XM_PIDIV2;


	//m_currentAngle = modifier * acos(dot) + DirectX::XM_PI;// -2.757f;// +DirectX::XM_PI + DirectX::XM_PIDIV2;// this->deleteThisAfterUsage;

	//if(m_currVelDir.m128_f32[2] < 0)
	//	m_currentAngle = acos(dot) + DirectX::XM_PI	;// this->deleteThisAfterUsage;
	//else
	//	m_currentAngle = -acos(dot) + DirectX::XM_PIDIV2;// this->deleteThisAfterUsage;

	return m_currentAngle;
}

void CurvePathComponent::DefaultPointSet()
{
	m_forwardDiffTable.clear();
	m_controlPoints.resize((size_t)s_defaultAmountOfControlPoints);

	//We subtract 3 from the amount of control points because we have the 2 extra points plus the fact that a segment lies between 2 points
	//For example:
	// P0    P1(0, ....., 1.0) P2    P3, that's the structure of the catmul-rom
	m_segmentCount = (size_t)(s_defaultAmountOfControlPoints - 3);
	m_tableEntrySegmentInterval = 1.f / s_defaultAmountOfEntriesPerSegment;

	m_forwardDiffTable.reserve((m_segmentCount * (int)s_defaultAmountOfEntriesPerSegment));
	m_tableEntryInterval = 1.0f / (float)(m_forwardDiffTable.capacity());
	m_totalLengthOfCurve = 0.f;

	//spread out the control points
	const float increment_X = 400.f / s_defaultAmountOfControlPoints;
	float x = -200.0f;
	for (int p = 0; p < s_defaultAmountOfControlPoints; ++p)
	{
		m_controlPoints[p] = XMVectorSet(x, 0, RandFloat(-100.f, 100.f), 1.0f);
		x += increment_X;
	}

	float tempFullCurvParmValue = 0.f;
	for ( int i = 0; i < m_segmentCount; ++i )
	{
		//Prepare the forward diff table with default values
		float p0_t = 0.f, curveSegmentLength = 0.f;

		const XMVECTOR& P0 = m_controlPoints[m_currentP0_index];
		const XMVECTOR& P1 = m_controlPoints[m_currentP1_index];
		const XMVECTOR& P2 = m_controlPoints[m_currentP2_index];
		const XMVECTOR& P3 = m_controlPoints[m_currentP3_index];

		const float x0 = GetSplinePointComponent(p0_t, 0, P0, P1, P2, P3);
		const float y0 = GetSplinePointComponent(p0_t, 1, P0, P1, P2, P3);
		const float z0 = GetSplinePointComponent(p0_t, 2, P0, P1, P2, P3);

		XMVECTOR current_P0 = XMVectorSet(x0, y0, z0, 1.0f);
		XMVECTOR current_P1;
		
		do
		{
			//Update main table parametric value
			m_forwardDiffTable.emplace_back(TableEntry(p0_t, tempFullCurvParmValue, m_totalLengthOfCurve));

			//update the full curve parametric value
			tempFullCurvParmValue += m_tableEntryInterval;

			//update temp ratio
			p0_t += m_tableEntrySegmentInterval;

			//calculate the new point
			const float x1 = GetSplinePointComponent(p0_t, 0, P0, P1, P2, P3);
			const float y1 = GetSplinePointComponent(p0_t, 1, P0, P1, P2, P3);
			const float z1 = GetSplinePointComponent(p0_t, 2, P0, P1, P2, P3);
			current_P1 = XMVectorSet(x1, y1, z1, 1.0f);

			m_totalLengthOfCurve += LengthBetween2Points(current_P0, current_P1);

			//update the P0
			current_P0 = current_P1;

		} while (p0_t < 1.0f);

		m_forwardDiffTable.emplace_back(TableEntry(p0_t, tempFullCurvParmValue, m_totalLengthOfCurve));

		ShiftRightPointIndices();
	}//end big for loop

	//normalize all of the arc lengths for the current segment
	for (size_t i = 0; i < m_forwardDiffTable.size(); ++i)
	{
		m_forwardDiffTable[i].arclength /= m_totalLengthOfCurve;
	}

	ResetSplineSamplers();

	//Prepare first angle vec
	const XMVECTOR& P0 = m_controlPoints[m_currentP0_index];
	const XMVECTOR& P1 = m_controlPoints[m_currentP1_index];
	const XMVECTOR& P2 = m_controlPoints[m_currentP2_index];
	const XMVECTOR& P3 = m_controlPoints[m_currentP3_index];

	float x1 = GetSplinePointComponent(0.f, 0, P0, P1, P2, P3);
	float y1 = GetSplinePointComponent(0.f, 1, P0, P1, P2, P3);
	float z1 = GetSplinePointComponent(0.f, 2, P0, P1, P2, P3);
	const XMVECTOR current_P0 = XMVectorSet(x1, y1, z1, 1.0f);

	x1 = GetSplinePointComponent(m_tableEntrySegmentInterval, 0, P0, P1, P2, P3);
	y1 = GetSplinePointComponent(m_tableEntrySegmentInterval, 1, P0, P1, P2, P3);
	z1 = GetSplinePointComponent(m_tableEntrySegmentInterval, 2, P0, P1, P2, P3);
	const XMVECTOR current_P1 = XMVectorSet(x1, y1, z1, 1.0f); 
	m_firstAngleVec = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(current_P1, current_P0));
}

void CurvePathComponent::PrepareDrawPoints()
{
	m_drawPoints.clear();

	m_drawPoints.reserve(m_forwardDiffTable.size() );
	for (int i = 0; i < m_segmentCount; ++i)
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

			tValue += m_tableEntrySegmentInterval;
		}

		ShiftRightPointIndices();
	}

	ResetSplineSamplers();
}

void CurvePathComponent::GenerateVertexBuffer(DX11Renderer* renderContext)
{
	m_vertices.clear();
	if (m_drawPointsVBuffer != ObjectHandle::Null())
	{
		renderContext->ReleaseObject(m_drawPointsVBuffer);
	}

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
	//reset rate if needed
	if ( fabs(m_currentDistTraveled - m_totalLengthOfCurve) <= 0.005f)
	{
		m_currentDistTraveled = m_currentRate = 0.f;
		m_currEaseInTime = 0.f;
	}

	//check if we're at ease-in stage
	if (m_easeInTime > 0.0f && m_currEaseInTime <= m_easeInTime)
	{
		m_currentDistTraveled += (m_currEaseInTime / m_easeInTime) * m_walkSpeed * dt;
		m_currEaseInTime += dt;
	}

	//check if we're at ease-out stage
	else if (m_currentRate >= m_easeOutRate)
	{
		m_currentDistTraveled += (1.0f - ((m_currentRate - m_easeOutRate) / (1.0f - m_easeOutRate))) * m_walkSpeed * dt;
	}

	else
	{
		m_currentDistTraveled += m_walkSpeed * dt;
	}

	m_currentTime += dt;

	m_currentRate = m_currentDistTraveled / m_totalLengthOfCurve;
}

int CurvePathComponent::GetPathVertexCount() const
{
	return m_drawPoints.size();
}

void CurvePathComponent::ShiftRightPointIndices()
{
	++m_currentP3_index;
	if (m_currentP3_index >= m_controlPoints.size())
	{
		m_currentSegmentIndex = 0;

		m_currentP0_index = 0;
		m_currentP1_index = 1;
		m_currentP2_index = 2;
		m_currentP3_index = 3;

		return;
	}

	m_currentP2_index = m_currentP3_index - 1;
	m_currentP1_index = m_currentP3_index - 2;
	m_currentP0_index = m_currentP3_index - 3;
	++m_currentSegmentIndex;
}

void CurvePathComponent::ShiftLeftPointIndices()
{
	--m_currentP0_index;
	if (m_currentP0_index < 0)
	{
		m_currentSegmentIndex = m_segmentCount - 1;

		m_currentP0_index = m_segmentCount - 1;
		m_currentP1_index = m_segmentCount;
		m_currentP2_index = m_segmentCount + 1;
		m_currentP3_index = m_segmentCount + 2;

		return;
	}

	m_currentP1_index = m_currentP0_index + 1;
	m_currentP2_index = m_currentP0_index + 2;
	m_currentP3_index = m_currentP0_index + 3;
	--m_currentSegmentIndex;
}

void CurvePathComponent::MovePointIndices(const float u)
{
	m_currentSegmentIndex = GetSplineSegmentIndex(u);

	m_currentP0_index = m_currentSegmentIndex;
	m_currentP1_index = m_currentSegmentIndex + 1;
	m_currentP2_index = m_currentSegmentIndex + 2;
	m_currentP3_index = m_currentSegmentIndex + 3;
}

void CurvePathComponent::ResetSplineSamplers()
{
	m_currentP0_index = m_currentSegmentIndex = 0;
	m_currentP1_index = 1;
	m_currentP2_index = 2;
	m_currentP3_index = 3;
}

int CurvePathComponent::GetSplineSegmentIndex(const float u) const
{
	return  (int)( GetSplineSegmentEntryIndex(u) / (s_defaultAmountOfEntriesPerSegment + 1));
}

int CurvePathComponent::GetSplineSegmentEntryIndex(const float u) const
{
	return (int)(u * (float)(m_forwardDiffTable.size() - 1));
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

const float CurvePathComponent::GetDerivedSplinePointComponent(const float t, const int i, const XMVECTOR& P0, const XMVECTOR& P1, const XMVECTOR& P2, const XMVECTOR& P3)
{
	const float result = 0.5f * (
		(-P0.m128_f32[i] + P2.m128_f32[i]) +
		(((2.0f * P0.m128_f32[i]) - (5.0f * P1.m128_f32[i]) + (4.0f * P2.m128_f32[i]) - P3.m128_f32[i]) * (2.0f * t)) +
		((-P0.m128_f32[i] + (3.0f * P1.m128_f32[i]) - (3.0f * P2.m128_f32[i]) + P3.m128_f32[i]) * (3.0f * t * t))
	);

	return result;
}

//Note: total curve will have #ofSegments * #ofEntries (example: 7 * 25 = 175 entries total)
const float CurvePathComponent::s_defaultAmountOfEntriesPerSegment = 45;

//Minimum is 10 since we're using catmull-romm spline, which requires 1 extra point on each side of the curve
const int CurvePathComponent::s_defaultAmountOfControlPoints = 10;// +(rand() % 5);
