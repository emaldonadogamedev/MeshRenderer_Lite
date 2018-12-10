#include<Utilities/precompiled.h>
#include <Systems/Graphics/DX11RenderStages/LightVolumeStage/LightVolumeStage.h>

#include<Systems/Graphics/DX11Renderer/DX11Renderer.h>
#include<Systems/Graphics/DX11Renderer/DX11RendererData.h>
#include <Systems/Graphics/Components/LightComponents/Light.h>
#include <Systems/Graphics/Components/LightComponents/ShadowLightComponent/ShadowLightComponent.h>

LightVolumeStage::LightVolumeStage(DX11Renderer* const renderer, RenderCompUmap* const gfxComponents, const ObjectHandle& fsqIndexBuffer)
	:IRenderStage(renderer, gfxComponents)
{

}

LightVolumeStage::~LightVolumeStage()
{

}

void LightVolumeStage::PreRender()
{
	m_renderer->BindRenderTarget(m_renderData.m_MainRenderTargets[m_renderData.m_currentMainRTindex], false);
	m_renderData.m_pImmediateContext->RSSetState(m_renderData.m_d3dRasterStateSolCullNone);
	m_renderData.m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	m_renderer->EnableAdditiveBlending();
	m_renderer->BindNullVertexBuffer();
	m_renderer->BindIndexBuffer(m_fsqIndexBuffer);
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

	//Bind shaders
	ObjectHandle handle = (graphicsResources[(int)ObjectType::VERTEX_SHADER]).at("LightVolumeVS");
	m_renderer->BindVertexShader(handle);

	handle = (graphicsResources[(int)ObjectType::PIXEL_SHADER]).at("SimplePS");
	m_renderer->BindPixelShader(handle);

	//Set view const buffers
	m_renderData.m_pImmediateContext->VSSetConstantBuffers(1, 1, &m_renderData.testViewProjConstBuffer);
	m_renderData.m_pImmediateContext->PSSetConstantBuffers(1, 1, &m_renderData.testViewProjConstBuffer);

	//Bind light volume const buffer
	m_renderData.m_pImmediateContext->VSSetConstantBuffers(10, 1, &m_renderData.testLightVolumePropertiesConstBuffer);

	m_renderer->DrawIndexed(6);
}

void LightVolumeStage::PostRender()
{

}