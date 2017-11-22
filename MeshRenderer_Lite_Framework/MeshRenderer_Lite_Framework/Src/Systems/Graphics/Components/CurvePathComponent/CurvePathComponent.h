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
	TableEntry()
		:parametricValue(0.f)
		, arclength(0.f)
	{}
	TableEntry(const float pV = 0.f, const float aL = 0.f)
		:parametricValue(pV)
		,arclength(aL)
	{}
	float parametricValue;
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
	float m_pointInterval;// d or delta u  which is the unique continuous diff factor ( 1 / #amount of points)
	float m_walkUpdateFreq = 2.0f;
	float m_totalLengthOfCurve;
	XMVECTOR m_pathCenterPos;
	std::vector<XMVECTOR> m_controlPoints;
	std::vector<XMVECTOR> m_drawPoints;

private:
	void DefaultPointSet();
	void PrepareDrawPoints();
	float Clamp(const float value, const float minValue = 0.0f, const float maxValue = 1.0f) const;
	float RandFloat(float minValue = 0, float maxValue = 1.0f) const;
	void ShiftPointIndices();

	int GetPointIndex(const float v) const;

	ObjectHandle m_drawPointsVBuffer;
	std::vector<VertexAnimation> m_vertices;

	ForwardDiffTable m_forwardDiffTable;

	XMVECTOR m_currentPos;
	XMVECTOR m_prevPos;
	float m_tValue = 0.0f;
	size_t m_currentP0_index = 0;
	size_t m_currentP1_index = 1;
	size_t m_currentP2_index = 2;
	size_t m_currentP3_index = 3;

	static const float LengthBetween2Points(const XMVECTOR& a, const XMVECTOR& b);
	static const float LengthSquaredBetween2Points(const XMVECTOR& a, const XMVECTOR& b);
	static const float GetSplinePointComponent(const float t, const int i, const XMVECTOR& P0, const XMVECTOR& P1, const XMVECTOR& P2, const XMVECTOR& P3);

	static const float s_defaultAmountOfPoints;
};