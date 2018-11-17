#include <Utilities/precompiled.h>

#include <Systems/Graphics/DX11RenderStages/SkyBoxRenderStage/SkyBoxRenderStage.h>

#include<Systems/Graphics/DX11Renderer/DX11Renderer.h>
#include<Systems/Graphics/DX11Renderer/DX11RendererData.h>

#include<Systems/Graphics/ModelClasses/Model/Model.h>

SkyBoxRenderStage::SkyBoxRenderStage(DX11Renderer* const renderer, RenderCompUmap* const gfxComponents, const Model* const boxGeometry)
	:IRenderStage(renderer, gfxComponents)
	, m_boxModel(boxGeometry)
	, m_skyboxTextureHandle()
{

}

SkyBoxRenderStage::~SkyBoxRenderStage()
{

}

void SkyBoxRenderStage::PreRender()
{
	m_renderData.m_pImmediateContext->RSSetState(m_renderData.m_d3dRasterStateSolCullBack); 
	m_renderData.m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_renderer->DisableColorBlending();
}

void SkyBoxRenderStage::Render(HandleDictionaryVec& graphicsResources, const float dt)
{
	if(!m_renderData.testCamera->GetIsUsingSkybox())
		return;

	ObjectHandle handle = (graphicsResources[(int)ObjectType::VERTEX_SHADER]).at("SkyBoxVS");
	m_renderer->BindVertexShader(handle);
	
	handle = (graphicsResources[(int)ObjectType::PIXEL_SHADER]).at("SkyBox2DPS");
	m_renderer->BindPixelShader(handle);

	m_renderer->BindVertexBuffer(m_boxModel->GetVBufferHandle(), sizeof(VertexAnimation));
	m_renderer->BindIndexBuffer(m_boxModel->GetIBufferHandle());


	if (const auto iblMap = m_renderer->GetTexture2D(s_textureDir + m_renderData.testCamera->GetSkyboxTexture() + ".hdr")) {
		m_renderData.m_pImmediateContext->PSSetShaderResources(25, 1, &m_renderData.textures2D[*iblMap].srv);
	}

	m_renderer->DrawIndexed(m_boxModel->GetIndicesCount());
}

void SkyBoxRenderStage::PostRender()
{

}
