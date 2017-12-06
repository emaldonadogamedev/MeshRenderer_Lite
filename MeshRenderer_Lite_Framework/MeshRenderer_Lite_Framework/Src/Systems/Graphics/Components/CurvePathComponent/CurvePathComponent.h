#pragma once

#include <Systems/Core/Components/IComponent.h>
#include <Systems/Graphics/GraphicsUtilities/ObjectHandle.h>
#include <Systems/Graphics/GraphicsUtilities/VertexTypes.h>
#include <DirectXMath.h>
#include <deque>

class DX11Renderer;

using DirectX::XMVECTOR;

struct TableEntry
{
	TableEntry(const float pV = 0.f, const float fcpv = 0.f, const float aL = 0.f)
		:segmentParametricValue(pV)
		,fullCurveParametricValue(fcpv)
		,arclength(aL)
	{}
	float segmentParametricValue;
	float fullCurveParametricValue;
	float arclength;
};
typedef std::vector<TableEntry> ForwardDiffTable;

typedef const XMVECTOR (*InterpolatingFunction)(const XMVECTOR& a, const XMVECTOR& b, const float factor);

class CurvePathComponent : public IComponent
{
public:
	CurvePathComponent(const GameObject* owner);
	virtual ~CurvePathComponent();

	//Initialization
	void GenerateVertexBuffer(DX11Renderer* renderContext);
	ObjectHandle GetPathVBuffer() const;
	
	void UpdatePath(const float dt);

	int GetPathVertexCount() const;
	const XMVECTOR GetCurrentSplinePoint();
	float GetCurrentAngle()const;

	bool m_usePath = false;
	float m_tableEntryInterval;// d or delta u  which is the unique continuous diff factor ( 1 / #amount of total intervals)
	float m_tableEntrySegmentInterval;// d or delta u  which is the unique continuous diff factor ( 1 / #amount of a single segment intervals)
	float m_walkSpeed = 300.0f;

	//time it takes to get from first control point to last control point
	float m_pathDuration = 3.0f;

	XMVECTOR m_pathCenterPos;
	std::vector<XMVECTOR> m_controlPoints;
	std::vector<XMVECTOR> m_drawPoints;

private:
	void DefaultPointSet();
	void PrepareDrawPoints();
	float Clamp(const float value, const float minValue = 0.0f, const float maxValue = 1.0f) const;
	float RandFloat(float minValue = 0, float maxValue = 1.0f) const;
	void ShiftRightPointIndices();
	void ShiftLeftPointIndices();
	void MovePointIndices(const float u);
	void ResetSplineSamplers();
	int GetSplineSegmentIndex(const float u) const;
	int GetSplineSegmentEntryIndex(const float u) const;

	ObjectHandle m_drawPointsVBuffer;
	std::vector<VertexAnimation> m_vertices;

	ForwardDiffTable m_forwardDiffTable;

	XMVECTOR m_currentPos;
	XMVECTOR m_nextPos;
	XMVECTOR m_currVelDir;
	float m_currentTime = 0;
	float m_currentPathDuration = 0.f;
	float m_currentDistTraveled = 0.f;
	float m_segmentDuration;
	float m_currentRate = 0.f;

	float m_easeInTime = 0.5f;
	float m_currEaseInTime = 0;

	float m_easeOutRate = 0.76f;

	int m_currentP0_index = 0;
	int m_currentP1_index = 1;
	int m_currentP2_index = 2;
	int m_currentP3_index = 3;
	int m_currentSegmentIndex = 0;
	int m_segmentCount = 0;

	float m_totalLengthOfCurve;

	static const float LengthBetween2Points(const XMVECTOR& a, const XMVECTOR& b);
	static const float LengthSquaredBetween2Points(const XMVECTOR& a, const XMVECTOR& b);
	static const float GetSplinePointComponent(const float t, const int i, const XMVECTOR& P0, const XMVECTOR& P1, const XMVECTOR& P2, const XMVECTOR& P3);

	static const float s_defaultAmountOfEntriesPerSegment;
	static const int s_defaultAmountOfControlPoints;

	friend class ImGuiStage;
};