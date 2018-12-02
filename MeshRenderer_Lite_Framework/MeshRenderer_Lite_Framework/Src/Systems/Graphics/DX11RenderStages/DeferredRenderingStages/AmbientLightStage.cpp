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

			m_renderer->BindRenderTarget(m_renderData.m_MainRenderTargets[m_renderData.m_AmbientOccRT], false);//No depth testing 
			m_renderer->ClearRenderTarget(m_renderData.m_MainRenderTargets[m_renderData.m_AmbientOccRT], XMVectorSet(0,0,0,0) );
			
			//Bind the G-buffer render targets
			for (char rtHandle = 0, textureId = 20; rtHandle < (char)DX11RendererData::GBufferRTType::COUNT; ++rtHandle, ++textureId)
			{
				m_renderer->BindTextureShaderResource(ObjectType::PIXEL_SHADER, textureId, 1, m_renderData.m_GBufferObjHandles[rtHandle]);
			}

			//invoke amb occ PS
			m_renderer->DrawIndexed(6);

			//Unbind the Ambient Map as RT cause now we're going to blur it
			m_renderer->BindNullRenderTarget();
		}
		
		m_renderer->BindRenderTarget(m_renderData.m_MainRenderTargets[m_renderData.m_currentMainRTindex], false);//No depth testing 
		m_renderer->ClearRenderTarget(m_renderData.m_MainRenderTargets[m_renderData.m_currentMainRTindex],
			m_renderData.testGlobalShaderProperties.gClearColor);

		//Bind the G-buffer render targets
		for (char rtHandle = 0, textureId = 20; rtHandle < (char)DX11RendererData::GBufferRTType::COUNT; ++rtHandle, ++textureId)
		{
			m_renderer->BindTextureShaderResource(ObjectType::PIXEL_SHADER, textureId, 1, m_renderData.m_GBufferObjHandles[rtHandle]);
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
