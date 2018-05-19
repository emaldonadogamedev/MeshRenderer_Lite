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

class DX11Renderer;

class IRenderStage
{
public:
	IRenderStage(DX11Renderer* const rendererData, RenderCompVec* const gfxComponents)
		:m_renderer(rendererData)
		,m_gfxSystemComponents(gfxComponents) {}
	virtual ~IRenderStage() {};

	virtual void PreRender() = 0;
	virtual void Render(HandleDictionaryVec& graphicsResources, const float dt) = 0;
	virtual void PostRender() = 0;

protected:
	IComponent* GetComponentHelper(const ComponentType compType, const int index)
	{
		const auto& compVec = (*m_gfxSystemComponents)[(int)compType];
		const int size = compVec.size();
		if (size > 0 && index >= 0 && index < size)
		{
			return compVec[index];
		}

		return nullptr;
	}

	DX11Renderer* const m_renderer;
	RenderCompVec* const m_gfxSystemComponents;
};