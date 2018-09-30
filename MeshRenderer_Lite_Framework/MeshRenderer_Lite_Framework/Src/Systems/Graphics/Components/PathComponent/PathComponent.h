#pragma once

#include <Systems/Core/Components/IComponent.h>
#include <Systems/Graphics/GraphicsUtilities/ObjectHandle.h>
#include <Systems/Graphics/GraphicsUtilities/VertexTypes.h>
#include <DirectXMath.h>
#include <deque>

class DX11Renderer;

using DirectX::XMVECTOR;

class PathComponent : public IComponent
{
public:
	PathComponent(const GameObject* owner);
	virtual ~PathComponent();

	//Initialization
	void GenerateVertexBuffer(DX11Renderer* renderContext);
	ObjectHandle GetPathVBuffer() const;
	
	void UpdatePath(const float dt);

	int GetPathVertexCount() const;
	const XMVECTOR GetCurrentSplinePoint();
	float GetCurrentAngle()const;

	bool m_usePath = false;
	float m_tValueIncrease = 0.20f;
	float m_walkUpdateFreq = 2.0f;
	XMVECTOR m_pathCenterPos;
	std::vector<XMVECTOR> m_controlPoints;
	std::vector<XMVECTOR> m_drawPoints;

private:
	void DefaultPointSet();
	void PrepareDrawPoints();
	void ShiftPointIndices();

	ObjectHandle m_drawPointsVBuffer;
	std::vector<VertexAnimation> m_vertices;

	XMVECTOR m_currentPos;
	XMVECTOR m_prevPos;
	float m_tValue = 0.0f;
	size_t m_currentP0_index = 0;
	size_t m_currentP1_index = 1;
	size_t m_currentP2_index = 2;
	size_t m_currentP3_index = 3;

	static const float GetSplinePointComponent(const float t, const int i, const XMVECTOR& P0, const XMVECTOR& P1, const XMVECTOR& P2, const XMVECTOR& P3);

	static const int s_defaultAmountOfPoints;
};