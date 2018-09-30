#include <Utilities/precompiled.h>
#include <Systems/Graphics/DX11RenderStages/IRenderStage.h>

#include<Systems/Graphics/DX11Renderer/DX11Renderer.h>

IRenderStage::IRenderStage(DX11Renderer* const rendererData, RenderCompUmap* const gfxComponents)
		:m_renderer(rendererData)
		, m_gfxSystemComponents(gfxComponents)
		, m_renderData(rendererData->GetRendererData())
{
}

IRenderStage::~IRenderStage()
{

}

bool IRenderStage::GetIsActive() const
{
		return m_isActive;
}

void IRenderStage::SetIsActive(const bool isActive)
{
		m_isActive = isActive;
}

IComponent* IRenderStage::GetComponentHelper(const ComponentType compType, const int index)
{
		const auto& compVec = (*m_gfxSystemComponents)[compType];
		const int size = compVec.size();
		if (size > 0 && index >= 0 && index < size)
		{
				return compVec[index];
		}

		return nullptr;
}
