#pragma once

#include <Systems/Core/Components/IComponent.h>
#include <Systems/Graphics/GraphicsUtilities/ObjectHandle.h>
#include <Systems/Graphics/GraphicsUtilities/VertexTypes.h>
#include <DirectXMath.h>
#include <deque>

class DX11Renderer;

using DirectX::XMVECTOR;

enum class SegmentStatus
{
	EASE_IN,
	NORMAL,
	EASE_OUT
};

struct CurveSegment
{
	CurveSegment(const float pV = 0.f, const float aL = 0.f)
		:parametricValue(pV)
		,arclength(aL)
	{}
	float parametricValue;
	float arclength;
};
typedef std::vector<CurveSegment> SegmentList;
typedef std::vector<SegmentList> ForwardDiffTable;

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
	float m_pointInterval;// d or delta u  which is the unique continuous diff factor ( 1 / #amount of table intervals)
	float m_walkUpdateFreq = 2.0f;
	float m_totalLengthOfCurve;

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
	void ShiftPointIndices();
	void ResetSplineSamplers();
	int GetSplineIndex(const float u)const;

	int GetPointIndex(const float v) const;

	ObjectHandle m_drawPointsVBuffer;
	std::vector<VertexAnimation> m_vertices;

	ForwardDiffTable m_forwardDiffTable;

	XMVECTOR m_currentPos;
	XMVECTOR m_prevPos;
	float m_currentTime = 0.0f;
	float m_segmentDuration;
	SegmentStatus m_segmentStatus = SegmentStatus::EASE_IN;

	size_t m_currentP0_index = 0;
	size_t m_currentP1_index = 1;
	size_t m_currentP2_index = 2;
	size_t m_currentP3_index = 3;
	size_t m_currentSegmentIndex = 0;
	size_t m_segmentCount = 0;

	static const float LengthBetween2Points(const XMVECTOR& a, const XMVECTOR& b);
	static const float LengthSquaredBetween2Points(const XMVECTOR& a, const XMVECTOR& b);
	static const float GetSplinePointComponent(const float t, const int i, const XMVECTOR& P0, const XMVECTOR& P1, const XMVECTOR& P2, const XMVECTOR& P3);

	static const float s_defaultAmountOfEntries;
	static const int s_defaultAmountOfPoints;

	friend class ImGuiStage;
};