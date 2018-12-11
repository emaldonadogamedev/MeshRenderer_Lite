#include<Utilities/precompiled.h>
#include <Systems/Graphics/DX11RenderStages/LightVolumeStage/LightVolumeStage.h>

#include<Systems/Graphics/DX11Renderer/DX11Renderer.h>
#include<Systems/Graphics/DX11Renderer/DX11RendererData.h>
#include <Systems/Graphics/Components/LightComponents/Light.h>
#include <Systems/Graphics/Components/LightComponents/ShadowLightComponent/ShadowLightComponent.h>
#include <Systems/Graphics/GraphicsUtilities/VertexTypes.h>

LightVolumeStage::LightVolumeStage(DX11Renderer* const renderer, RenderCompUmap* const gfxComponents, const ObjectHandle& quadVertexBuffer)
	:IRenderStage(renderer, gfxComponents)
	, m_quadVertexBuffer(quadVertexBuffer)
{

}

LightVolumeStage::~LightVolumeStage()
{

}

void LightVolumeStage::PreRender()
{
	m_renderer->BindRenderTarget(m_renderData.m_MainRenderTargets[m_renderData.m_currentMainRTindex], true);
	m_renderData.m_pImmediateContext->RSSetState(m_renderData.m_d3dRasterStateSolCullNone);
	m_renderData.m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);
	m_renderer->EnableAdditiveBlending();
	m_renderer->BindVertexBuffer(m_quadVertexBuffer, sizeof(VertexAnimation));
}

void LightVolumeStage::Render(HandleDictionaryVec& graphicsResources, const float dt)
{
	//TODO: not just get the FIRST light
	//Update light volume const buffer
	const auto& shadowLight = m_gfxSystemComponents->at(ComponentType::RENDERABLE_LIGHT_WITH_SHADOW)[0];
	const ShadowLightComponent* shadowLighComp = (const ShadowLightComponent*)shadowLight;
	const auto shadowLightPtr = shadowLighComp->GetLight();

	const auto& up = shadowLighComp->GetUpVector();
	const auto& lookAt = shadowLighComp->GetLookAtVector();
	const auto& right = shadowLighComp->GetRightVector();

	m_renderData.testLightVolumeProperties.volumeLightPos = shadowLightPtr->m_position;

	m_renderData.testLightVolumeProperties.volumeLightUpVector =
		XMFLOAT3(up.m128_f32[0], up.m128_f32[1], up.m128_f32[2]);

	m_renderData.testLightVolumeProperties.volumeLightForwardVector =
		XMFLOAT3(lookAt.m128_f32[0], lookAt.m128_f32[1], lookAt.m128_f32[2]);

	m_renderData.testLightVolumeProperties.volumeLightRightVector =
		XMFLOAT3(right.m128_f32[0], right.m128_f32[1], right.m128_f32[2]);

	m_renderData.m_pImmediateContext->UpdateSubresource(m_renderData.testLightVolumePropertiesConstBuffer,
		0, NULL, &m_renderData.testLightVolumeProperties, 0, 0);

	
	//////////////////////////////////////////////////////////////////////////
	// Vertex Shader
	ObjectHandle handle = (graphicsResources[(int)ObjectType::VERTEX_SHADER]).at("LightVolumeVS");
	m_renderer->BindVertexShader(handle);
	m_renderData.m_pImmediateContext->VSSetConstantBuffers(1, 1, &m_renderData.testViewProjConstBuffer);
	
	//Bind light volume const buffer
	m_renderData.m_pImmediateContext->VSSetConstantBuffers(10, 1, &m_renderData.testLightVolumePropertiesConstBuffer);

	//////////////////////////////////////////////////////////////////////////
	// Hull Shader
	handle = (graphicsResources[(int)ObjectType::HULL_SHADER]).at("QuadDiviveHS");
	m_renderer->BindHullShader(handle);

	//////////////////////////////////////////////////////////////////////////
	// Domain Shader
	handle = (graphicsResources[(int)ObjectType::DOMAIN_SHADER]).at("LightVolumeDS");
	m_renderer->BindDomainShader(handle);
	m_renderData.m_pImmediateContext->DSSetConstantBuffers(1, 1, &m_renderData.testViewProjConstBuffer);
    m_renderData.m_pImmediateContext->DSSetConstantBuffers(10, 1, &m_renderData.testLightVolumePropertiesConstBuffer);
	//Bind all of the shadow maps
	const auto& lightComponents = (*m_gfxSystemComponents)[ComponentType::RENDERABLE_LIGHT_WITH_SHADOW];
	for (const auto& component : lightComponents)
	{
		ShadowLightComponent* lightComp = (ShadowLightComponent*)component;
		if (lightComp->IsUsingShadows())
		{
			m_renderer->BindTextureShaderResource(ObjectType::DOMAIN_SHADER, lightComp->GetShadowTextureIdx(), 1, lightComp->GetShadowDepthMapHandle());
		}
	}


	//////////////////////////////////////////////////////////////////////////
	//Pixel Shader
	handle = (graphicsResources[(int)ObjectType::PIXEL_SHADER]).at("LightVolumePS");
	m_renderer->BindPixelShader(handle);
	m_renderData.m_pImmediateContext->PSSetConstantBuffers(1, 1, &m_renderData.testViewProjConstBuffer);

	m_renderer->Draw(4);
}

void LightVolumeStage::PostRender()
{
	//clear domain and hull shaders
	m_renderData.m_pImmediateContext->HSSetShader(nullptr, nullptr, 0);
	m_renderData.m_pImmediateContext->DSSetShader(nullptr, nullptr, 0);
	m_renderData.m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
}