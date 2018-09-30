#pragma once

#include <Systems/Core/Components/IComponent.h>
#include <Systems/Graphics/GraphicsUtilities/ObjectHandle.h>
#include <string>
#include <vector>
#include <unordered_map>


//typedefs
typedef std::unordered_map<std::string, ObjectHandle> HandleDictionary;
typedef std::vector<HandleDictionary>  HandleDictionaryVec;
typedef std::vector<std::vector<IComponent*>> RenderCompVec;
typedef std::unordered_map<ComponentType, std::vector<IComponent*>> RenderCompUmap;

class DX11Renderer;
class DX11RendererData;

class IRenderStage
{
public:
	IRenderStage(DX11Renderer* const rendererData, RenderCompUmap* const gfxComponents);
	virtual ~IRenderStage();

	virtual void PreRender() = 0;
	virtual void Render(HandleDictionaryVec& graphicsResources, const float dt) = 0;
	virtual void PostRender() = 0;

	bool GetIsActive() const;
	void SetIsActive(const bool isActive);

protected:
	IComponent* GetComponentHelper(const ComponentType compType, const int index);

	bool m_isActive = true;

	DX11RendererData& m_renderData;
	DX11Renderer* const m_renderer;
	RenderCompUmap* const m_gfxSystemComponents;
};