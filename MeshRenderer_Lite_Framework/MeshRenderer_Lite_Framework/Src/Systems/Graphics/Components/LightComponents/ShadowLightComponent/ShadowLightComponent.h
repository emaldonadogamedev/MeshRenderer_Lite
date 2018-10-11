#pragma once

#include <Systems/Core/Components/IComponent.h>
#include <Systems/Graphics/GraphicsUtilities/ObjectHandle.h>

struct Light;
struct LightViewProj;

class ShadowLightComponent : public IComponent
{
public:
	ShadowLightComponent(const GameObject* owner, bool isActive = true, bool useShadows = true, 
			const int shadowWidthHeight = 1024);
	virtual ~ShadowLightComponent();

	Light* GetLight() const;
	LightViewProj* GetLightViewProjBuffer() const;

	bool IsUsingShadows() const;
	ObjectHandle& GetShadowDepthMapHandle();
	const ObjectHandle& GetShadowDepthMapHandle() const;
	const int GetShadowTextureIdx() const;

	void SetUseShadows(const bool v);

	static const Light* const GetSceneLightsWithShadowPtr();
	static const LightViewProj* const GetShadowLightViewProjBuffersPtr();
	static int GetActiveLightsWithShadowCount();

protected:
	Light* m_light;
	LightViewProj* m_viewProj;

	bool m_useShadows;
	ObjectHandle* m_shadowDepthMapHandle;
	int m_shadowTextureIdx;
	int m_shadowMapWidthHeight;

	static Light sceneLightsWithShadows[];
	static ObjectHandle shadowMapHandles[];
	static LightViewProj shadowLightViewProjBuffers[];
	static int s_takenLightCount;

	friend class GraphicsSystem;
	friend class DX11Renderer;
};