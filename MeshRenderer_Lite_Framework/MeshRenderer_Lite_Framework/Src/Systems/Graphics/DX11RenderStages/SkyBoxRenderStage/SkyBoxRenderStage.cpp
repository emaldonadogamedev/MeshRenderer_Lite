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
	m_renderData.m_pImmediateContext->RSSetState(m_renderData.m_d3dRasterStateSolCullNone);
	m_renderData.m_pImmediateContext->OMSetDepthStencilState(m_renderData.m_DSLessEqual, 0);
	//m_renderData.m_pImmediateContext->RSSetState(m_renderData.m_d3dRasterStateSkybox);
	m_renderData.m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
	m_renderer->DisableColorBlending();
	m_renderData.m_pImmediateContext->RSSetViewports(1, &m_renderData.m_mainViewport);
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


	auto& textures2D = graphicsResources.at((int)ObjectType::TEXTURE_2D);
	const auto iblMapDir = s_textureDir + m_renderData.testCamera->GetSkyboxTexture() + ".hdr";

	handle.MakeNull();

	const auto it = textures2D.find(iblMapDir);
	if (it != textures2D.end())
	{
		handle = it->second;
	}

	else {
		handle = m_renderer->GetTexture2D(iblMapDir);
		textures2D[iblMapDir] = handle;
	}

	m_renderData.m_pImmediateContext->PSSetShaderResources(25, 1, &m_renderData.textures2D[*handle].srv);
	m_renderer->DrawIndexed(m_boxModel->GetIndicesCount());
}

void SkyBoxRenderStage::PostRender()
{
	m_renderData.m_pImmediateContext->OMSetDepthStencilState(nullptr, 0);
}
