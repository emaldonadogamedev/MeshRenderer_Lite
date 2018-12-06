#include <Utilities/precompiled.h>
#include <Systems//Graphics/DX11RenderStages/DeferredRenderingStages/AmbientLightStage.h>

#include<Systems/Graphics/DX11Renderer/DX11Renderer.h>
#include<Systems/Graphics/DX11Renderer/DX11RendererData.h>

AmbientLightStage::AmbientLightStage(DX11Renderer* const renderer, RenderCompUmap* const gfxComponents, const ObjectHandle& fsqIndexBuffer)
		:IRenderStage(renderer, gfxComponents)
		, m_fsqIndexBuffer(fsqIndexBuffer)
{

}

AmbientLightStage::~AmbientLightStage()
{

}

void AmbientLightStage::PreRender()
{
		//bind main render target and clear it
		m_renderData.m_currentMainRTindex = 0;
		m_renderData.m_pImmediateContext->RSSetState(m_renderData.m_d3dRasterStateSolCullNone);
		m_renderData.m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		m_renderer->DisableColorBlending();

		m_renderData.m_pImmediateContext->RSSetViewports(1, &m_renderData.m_mainViewport);
}

void AmbientLightStage::Render(HandleDictionaryVec& graphicsResources, const float dt)
{
		ObjectHandle handle = (graphicsResources[(int)ObjectType::VERTEX_SHADER]).at("FullScreenQuadVS");
		m_renderer->BindVertexShader(handle);

		m_renderer->BindNullVertexBuffer(); //we create the geometry on the Vertex Shader
		m_renderer->BindIndexBuffer(m_fsqIndexBuffer);

		if(m_renderData.testGlobalShaderProperties.gIsUsingAmbientOcclussion)
		{
			//Create the ambien occlussion map
			handle = (graphicsResources[(int)ObjectType::PIXEL_SHADER]).at("CreateAmbientOccMapPS");
			m_renderer->BindPixelShader(handle);

			m_renderer->BindRenderTarget(m_renderData.m_AmbientOccMapRT, false);//No depth testing 
			m_renderer->ClearRenderTarget(m_renderData.m_AmbientOccMapRT, XMVectorSet(0,0,0,0) );
			
			//Bind the G-buffer render targets
			for (char rtHandle = 0, textureId = 20; rtHandle < (char)DX11RendererData::GBufferRTType::COUNT; ++rtHandle, ++textureId)
			{
				m_renderer->BindTextureShaderResource(ObjectType::PIXEL_SHADER, textureId, 1, m_renderData.m_GBufferObjHandles[rtHandle]);
			}

			//invoke amb occ PS
			m_renderer->DrawIndexed(6);

			//Unbind the Ambient Map as RT cause now we're going to blur it
			m_renderer->BindNullRenderTarget();

			//////////////////////////////////////////////////////////////////////////
			//Horizontal blur
			handle = (graphicsResources[(int)ObjectType::COMPUTE_SHADER]).at("AOmapBlur_Horizontal"); 
			m_renderer->BindComputeShader(handle);

			//Bind the G-buffer pos. and normal render targets
			m_renderer->BindTextureShaderResource(ObjectType::COMPUTE_SHADER, 20, 1, m_renderData.m_GBufferObjHandles[0]);
			m_renderer->BindTextureShaderResource(ObjectType::COMPUTE_SHADER, 21, 1, m_renderData.m_GBufferObjHandles[1]);

			//Bind resources
			m_renderer->BindTextureShaderResource(ObjectType::COMPUTE_SHADER, 0, 1, m_renderData.m_AmbientOccMapRT);
			m_renderData.m_pImmediateContext->CSSetUnorderedAccessViews(0, 1, &m_renderData.renderTargets[*m_renderData.AOtempBlurTexture].uav, nullptr);
			m_renderData.m_pImmediateContext->CSSetShaderResources(1, 1, &m_renderData.structuredBuffers[*m_renderData.AOblurSampleBuffer].srv);
			m_renderer->DispatchComputeShader(handle, (m_renderer->GetRenderTargetWidth() / 128) + 1, 1, 1);

			//////////////////////////////////////////////////////////////////////////
			// Cleanup before vertical blur
			static ID3D11ShaderResourceView* const nullShadowSrvs[2] = { nullptr,nullptr };
			static ID3D11UnorderedAccessView* const nullUavArr[1] = { nullptr };

			m_renderData.m_pImmediateContext->CSSetShader(nullptr, nullptr, 0);
			m_renderData.m_pImmediateContext->CSSetShaderResources(0, 2, nullShadowSrvs);
			m_renderData.m_pImmediateContext->CSSetUnorderedAccessViews(0, 1, nullUavArr, nullptr);

			//////////////////////////////////////////////////////////////////////////
			//Vertical blur

			handle = (graphicsResources[(int)ObjectType::COMPUTE_SHADER]).at("AOmapBlur_Vertical");
			m_renderer->BindComputeShader(handle);

			//Bind the G-buffer pos. and normal render targets
			m_renderer->BindTextureShaderResource(ObjectType::COMPUTE_SHADER, 20, 1, m_renderData.m_GBufferObjHandles[0]);
			m_renderer->BindTextureShaderResource(ObjectType::COMPUTE_SHADER, 21, 1, m_renderData.m_GBufferObjHandles[1]);

			//Bind resources
			m_renderer->BindTextureShaderResource(ObjectType::COMPUTE_SHADER, 0, 1, m_renderData.AOtempBlurTexture);
			m_renderData.m_pImmediateContext->CSSetUnorderedAccessViews(0, 1, &m_renderData.renderTargets[*m_renderData.m_AmbientOccMapRT].uav, nullptr);
			m_renderData.m_pImmediateContext->CSSetShaderResources(1, 1, &m_renderData.structuredBuffers[*m_renderData.AOblurSampleBuffer].srv);
			m_renderer->DispatchComputeShader(handle, 1, (m_renderer->GetRenderTargetHeight() / 128) + 1,  1);
		}
		
		m_renderer->BindRenderTarget(m_renderData.m_MainRenderTargets[m_renderData.m_currentMainRTindex], false);//No depth testing 
		m_renderer->ClearRenderTarget(m_renderData.m_MainRenderTargets[m_renderData.m_currentMainRTindex],
			m_renderData.testGlobalShaderProperties.gClearColor);

		//Bind the G-buffer render targets
		for (char rtHandle = 0, textureId = 20; rtHandle < (char)DX11RendererData::GBufferRTType::COUNT; ++rtHandle, ++textureId)
		{
			m_renderer->BindTextureShaderResource(ObjectType::PIXEL_SHADER, textureId, 1, m_renderData.m_GBufferObjHandles[rtHandle]);
		}

		if (m_renderData.testGlobalShaderProperties.gIsUsingAmbientOcclussion)
		{
			m_renderer->BindTextureShaderResource(ObjectType::PIXEL_SHADER, 28, 1, m_renderData.m_AmbientOccMapRT);
		}

		handle = (graphicsResources[(int)ObjectType::PIXEL_SHADER]).at("DeferredAmbientStagePS");
		m_renderer->BindPixelShader(handle);

		m_renderData.m_pImmediateContext->PSSetConstantBuffers(7, 1, &m_renderData.testLightWithShadowConstBuffer);
		m_renderData.m_pImmediateContext->PSSetConstantBuffers(9, 1, &m_renderData.testGlobalShaderPropertiesConstBuffer);

		m_renderer->DrawIndexed(6);
}

void AmbientLightStage::PostRender()
{
}
