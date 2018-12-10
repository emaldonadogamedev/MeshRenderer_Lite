#pragma once

#include <Systems/Core/Components/IComponent.h>

struct Light;
struct LightViewProj;
class ObjectHandle;

class ShadowLightComponent : public IComponent
{
public:
	ShadowLightComponent(const GameObject* owner, bool isActive = true, bool useShadows = true, bool useSoftShadows = false,
			const int shadowWidthHeight = 1024);
	virtual ~ShadowLightComponent();

	Light* GetLight() const;
	LightViewProj* GetLightViewProjBuffer() const;

	const XMVECTOR& GetUpVector()const;
	const XMVECTOR& GetRightVector()const;
	const XMVECTOR& GetLookAtVector()const;

	bool IsUsingShadows() const;
	void SetUseShadows(const bool v);

	bool IsUsingSoftShadows() const;
	void SetUseSoftShadows(const bool v);

	int GetShadowMapDimension() const;

	ObjectHandle& GetShadowDepthMapHandle();
	const ObjectHandle& GetShadowDepthMapHandle() const;

	ObjectHandle& GetSoftShadowDepthMapHandle();
	const ObjectHandle& GetSoftShadowDepthMapHandle() const;

	ObjectHandle& GetSoftShadowMapKernelWeightHandle();
	const ObjectHandle& GetSoftShadowMapKernelWeightHandle() const;

	int GetSoftShadowMapKernelHalfWidth() const;

	const int GetShadowTextureIdx() const;

	static const Light* const GetSceneLightsWithShadowPtr();
	static const LightViewProj* const GetShadowLightViewProjBuffersPtr();
	static int GetActiveLightsWithShadowCount();

protected:
	Light* m_light;
	LightViewProj* m_viewProj;

	XMVECTOR m_upVec, m_lookAtVec, m_rightVec;
	bool m_useShadows;
	bool m_useSoftShadows;
	int m_shadowTextureIdx;
	int m_shadowMapWidthHeight;

	ObjectHandle* m_shadowDepthMapHandle;
	ObjectHandle* m_softShadowDepthMapHandle;
	ObjectHandle* m_softShadowKernelWeightsHandle;
	int m_softShadowMapKernelHalfWidth;

	static Light sceneLightsWithShadows[];
	static ObjectHandle shadowMapHandles[];
	static ObjectHandle softShadowMapHandles[];
	static ObjectHandle softShadowKernelWeightHandles[];
	static LightViewProj shadowLightViewProjBuffers[];
	static int s_takenLightCount;

	friend class GraphicsSystem;
	friend class DX11Renderer;
};