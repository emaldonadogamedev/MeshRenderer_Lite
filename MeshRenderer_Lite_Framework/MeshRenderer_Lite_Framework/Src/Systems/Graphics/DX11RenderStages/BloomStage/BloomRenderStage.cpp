#include<Utilities/precompiled.h>
#include<Systems/Graphics/DX11RenderStages/BloomStage/BloomRenderStage.h>

#include<Systems/Graphics/DX11Renderer/DX11Renderer.h>
#include<Systems/Graphics/DX11Renderer/DX11RendererData.h>
#include <Systems/Graphics/GraphicsUtilities/VertexTypes.h>

BloomRenderStage::BloomRenderStage(DX11Renderer* const renderer, RenderCompUmap* const gfxComponents)
	:IRenderStage(renderer, gfxComponents)
{
	m_bloomTextureHandle = m_renderData.m_BloomBrightMap;
}

BloomRenderStage::~BloomRenderStage()
{
}

void BloomRenderStage::PreRender()
{

}

void BloomRenderStage::Render(HandleDictionaryVec& graphicsResources, const float dt)
{
	//In this stage we blur the Bloom texture and combine it with the current main RT
	if(!m_renderData.m_isUsingBloom)
		return;

	//////////////////////////////////////////////////////////////////////////
	// Cleanup before horizontal blur
	static ID3D11ShaderResourceView* const nullShadowSrvs[2] = { nullptr, nullptr };
	static ID3D11UnorderedAccessView* const nullUavArr[1] = { nullptr };

	m_renderData.m_pImmediateContext->CSSetShader(nullptr, nullptr, 0);
	m_renderData.m_pImmediateContext->CSSetShaderResources(0, 2, nullShadowSrvs);
	m_renderData.m_pImmediateContext->CSSetUnorderedAccessViews(0, 1, nullUavArr, nullptr);

	//remove the current render target
	m_renderer->BindNullRenderTarget();

	//////////////////////////////////////////////////////////////////////////
	//Horizontal Blur
	 ObjectHandle handle = (graphicsResources[(int)ObjectType::COMPUTE_SHADER]).at("SimpleBloomBlur_Horizontal");
	 m_renderer->BindComputeShader(handle);
	 m_renderer->BindTextureShaderResource(ObjectType::COMPUTE_SHADER, 0, 1, m_bloomTextureHandle);
	 m_renderData.m_pImmediateContext->CSSetUnorderedAccessViews(0, 1, &m_renderData.renderTargets[ *m_renderData.m_MainRenderTargets[!m_renderData.m_currentMainRTindex] ].uav, nullptr);
	 m_renderer->DispatchComputeShader(handle, m_renderer->GetRenderTargetWidth() / 5, 
		m_renderer->GetRenderTargetHeight(), 1);

	//////////////////////////////////////////////////////////////////////////
	// Cleanup before vertical blur
	m_renderData.m_pImmediateContext->CSSetShader(nullptr, nullptr, 0);
	m_renderData.m_pImmediateContext->CSSetShaderResources(0, 2, nullShadowSrvs);
	m_renderData.m_pImmediateContext->CSSetUnorderedAccessViews(0, 1, nullUavArr, nullptr);

	//////////////////////////////////////////////////////////////////////////
	//Vertical Blur
	handle = (graphicsResources[(int)ObjectType::COMPUTE_SHADER]).at("SimpleBloomBlur_Vertical");
	m_renderer->BindComputeShader(handle);

	//Here we swap and the soft shadow map and the originally hard shadow map
	//The soft shadow map is blurred, but only horizontally
	m_renderer->BindTextureShaderResource(ObjectType::COMPUTE_SHADER, 0, 1, m_renderData.m_MainRenderTargets[!m_renderData.m_currentMainRTindex]);
	m_renderData.m_pImmediateContext->CSSetUnorderedAccessViews(0, 1, &m_renderData.renderTargets[*m_bloomTextureHandle].uav, nullptr);
	m_renderer->DispatchComputeShader(handle, m_renderer->GetRenderTargetWidth(), 
		m_renderer->GetRenderTargetHeight() / 5, 1);

	m_renderData.m_pImmediateContext->CSSetShader(nullptr, nullptr, 0);
	m_renderData.m_pImmediateContext->CSSetShaderResources(0, 2, nullShadowSrvs);
	m_renderData.m_pImmediateContext->CSSetUnorderedAccessViews(0, 1, nullUavArr, nullptr);

	
	//////////////////////////////////////////////////////////////////////////
	//Copy the main RT contents to the alternate RT
	handle = (graphicsResources[(int)ObjectType::COMPUTE_SHADER]).at("Texture2DCopy");
	m_renderer->BindComputeShader(handle);
	m_renderer->BindTextureShaderResource(ObjectType::COMPUTE_SHADER, 0, 1, m_renderData.m_MainRenderTargets[m_renderData.m_currentMainRTindex]);
	m_renderData.m_pImmediateContext->CSSetUnorderedAccessViews(0, 1, &m_renderData.renderTargets[*m_renderData.m_MainRenderTargets[!m_renderData.m_currentMainRTindex]].uav, nullptr);
	m_renderer->DispatchComputeShader(handle, m_renderer->GetRenderTargetWidth(), m_renderer->GetRenderTargetHeight(), 1);

	m_renderData.m_pImmediateContext->CSSetShader(nullptr, nullptr, 0);
	m_renderData.m_pImmediateContext->CSSetShaderResources(0, 2, nullShadowSrvs);
	m_renderData.m_pImmediateContext->CSSetUnorderedAccessViews(0, 1, nullUavArr, nullptr);

	//////////////////////////////////////////////////////////////////////////
	//Combine the main lighting and blurred textures
	handle = (graphicsResources[(int)ObjectType::COMPUTE_SHADER]).at("Texture2DAdd");
	m_renderer->BindComputeShader(handle);
	m_renderer->BindTextureShaderResource(ObjectType::COMPUTE_SHADER, 0, 1, m_bloomTextureHandle);
	m_renderer->BindTextureShaderResource(ObjectType::COMPUTE_SHADER, 1, 1, m_renderData.m_MainRenderTargets[!m_renderData.m_currentMainRTindex]);
	m_renderData.m_pImmediateContext->CSSetUnorderedAccessViews(0, 1, &m_renderData.renderTargets[*m_renderData.m_MainRenderTargets[m_renderData.m_currentMainRTindex]].uav, nullptr);
	m_renderer->DispatchComputeShader(handle, m_renderer->GetRenderTargetWidth(), m_renderer->GetRenderTargetHeight(), 1);

	m_renderData.m_pImmediateContext->CSSetShader(nullptr, nullptr, 0);
	m_renderData.m_pImmediateContext->CSSetShaderResources(0, 2, nullShadowSrvs);
	m_renderData.m_pImmediateContext->CSSetUnorderedAccessViews(0, 1, nullUavArr, nullptr);
}

void BloomRenderStage::PostRender()
{
	if (!m_renderData.m_isUsingBloom)
	{
		//Re-add the render target
		m_renderer->BindRenderTarget(m_renderData.m_MainRenderTargets[m_renderData.m_currentMainRTindex], false);//No depth testing
	}
}