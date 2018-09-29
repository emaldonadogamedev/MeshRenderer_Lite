#pragma once

#include <Systems/Graphics/Components/LightComponents/LightComponent/LightComponent.h>
#include <Systems/Graphics/GraphicsUtilities/ObjectHandle.h>

class Light;

class ShadowLightComponent : public IComponent
{
public:
	ShadowLightComponent(const GameObject* owner, bool isActive = true, bool useShadows = true, 
			const int shadowWidthHeight = 512);
	virtual ~ShadowLightComponent();

	Light* GetLight() const;

	bool IsUsingShadows() const;
	ObjectHandle& GetShadowRThandle();
	const ObjectHandle& GetShadowRThandle() const;
	const int GetShadowTextureIdx() const;

	void SetUseShadows(const bool v);

	static const unsigned int s_maxLights;
	static const Light* const GetSceneLightsWithShadowPtr();
	static int GetActiveLightsWithShadowCount();

protected:
		Light* m_light;

	bool m_useShadows;
	ObjectHandle* m_shadowRThandle;
	int m_shadowTextureIdx;
	int m_shadowMapWidthHeight;

	static Light sceneLightsWithShadows[];
	static ObjectHandle shadowMapHandles[];
	static int s_takenLightCount;

	friend class GraphicsSystem;
	friend class DX11Renderer;
};