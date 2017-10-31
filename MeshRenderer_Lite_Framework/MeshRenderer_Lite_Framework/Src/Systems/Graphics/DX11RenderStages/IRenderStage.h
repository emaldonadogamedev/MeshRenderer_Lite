#pragma once

#include <Systems/Graphics/GraphicsUtilities/ObjectHandle.h>
#include <string>
#include <vector>
#include <array>
#include <unordered_map>


//typedefs
typedef std::unordered_map<std::string, ObjectHandle> HandleDictionary;
typedef std::array<HandleDictionary, (size_t)ObjectType::COUNT>  HandleDictionaryVec;

class DX11Renderer;
class IRenderComponent;

class IRenderStage
{
public:
	IRenderStage(DX11Renderer* const rendererData, std::vector<IRenderComponent*> * const gfxComponents)
		:m_renderer(rendererData)
		,m_gfxSystemComponents(gfxComponents) {}
	virtual ~IRenderStage() {};

	virtual void PreRender() = 0;
	virtual void Render(const HandleDictionaryVec& graphicsResources) = 0;
	virtual void PostRender() = 0;

protected:
	DX11Renderer* const m_renderer;
	std::vector<IRenderComponent*>* const m_gfxSystemComponents;
};