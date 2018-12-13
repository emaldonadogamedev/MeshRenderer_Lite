#include <Utilities/precompiled.h>
#include <Systems/Graphics/DX11Renderer/DX11Renderer.h>

#include <Systems/Graphics/Components/LightComponents/Light.h>
#include <Systems/Graphics/Components/LightComponents/ShadowLightComponent/ShadowLightComponent.h>
#include <Systems/Graphics/DX11Renderer/DX11RendererData.h>
#include <Systems/Graphics/GraphicsUtilities/ObjectHandle.h>
#include <Systems/Graphics/GraphicsUtilities/VertexTypes.h>

DX11Renderer::DX11Renderer() : m_renderData(std::make_unique<DX11RendererData>())
{
}

DX11Renderer::~DX11Renderer()
{

}

bool DX11Renderer::InitializeRenderer(const int width, const int height, HWND hwnd)
{
	if (!InitializeD3D(width, height, hwnd))
		return false;

	if (!InitializeTestData(width, height))
		return false;

	return m_isInitialized = true;
}


void DX11Renderer::ReleaseData()
{
	//////////////////////////////////////////////////////////////////////////
	//cleanup objects

	//render targets
	for (auto& rt : m_renderData->renderTargets)
	{
		SafeRelease(rt.rtv);
		SafeRelease(rt.srv);
		SafeRelease(rt.texture2D);
		SafeRelease(rt.depthBuffer);
		SafeRelease(rt.depthStencilView);
	}

	//1D textures
	for (auto& tex1D : m_renderData->textures1D)
		SafeRelease(tex1D.srv);

	//2D textures
	for (auto& tex2D : m_renderData->textures2D)
		SafeRelease(tex2D.srv);

	//3D textures
	for (auto& tex3D : m_renderData->textures3D)
		SafeRelease(tex3D.srv);

	//vertex shaders
	for (auto& vs : m_renderData->vertexShaders) {
		SafeRelease(vs.vertexShader);
		SafeRelease(vs.shaderBlob);
		SafeRelease(vs.layout);
	}

	//pixel shaders
	for (auto& ps : m_renderData->pixelShaders) {
		SafeRelease(ps.pixelShader);
		SafeRelease(ps.shaderBlob);
	}

	//geometry shaders
	for (auto& gs : m_renderData->geometryShaders) {
		SafeRelease(gs.geometryShader);
		SafeRelease(gs.shaderBlob);
	}

	//compute shaders
	for (auto& cs : m_renderData->computeShaders) {
			SafeRelease(cs.computeShader);
			SafeRelease(cs.shaderBlob);
	}

	//hull / tessellation shaders
	for (auto& hs : m_renderData->hullShaders) {
			SafeRelease(hs.hullShader);
			SafeRelease(hs.shaderBlob);
	}

	//hull / tessellation shaders
	for (auto& ds : m_renderData->domainShaders) {
		SafeRelease(ds.domainShader);
		SafeRelease(ds.shaderBlob);
	}

	//vertex buffers
	for (auto& vb : m_renderData->vertexBuffers)
		SafeRelease(vb.buffer);

	//index buffers
	for (auto& ib : m_renderData->indexBuffers)
		SafeRelease(ib.buffer);

	//constant buffers
	for (auto& cb : m_renderData->constantBuffers)
		SafeRelease(cb.buffer);

	//structured buffers
	for (auto& sb : m_renderData->structuredBuffers)
	{
		SafeRelease(sb.buffer);
		SafeRelease(sb.srv);
	}

	//structured rw buffers
	for (auto& sbrw : m_renderData->structuredBuffersRW)
	{
		SafeRelease(sbrw.buffer);
		SafeRelease(sbrw.uav);
	}

	//structured rw texture2D buffers
	for (auto& sbrw : m_renderData->structuredBuffersRW_Texture2D)
	{
		SafeRelease(sbrw.texture2D);
		SafeRelease(sbrw.srv);
		SafeRelease(sbrw.uav);
	}

	//////////////////////////////////////////////////////////////////////////

	//cleanup rasterizers
	SafeRelease(m_renderData->m_d3dRasterStateSolCullNone);
	SafeRelease(m_renderData->m_d3dRasterStateSolCullBack);
	SafeRelease(m_renderData->m_d3dRasterStateSolCullFront);
	SafeRelease(m_renderData->m_d3dRasterStateWireframe);
	SafeRelease(m_renderData->m_d3dRasterStateImgui);

	//cleanup test const buffers
	SafeRelease(m_renderData->testPerObjectConstBuffer);
	SafeRelease(m_renderData->testViewProjConstBuffer);
	SafeRelease(m_renderData->testAnimationConstBuffer);
	SafeRelease(m_renderData->testSimpleClothConstBuffer);
	SafeRelease(m_renderData->testLightWithShadowConstBuffer);
	SafeRelease(m_renderData->testLightViewConstBuffer);
	SafeRelease(m_renderData->testMeshMaterialConstBuffer);

	//cleanup texture samplers
	SafeRelease(m_renderData->m_pWrapSamplerState);
	SafeRelease(m_renderData->m_pMirrorSamplerState);
	SafeRelease(m_renderData->m_pClampSamplerState);
	SafeRelease(m_renderData->m_pBorderSamplerState);

	//CLEANUP DIRECT3D
	if (m_renderData->m_pImmediateContext)
		m_renderData->m_pImmediateContext->ClearState();

	SafeRelease(m_renderData->m_pBackBufferRenderTargetView);
	SafeRelease(m_renderData->m_pSwapChain);
	SafeRelease(m_renderData->m_pImmediateContext);
	SafeRelease(m_renderData->m_pDevice);
}

void DX11Renderer::ClearMainBuffer() const
{
	m_renderData->m_pImmediateContext->ClearRenderTargetView(m_renderData->m_pBackBufferRenderTargetView, 
			m_renderData->testGlobalShaderProperties.gClearColor.m128_f32);
	m_renderData->m_pImmediateContext->ClearDepthStencilView(m_renderData->m_DepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);
}

void DX11Renderer::SwapBuffers(void) const
{
	HR(m_renderData->m_pSwapChain->Present(0, 0));
}

DX11RendererData& DX11Renderer::GetRendererData()
{
	return *m_renderData;
}

void DX11Renderer::Draw(unsigned vertexCount, unsigned startVertexLocation) const
{
	m_renderData->m_pImmediateContext->Draw(vertexCount, startVertexLocation);
}

void DX11Renderer::DrawIndexed(unsigned indexCount, unsigned startIndexLocation, unsigned baseVertexLocation) const
{
	m_renderData->m_pImmediateContext->DrawIndexed(indexCount, startIndexLocation, baseVertexLocation);
}

void DX11Renderer::DrawInstanced(unsigned vertexCount, unsigned instanceCount, unsigned startVertexLocation, unsigned startInstanceLocation) const
{
	m_renderData->m_pImmediateContext->DrawInstanced(vertexCount, instanceCount, startVertexLocation, startInstanceLocation);
}

void DX11Renderer::DrawIndexedInstanced(unsigned indexCountPerInstance, unsigned instanceCount, unsigned startIndexLocation, unsigned baseVertexLocation, unsigned startInstanceLocation) const
{
	m_renderData->m_pImmediateContext->DrawIndexedInstanced(indexCountPerInstance, instanceCount, startIndexLocation, baseVertexLocation, startInstanceLocation);
}

void DX11Renderer::CreateVertexBuffer(ObjectHandle& vertexBuffer, const BufferUsage bufferUsage, unsigned size, const void* initialData /*= nullptr*/)
{
	//Create zeroed out buffer description
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));

	//Establish description usage
	switch (bufferUsage)
	{
	case BufferUsage::USAGE_DEFAULT:
		desc.Usage = D3D11_USAGE_DEFAULT;
		break;
	case BufferUsage::USAGE_IMMUTABLE:
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		break;
	case BufferUsage::USAGE_DYNAMIC:
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		break;
	case BufferUsage::USAGE_STAGING:
		desc.Usage = D3D11_USAGE_STAGING;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		break;
	default:
		break;
	}

	//Specify buffer description info
	desc.ByteWidth = size;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	//Create zeroed out subresource data
	D3D11_SUBRESOURCE_DATA srd;
	ZeroMemory(&srd, sizeof(D3D11_SUBRESOURCE_DATA));

	srd.pSysMem = initialData;

	ID3D11Buffer* vBuffer;
	//Create buffer, if no initial data, it creates an empty buffer.
	HR(m_renderData->m_pDevice->CreateBuffer(&desc, (initialData) ? &srd : nullptr, &vBuffer));

	//If handle already exists, release preexisting buffer and update data.
	if (vertexBuffer)
	{
		Buffer& vBufferObj = m_renderData->vertexBuffers[*vertexBuffer];

		if (vBufferObj.buffer)
			vBufferObj.buffer->Release();

		vBufferObj.size = size;
		vBufferObj.buffer = vBuffer;
		vBufferObj.usage = bufferUsage;
	}

	//Handle needs to be created
	else
	{
		Buffer vBufferObj;
		vBufferObj.size = size;
		vBufferObj.buffer = vBuffer;
		vBufferObj.usage = bufferUsage;

		int index = m_renderData->NextAvailableIndex(m_renderData->vertexBuffers);

		//If no available space in container, push it back
		if (index == -1)
		{
			m_renderData->vertexBuffers.push_back(vBufferObj);
			vertexBuffer = CreateHandle(ObjectType::VERTEX_BUFFER, m_renderData->vertexBuffers.size() - 1);
		}

		else
		{
			//Use the available free space in the container
			m_renderData->vertexBuffers[index] = vBufferObj;
			vertexBuffer = CreateHandle(ObjectType::VERTEX_BUFFER, index);
		}
	}
}

void DX11Renderer::CreateIndexBuffer(ObjectHandle& indexBuffer, const BufferUsage bufferUsage, unsigned size, const void* initialData /*= nullptr*/)
{
	//Create zeroed out buffer description
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));

	//Specify correct buffer usage
	switch (bufferUsage)
	{
	case BufferUsage::USAGE_DEFAULT:
		desc.Usage = D3D11_USAGE_DEFAULT;
		break;

	case BufferUsage::USAGE_IMMUTABLE:
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		break;

	case BufferUsage::USAGE_DYNAMIC:
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		break;

	case BufferUsage::USAGE_STAGING:
		desc.Usage = D3D11_USAGE_STAGING;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		break;

	default: 
			break;
	}

	//Specify buffer description data
	desc.ByteWidth = size;
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	//Zeroed out subresource data
	D3D11_SUBRESOURCE_DATA srd;
	ZeroMemory(&srd, sizeof(D3D11_SUBRESOURCE_DATA));
	srd.pSysMem = initialData;

	ID3D11Buffer* iBuffer;

	HR(m_renderData->m_pDevice->CreateBuffer(&desc, (initialData) ? &srd : nullptr, &iBuffer));

	if (indexBuffer)
	{
		Buffer& indexBufferObj = m_renderData->indexBuffers[*indexBuffer];

		//Release memory if it is being used
		if (indexBufferObj.buffer)
			indexBufferObj.buffer->Release();

		//Update data
		indexBufferObj.buffer = iBuffer;
		indexBufferObj.usage = bufferUsage;
		indexBufferObj.size = size;
	}

	else
	{
		Buffer indexBufferObj;

		//Assign data
		indexBufferObj.buffer = iBuffer;
		indexBufferObj.usage = bufferUsage;
		indexBufferObj.size = size;

		int index = m_renderData->NextAvailableIndex(m_renderData->indexBuffers);

		//No free space in container, append object and create handle
		if (index == -1)
		{
			m_renderData->indexBuffers.push_back(indexBufferObj);
			indexBuffer = CreateHandle(ObjectType::INDEX_BUFFER, m_renderData->indexBuffers.size() - 1);
		}

		else
		{
			//Use free space in the container and create handle
			m_renderData->indexBuffers[index] = indexBufferObj;
			indexBuffer = CreateHandle(ObjectType::INDEX_BUFFER, index);
		}
	}
}

void DX11Renderer::CreateConstantBuffer(ObjectHandle& constantBuffer, const BufferUsage bufferUsage, unsigned size, const void* initialData)
{
	//Create zeroed out buffer description
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	//Specify correct buffer usage
	switch (bufferUsage)
	{
	case BufferUsage::USAGE_DEFAULT:
			desc.Usage = D3D11_USAGE_DEFAULT;
			break;

	case BufferUsage::USAGE_IMMUTABLE:
			desc.Usage = D3D11_USAGE_IMMUTABLE;
			break;

	case BufferUsage::USAGE_DYNAMIC:
			desc.Usage = D3D11_USAGE_DYNAMIC;
			desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			break;

	case BufferUsage::USAGE_STAGING:
			desc.Usage = D3D11_USAGE_STAGING;
			desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
			break;

	default:
			break;
	}

	//Need to ensure that memory is aligned to 16 bytes.
	desc.ByteWidth = size;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	//Create Buffer
	ID3D11Buffer* buffer;

	//Zeroed out subresource data
	D3D11_SUBRESOURCE_DATA srd;
	ZeroMemory(&srd, sizeof(D3D11_SUBRESOURCE_DATA));
	srd.pSysMem = initialData;

	HR(m_renderData->m_pDevice->CreateBuffer(&desc, initialData ? &srd : nullptr, &buffer));

	if (constantBuffer)
	{
		Buffer& constBufferObj = m_renderData->constantBuffers[*constantBuffer];

		//Free buffer if it has data
		if (constBufferObj.buffer)
			constBufferObj.buffer->Release();

		//Update data
		constBufferObj.buffer = buffer;
		constBufferObj.usage = bufferUsage;
		constBufferObj.size = size;
	}

	else
	{
		Buffer constBufferObj;

		//Update data
		constBufferObj.buffer = buffer;
		constBufferObj.usage = bufferUsage;
		constBufferObj.size = size;

		int index = m_renderData->NextAvailableIndex(m_renderData->constantBuffers);

		if (index == -1)
		{
			//No free space exists in container, push back
			m_renderData->constantBuffers.push_back(constBufferObj);
			constantBuffer = CreateHandle(ObjectType::CONSTANT_BUFFER, m_renderData->constantBuffers.size() - 1);
		}

		else
		{
			//Use available space in container
			m_renderData->constantBuffers[index] = constBufferObj;
			constantBuffer = CreateHandle(ObjectType::CONSTANT_BUFFER, index);
		}
	}
}

void DX11Renderer::CreateStructuredBuffer(ObjectHandle& structuredBuffer, const BufferUsage bufferUsage, unsigned numOfElements, 
		unsigned stride, const void* initialData /*= nullptr*/)
{
		//Create zeroed out buffer description
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		//Specify correct buffer usage
		switch (bufferUsage)
		{
		case BufferUsage::USAGE_DEFAULT:
				desc.Usage = D3D11_USAGE_DEFAULT;
				break;

		case BufferUsage::USAGE_IMMUTABLE:
				desc.Usage = D3D11_USAGE_IMMUTABLE;
				break;

		case BufferUsage::USAGE_DYNAMIC:
				desc.Usage = D3D11_USAGE_DYNAMIC;
				desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
				break;

		case BufferUsage::USAGE_STAGING:
				desc.Usage = D3D11_USAGE_STAGING;
				desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
				break;

		default:
				break;
		}

		//Need to ensure that memory is aligned to 16 bytes.
		desc.ByteWidth = numOfElements * stride;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.StructureByteStride = stride;
		desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

		//Create Buffer
		ID3D11Buffer* buffer;

		//Zeroed out subresource data
		D3D11_SUBRESOURCE_DATA srd;
		ZeroMemory(&srd, sizeof(D3D11_SUBRESOURCE_DATA));
		srd.pSysMem = initialData;

		HR(m_renderData->m_pDevice->CreateBuffer(&desc, initialData ? &srd : nullptr, &buffer));

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		ZeroMemory(&srvDesc, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
		srvDesc.Buffer.NumElements = numOfElements;

		ID3D11ShaderResourceView* srvPtr = nullptr;
		HR(m_renderData->m_pDevice->CreateShaderResourceView(buffer, &srvDesc, &srvPtr));

		if (structuredBuffer)
		{
			StructuredBuffer& structuredBufferObj = m_renderData->structuredBuffers[*structuredBuffer];

			//Free buffer if it has data
			SafeRelease(structuredBufferObj.buffer);
			SafeRelease(structuredBufferObj.srv);

			//Update data
			structuredBufferObj.buffer = buffer;
			structuredBufferObj.srv = srvPtr;
			structuredBufferObj.usage = bufferUsage;
			structuredBufferObj.size = desc.ByteWidth;
		}

		else
		{
			StructuredBuffer structuredBufferObj;

			//Update data
			structuredBufferObj.buffer = buffer;
			structuredBufferObj.srv = srvPtr;
			structuredBufferObj.usage = bufferUsage;
			structuredBufferObj.size = desc.ByteWidth;

			int index = m_renderData->NextAvailableIndex(m_renderData->structuredBuffers);

			if (index == -1)
			{
				//No free space exists in container, push back
				m_renderData->structuredBuffers.push_back(std::move(structuredBufferObj));
				structuredBuffer = CreateHandle(ObjectType::STRUCTURED_BUFFER, m_renderData->structuredBuffers.size() - 1);
			}

			else
			{
				//Use available space in container
				m_renderData->structuredBuffers[index] = structuredBufferObj;
				structuredBuffer = CreateHandle(ObjectType::STRUCTURED_BUFFER, index);
			}
		}
}

void DX11Renderer::CreateStructuredBufferRW(ObjectHandle& structuredBufferRW, const BufferUsage bufferUsage, unsigned numOfElements, unsigned stride, const void* initialData /*= nullptr*/)
{
		//Create zeroed out buffer description
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		//Specify correct buffer usage
		switch (bufferUsage)
		{
		case BufferUsage::USAGE_DEFAULT:
				desc.Usage = D3D11_USAGE_DEFAULT;
				break;

		case BufferUsage::USAGE_IMMUTABLE:
				desc.Usage = D3D11_USAGE_IMMUTABLE;
				break;

		case BufferUsage::USAGE_DYNAMIC:
				desc.Usage = D3D11_USAGE_DYNAMIC;
				desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
				break;

		case BufferUsage::USAGE_STAGING:
				desc.Usage = D3D11_USAGE_STAGING;
				desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
				break;

		default:
				break;
		}

		//Need to ensure that memory is aligned to 16 bytes.
		desc.ByteWidth = numOfElements * stride;
		desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
		desc.StructureByteStride = stride;

		//Create Buffer
		ID3D11Buffer* buffer;

		//Zeroed out subresource data
		D3D11_SUBRESOURCE_DATA srd;
		ZeroMemory(&srd, sizeof(D3D11_SUBRESOURCE_DATA));
		srd.pSysMem = initialData;

		HR(m_renderData->m_pDevice->CreateBuffer(&desc, initialData ? &srd : nullptr, &buffer));

		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
		ZeroMemory(&uavDesc, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));
		uavDesc.Format = DXGI_FORMAT_UNKNOWN;
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		uavDesc.Buffer.NumElements = numOfElements;

		ID3D11UnorderedAccessView* uavPtr = nullptr;
		HR(m_renderData->m_pDevice->CreateUnorderedAccessView(buffer, &uavDesc, &uavPtr));

		if (structuredBufferRW)
		{
				StructuredBufferRW& STRUCTURED_BUFFER_RW = m_renderData->structuredBuffersRW[*structuredBufferRW];

				//Free buffer if it has data
				if (STRUCTURED_BUFFER_RW.buffer)
				{
						SafeRelease(STRUCTURED_BUFFER_RW.buffer);
						SafeRelease(STRUCTURED_BUFFER_RW.uav);
				}

				//Update data
				STRUCTURED_BUFFER_RW.buffer = buffer;
				STRUCTURED_BUFFER_RW.uav = uavPtr;
				STRUCTURED_BUFFER_RW.usage = bufferUsage;
				STRUCTURED_BUFFER_RW.size = desc.ByteWidth;
		}

		else
		{
				StructuredBufferRW structuredBufferRWobj;

				//Update data
				structuredBufferRWobj.buffer = buffer;
				structuredBufferRWobj.uav = uavPtr;
				structuredBufferRWobj.usage = bufferUsage;
				structuredBufferRWobj.size = desc.ByteWidth;

				int index = m_renderData->NextAvailableIndex(m_renderData->structuredBuffersRW);

				if (index == -1)
				{
						//No free space exists in container, push back
						m_renderData->structuredBuffersRW.push_back(std::move(structuredBufferRWobj));
						structuredBufferRW = CreateHandle(ObjectType::STRUCTURED_BUFFER_RW, m_renderData->structuredBuffersRW.size() - 1);
				}

				else
				{
						//Use available space in container
						m_renderData->structuredBuffersRW[index] = structuredBufferRWobj;
						structuredBufferRW = CreateHandle(ObjectType::STRUCTURED_BUFFER_RW, index);
				}
		}
}

ObjectHandle DX11Renderer::GetTexture2D(const std::string& fileName)
{
	ObjectHandle texture2DHandle;
	CreateTexture2D(texture2DHandle, fileName);

	return texture2DHandle;
}

void DX11Renderer::CreateRenderTarget(ObjectHandle& rt, const int W, const int H, const DataFormat dataFormat, bool useDepthBuffer)
{
	D3D11_TEXTURE2D_DESC textureDesc;
	HRESULT result;
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;

	// Initialize the render target texture description.
	ZeroMemory(&textureDesc, sizeof(textureDesc));

	// Setup the render target texture description.
	textureDesc.Width = W;
	textureDesc.Height = H;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = m_renderData->dxgiFormatArrHelper[(int)dataFormat];
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	ID3D11Texture2D* tempTargetTexture = nullptr;

	// Create the render target texture.
	HR(m_renderData->m_pDevice->CreateTexture2D(&textureDesc, nullptr, &tempTargetTexture));

	// Setup the description of the render target view.
	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	ID3D11RenderTargetView* renderTargetView = nullptr;
	HR(m_renderData->m_pDevice->CreateRenderTargetView(tempTargetTexture, &renderTargetViewDesc, &renderTargetView));

	// Setup the description of the shader resource view.
	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	ID3D11ShaderResourceView* shaderResourceView = nullptr;

	HR(m_renderData->m_pDevice->CreateShaderResourceView(tempTargetTexture, &shaderResourceViewDesc, &shaderResourceView));


	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	ZeroMemory(&uavDesc, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));
	uavDesc.Format = textureDesc.Format;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	uavDesc.Texture2D.MipSlice = 0;

	ID3D11UnorderedAccessView* uavPtr = nullptr;
	HR(m_renderData->m_pDevice->CreateUnorderedAccessView(tempTargetTexture, &uavDesc, &uavPtr));


	ID3D11Texture2D* depthBuffer = nullptr;
	ID3D11DepthStencilView* stencilView = nullptr;

	if (useDepthBuffer) {
		//CREATE DEPTH STENCIL BUFFER
		D3D11_TEXTURE2D_DESC depthStencilDesc;
		depthStencilDesc.Width = W;
		depthStencilDesc.Height = H;
		depthStencilDesc.MipLevels = 1;
		depthStencilDesc.ArraySize = 1;
		depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
		depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
		depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthStencilDesc.CPUAccessFlags = 0;
		depthStencilDesc.MiscFlags = 0;

		HR(m_renderData->m_pDevice->CreateTexture2D(&depthStencilDesc, 0, &depthBuffer));

		D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
		ZeroMemory(&dsvd, sizeof(dsvd));

		dsvd.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

		HR(m_renderData->m_pDevice->CreateDepthStencilView(depthBuffer, &dsvd, &stencilView));
	}
	
	if (rt && rt.GetType() == ObjectType::RENDER_TARGET) {
		auto& renderTargetObj = m_renderData->renderTargets[*rt];

		SafeRelease(renderTargetObj.rtv);
		SafeRelease(renderTargetObj.uav);
		SafeRelease(renderTargetObj.texture2D);
		SafeRelease(renderTargetObj.srv);
		SafeRelease(renderTargetObj.depthStencilView);
		SafeRelease(renderTargetObj.depthBuffer);

		renderTargetObj.format = dataFormat;
		renderTargetObj.rtv = renderTargetView;
		renderTargetObj.uav = uavPtr;
		renderTargetObj.texture2D = tempTargetTexture;
		renderTargetObj.srv = shaderResourceView;
		renderTargetObj.depthBuffer = depthBuffer;
		renderTargetObj.depthStencilView = stencilView;
		renderTargetObj.width = W;
		renderTargetObj.height = H;
	}

	else {
		RenderTarget renderTargetObj;
		
		renderTargetObj.format = dataFormat;
		renderTargetObj.rtv = renderTargetView;
		renderTargetObj.uav = uavPtr;
		renderTargetObj.texture2D = tempTargetTexture;
		renderTargetObj.srv = shaderResourceView;
		renderTargetObj.depthBuffer = depthBuffer;
		renderTargetObj.depthStencilView = stencilView;
		renderTargetObj.width = W;
		renderTargetObj.height = H;

		const int index = m_renderData->NextAvailableIndex(m_renderData->renderTargets);

		//No free space in the texture container
		if (index == -1)
		{
			m_renderData->renderTargets.emplace_back(renderTargetObj);
			rt = CreateHandle(ObjectType::RENDER_TARGET, m_renderData->renderTargets.size() - 1);
		}

		//Insert into available slot
		else
		{
			m_renderData->renderTargets[index] = renderTargetObj;
			rt = CreateHandle(ObjectType::RENDER_TARGET, index);
		}

	}
}

void DX11Renderer::BindRenderTarget(const ObjectHandle& rt, const bool useDepthMap)
{
	if (rt && rt.GetType() == ObjectType::RENDER_TARGET) {
		const auto& renderTarget = m_renderData->renderTargets[*rt];
		m_renderData->m_pImmediateContext->OMSetRenderTargets(1, &renderTarget.rtv, 
				useDepthMap ? renderTarget.depthStencilView : nullptr);
	}
}

void DX11Renderer::BindNullRenderTarget(const ObjectHandle& depthMapHandle)
{
	m_renderData->m_pImmediateContext->OMSetRenderTargets(0, nullptr, 
			depthMapHandle ? m_renderData->depthBuffers[*depthMapHandle].depthStencilView : nullptr);
}

void DX11Renderer::ClearRenderTarget(const ObjectHandle& rt, const XMVECTOR& clearColor)
{
	ClearRenderTarget(rt, clearColor.m128_f32);
}

void DX11Renderer::ClearRenderTarget(const ObjectHandle& rt, const float colorArr[4])
{
		if (rt && rt.GetType() == ObjectType::RENDER_TARGET) 
		{
				const auto& rtObj = m_renderData->renderTargets[*rt];
				if (rtObj.rtv)
				{
						m_renderData->m_pImmediateContext->ClearRenderTargetView(rtObj.rtv, colorArr);
				}
				if (rtObj.depthStencilView)
				{
						m_renderData->m_pImmediateContext->ClearDepthStencilView(rtObj.depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);
				}
		}
}

void DX11Renderer::CreateStructuredBufferRW_Texture2D(ObjectHandle& sbrwTexture2Dhandle, const BufferUsage bufferUsage, 
		const DataFormat dataFormat, unsigned w, unsigned h)
{
		D3D11_TEXTURE2D_DESC texture2dDesc;
		ZeroMemory(&texture2dDesc, sizeof(D3D11_TEXTURE2D_DESC));
		texture2dDesc.Width = w;
		texture2dDesc.Height = h;
		texture2dDesc.MipLevels = texture2dDesc.ArraySize = 1;
		texture2dDesc.Format = m_renderData->dxgiFormatArrHelper[(int)dataFormat];
		texture2dDesc.SampleDesc.Count = 1;
		texture2dDesc.SampleDesc.Quality = 0;

		//Establish description usage
		switch (bufferUsage)
		{
		case BufferUsage::USAGE_DEFAULT:
				texture2dDesc.Usage = D3D11_USAGE_DEFAULT;
				break;
		case BufferUsage::USAGE_IMMUTABLE:
				texture2dDesc.Usage = D3D11_USAGE_IMMUTABLE;
				break;
		case BufferUsage::USAGE_DYNAMIC:
				texture2dDesc.Usage = D3D11_USAGE_DYNAMIC;
				texture2dDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
				break;
		case BufferUsage::USAGE_STAGING:
				texture2dDesc.Usage = D3D11_USAGE_STAGING;
				texture2dDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
				break;
		default:
				break;
		}

		texture2dDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;

		ID3D11Texture2D* texture2Dbuffer = nullptr;
		HR(m_renderData->m_pDevice->CreateTexture2D(&texture2dDesc, nullptr, &texture2Dbuffer));

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
		srvDesc.Format = texture2dDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;
		srvDesc.Texture2D.MostDetailedMip = 0;

		ID3D11ShaderResourceView* srvPtr = nullptr;
		HR(m_renderData->m_pDevice->CreateShaderResourceView(texture2Dbuffer, &srvDesc, &srvPtr));

		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
		ZeroMemory(&uavDesc, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));
		uavDesc.Format = texture2dDesc.Format;
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
		uavDesc.Texture2D.MipSlice = 0;

		ID3D11UnorderedAccessView* uavPtr = nullptr;
		HR(m_renderData->m_pDevice->CreateUnorderedAccessView(texture2Dbuffer, &uavDesc, &uavPtr));

		if (sbrwTexture2Dhandle && sbrwTexture2Dhandle.GetType() == ObjectType::STRUCTURED_BUFFER_RW_TEXTURE2D)
		{
				auto& sbrwTex2D = m_renderData->structuredBuffersRW_Texture2D[*sbrwTexture2Dhandle];

				SafeRelease(sbrwTex2D.srv);
				SafeRelease(sbrwTex2D.texture2D);
				SafeRelease(sbrwTex2D.uav);

				sbrwTex2D.texture2D = texture2Dbuffer;
				sbrwTex2D.uav = uavPtr;
				sbrwTex2D.srv = srvPtr;
				sbrwTex2D.width = w;
				sbrwTex2D.height = h;
		}
		else 
		{
				StructuredBufferRW_Texture2D sbrwTex2D;
				sbrwTex2D.texture2D = texture2Dbuffer;
				sbrwTex2D.uav = uavPtr;
				sbrwTex2D.srv = srvPtr;
				sbrwTex2D.width = w;
				sbrwTex2D.height = h;

				const int index = m_renderData->NextAvailableIndex(m_renderData->structuredBuffersRW_Texture2D);

				//No free space in the texture container
				if (index == -1)
				{
						m_renderData->structuredBuffersRW_Texture2D.push_back(std::move(sbrwTex2D));
						sbrwTexture2Dhandle = CreateHandle(ObjectType::STRUCTURED_BUFFER_RW_TEXTURE2D, m_renderData->structuredBuffersRW_Texture2D.size() - 1);
				}

				//Insert into available slot
				else
				{
						m_renderData->structuredBuffersRW_Texture2D[index] = sbrwTex2D;
						sbrwTexture2Dhandle = CreateHandle(ObjectType::STRUCTURED_BUFFER_RW_TEXTURE2D, index);
				}

		}
}

void DX11Renderer::BindNullVertexBuffer()
{
	//Bind NULL buffer - this is used for the particles
	m_renderData->m_pImmediateContext->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
}

void DX11Renderer::BindVertexBuffer(const ObjectHandle& vertexBuffer, unsigned stride)
{
	//Make sure we are binding the right type of buffer
	if (!vertexBuffer || vertexBuffer.GetType() != ObjectType::VERTEX_BUFFER)
	{
		return;
	}

	const unsigned offset = 0;

	//Get buffer from container
	ID3D11Buffer* buffer = m_renderData->vertexBuffers[*vertexBuffer].buffer;

	//Bind buffer
	m_renderData->m_pImmediateContext->IASetVertexBuffers(0, 1, &buffer, &stride, &offset);
}

void DX11Renderer::BindIndexBuffer(const ObjectHandle& indexBuffer)
{
	//Make sure we are binding the right type of buffer
	if (!indexBuffer || indexBuffer.GetType() != ObjectType::INDEX_BUFFER)
	{
		return;
	}

	//Get buffer from container
	ID3D11Buffer* buffer = m_renderData->indexBuffers[*indexBuffer].buffer;

	//Bind buffer
	m_renderData->m_pImmediateContext->IASetIndexBuffer(buffer, DXGI_FORMAT_R32_UINT, 0);
}

void DX11Renderer::BindConstantBuffer(unsigned slot, const ObjectHandle& constantBuffer, const ObjectType& shaderType)
{
	if (!constantBuffer || constantBuffer.GetType() != ObjectType::CONSTANT_BUFFER)
	{
		return;
	}

	ID3D11Buffer* buffer = m_renderData->constantBuffers[*constantBuffer].buffer;

	//Verify which type of shader we are binding the buffer to
	//Then bind buffer to that shader type
	if (shaderType == ObjectType::VERTEX_SHADER)
	{
		m_renderData->m_pImmediateContext->VSSetConstantBuffers(slot, 1, &buffer);
	}

	else if (shaderType == ObjectType::PIXEL_SHADER)
	{
		m_renderData->m_pImmediateContext->PSSetConstantBuffers(slot, 1, &buffer);
	}

	else if (shaderType == ObjectType::GEOMETRY_SHADER)
	{
		m_renderData->m_pImmediateContext->GSSetConstantBuffers(slot, 1, &buffer);
	}

	else if (shaderType == ObjectType::COMPUTE_SHADER)
	{
		m_renderData->m_pImmediateContext->CSSetConstantBuffers(slot, 1, &buffer);
	}
	else //shaderType is Hull Shader
	{
		m_renderData->m_pImmediateContext->HSSetConstantBuffers(slot, 1, &buffer);
	}
}

void DX11Renderer::CreateVertexShader(ObjectHandle& vertexShader, const std::string& fileName, const InputLayout& inputLayout,
	bool precompiled, const std::string& entryPoint)
{
	const std::string target = "vs_5_0";
	ID3DBlob* blob;
	int HResult = 0;
	CompileShaderHelper(HResult, &blob, fileName, target, entryPoint);

	if (SUCCEEDED(HResult))
	{
		ID3D11VertexShader* vertexShaderPtr = nullptr;

		HR(m_renderData->m_pDevice->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &vertexShaderPtr));

		//Handle input layout info
		std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayoutDesc;
		//int lastInputSlot = 0;
		int lastSemanticIndex = 0;
		std::string lastSemanticName = "";

		for (unsigned i = 0; i < inputLayout.size(); ++i)
		{
			const InputData& element = inputLayout[i];

			D3D11_INPUT_ELEMENT_DESC elementDesc;
			ZeroMemory(&elementDesc, sizeof(D3D11_INPUT_ELEMENT_DESC));

			//Element name
			elementDesc.SemanticName = element.semanticName.c_str();

			//Calculate byte offset per element in order to remain aligned
			elementDesc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;

			elementDesc.Format = m_renderData->dxgiFormatArrHelper[(int)element.format];

			//Handle if the element is instanced
			if (element.instanceData)
			{
				elementDesc.InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
				elementDesc.InstanceDataStepRate = 1;
				elementDesc.InputSlot = 1;

				if (element.semanticName == lastSemanticName)
				{
					elementDesc.SemanticIndex = ++lastSemanticIndex;
				}
				else
					elementDesc.SemanticIndex = lastSemanticIndex = 0;
			}
			//If not instanced, default values of 0 will be correct 

			lastSemanticName = element.semanticName;
			inputLayoutDesc.push_back(elementDesc);
		} //end for...

		  //Create input layout with the input element descriptions
		ID3D11InputLayout* dxInputLayout;

		HR(m_renderData->m_pDevice->CreateInputLayout(&inputLayoutDesc[0], inputLayoutDesc.size(),
			blob->GetBufferPointer(), blob->GetBufferSize(), &dxInputLayout));

		if (vertexShader)
		{
			//Handle already exists, update data
			VertexShader& vertexShaderObj = m_renderData->vertexShaders[*vertexShader];

			//Safely release current data
			if (vertexShaderObj.layout)
				vertexShaderObj.layout->Release();
			if (vertexShaderObj.vertexShader)
				vertexShaderObj.vertexShader->Release();
			if (vertexShaderObj.layout)
					vertexShaderObj.layout->Release();

			//Assign new data
			vertexShaderObj.layout = dxInputLayout;
			vertexShaderObj.shaderBlob = blob;
			vertexShaderObj.vertexShader = vertexShaderPtr;
		}
		else
		{
			VertexShader vertexShaderObj;

			vertexShaderObj.layout = dxInputLayout;
			vertexShaderObj.shaderBlob = blob;
			vertexShaderObj.vertexShader = vertexShaderPtr;

			//find where in the container the handle can go
			int index = m_renderData->NextAvailableIndex(m_renderData->vertexShaders);

			if (index == -1)
			{
				//No free space in container, push back
				m_renderData->vertexShaders.push_back(vertexShaderObj);
				vertexShader = CreateHandle(ObjectType::VERTEX_SHADER, m_renderData->vertexShaders.size() - 1);
			}
			else
			{
				//Use the free space for the new handle
				m_renderData->vertexShaders[index] = vertexShaderObj;
				vertexShader = CreateHandle(ObjectType::VERTEX_SHADER, index);
			}
		}
	} //end if(succeeded.....
}

void DX11Renderer::CreatePixelShader(ObjectHandle& pixelShader, const std::string& fileName,
	bool precompiled, const std::string& entryPoint /*= "main"*/)
{
	ID3D11PixelShader* pixelShaderPtr;
	int result;

	ID3DBlob* blob;
	const std::string target = "ps_5_0";

	CompileShaderHelper(result, &blob, fileName, target, entryPoint);

	HR(m_renderData->m_pDevice->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &pixelShaderPtr));

	//If handle already exists, update data
	if (pixelShader)
	{
		PixelShader& pixelShaderObj = m_renderData->pixelShaders[*pixelShader];
		if (pixelShaderObj.pixelShader)
		{
			pixelShaderObj.pixelShader->Release();
			pixelShaderObj.shaderBlob->Release();
		}

		pixelShaderObj.pixelShader = pixelShaderPtr;
		pixelShaderObj.shaderBlob = blob;
	}
	//Else create handle
	else
	{
		PixelShader pixelShaderObj;
		pixelShaderObj.pixelShader = pixelShaderPtr;
		pixelShaderObj.shaderBlob = blob;

		int index = m_renderData->NextAvailableIndex(m_renderData->pixelShaders);

		if (index == -1)
		{
			//No free space exists in container, push back
			m_renderData->pixelShaders.push_back(pixelShaderObj);
			pixelShader = CreateHandle(ObjectType::PIXEL_SHADER, m_renderData->pixelShaders.size() - 1);
		}

		else
		{
			//Use available space in container
			m_renderData->pixelShaders[index] = pixelShaderObj;
			pixelShader = CreateHandle(ObjectType::PIXEL_SHADER, index);
		}
	}
}

void DX11Renderer::CreateGeometryShader(ObjectHandle& geometryShader, const std::string& fileName, bool precompiled, const std::string& entryPoint /*= "main"*/)
{
	ID3D11GeometryShader* geometryShaderPtr = nullptr;
	int result;

	ID3DBlob* blob = nullptr;
	const std::string target = "gs_5_0";

	CompileShaderHelper(result, &blob, fileName, target, entryPoint);

	HR(m_renderData->m_pDevice->CreateGeometryShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &geometryShaderPtr));

	//If handle already exists, update data
	if (geometryShader)
	{
		GeometryShader& geometryShaderObj = m_renderData->geometryShaders[*geometryShader];
		if (geometryShaderObj.geometryShader)
		{
			geometryShaderObj.geometryShader->Release();
			geometryShaderObj.shaderBlob->Release();
		}

		geometryShaderObj.geometryShader = geometryShaderPtr;
		geometryShaderObj.shaderBlob = blob;
	}
	//Else create handle
	else
	{
		GeometryShader geometryShaderObj;
		geometryShaderObj.geometryShader = geometryShaderPtr;
		geometryShaderObj.shaderBlob = blob;

		int index = m_renderData->NextAvailableIndex(m_renderData->geometryShaders);

		if (index == -1)
		{
			//No free space exists in container, push back
			m_renderData->geometryShaders.push_back(geometryShaderObj);
			geometryShader = CreateHandle(ObjectType::GEOMETRY_SHADER, m_renderData->geometryShaders.size() - 1);
		}

		else
		{
			//Use available space in container
			m_renderData->geometryShaders[index] = geometryShaderObj;
			geometryShader = CreateHandle(ObjectType::GEOMETRY_SHADER, index);
		}
	}
}

void DX11Renderer::CreateComputeShader(ObjectHandle& computeShader, const std::string& fileName, bool precompiled, const std::string& entryPoint)
{
		ID3D11ComputeShader* computeShaderPtr = nullptr;
		int result;

		ID3DBlob* blob = nullptr;
		const std::string target = "cs_5_0";

		CompileShaderHelper(result, &blob, fileName, target, entryPoint);

		HR(m_renderData->m_pDevice->CreateComputeShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &computeShaderPtr));

		//If handle already exists, update data
		if (computeShader)
		{
				ComputeShader& computeShaderObj = m_renderData->computeShaders[*computeShader];
				if (computeShaderObj.computeShader)
				{
						computeShaderObj.computeShader->Release();
						computeShaderObj.shaderBlob->Release();
				}

				computeShaderObj.computeShader = computeShaderPtr;
				computeShaderObj.shaderBlob = blob;
		}
		//Else create handle
		else
		{
				ComputeShader computeShaderObj;
				computeShaderObj.computeShader = computeShaderPtr;
				computeShaderObj.shaderBlob = blob;

				int index = m_renderData->NextAvailableIndex(m_renderData->computeShaders);

				if (index == -1)
				{
						//No free space exists in container, push back
						m_renderData->computeShaders.push_back(computeShaderObj);
						computeShader = CreateHandle(ObjectType::COMPUTE_SHADER, m_renderData->computeShaders.size() - 1);
				}

				else
				{
						//Use available space in container
						m_renderData->computeShaders[index] = computeShaderObj;
						computeShader = CreateHandle(ObjectType::COMPUTE_SHADER, index);
				}
		}
}

void DX11Renderer::CreateHullShader(ObjectHandle& hullShader, const std::string& fileName, bool precompiled, const std::string& entryPoint /*= "main"*/)
{
	ID3D11HullShader* hullShaderPtr = nullptr;
	int result;

	ID3DBlob* blob = nullptr;
	const std::string target = "hs_5_0";

	CompileShaderHelper(result, &blob, fileName, target, entryPoint);

	HR(m_renderData->m_pDevice->CreateHullShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &hullShaderPtr));

	//If handle already exists, update data
	if (hullShader)
	{
		HullShader& hullShaderObj = m_renderData->hullShaders[*hullShader];
		if (hullShaderObj.hullShader)
		{
			hullShaderObj.hullShader->Release();
			hullShaderObj.shaderBlob->Release();
		}

		hullShaderObj.hullShader = hullShaderPtr;
		hullShaderObj.shaderBlob = blob;
	}
	//Else create handle
	else
	{
		HullShader hullShaderObj;
		hullShaderObj.hullShader = hullShaderPtr;
		hullShaderObj.shaderBlob = blob;

		int index = m_renderData->NextAvailableIndex(m_renderData->hullShaders);

		if (index == -1)
		{
			//No free space exists in container, push back
			m_renderData->hullShaders.push_back(hullShaderObj);
			hullShader = CreateHandle(ObjectType::HULL_SHADER, m_renderData->hullShaders.size() - 1);
		}

		else
		{
			//Use available space in container
			m_renderData->hullShaders[index] = hullShaderObj;
			hullShader = CreateHandle(ObjectType::HULL_SHADER, index);
		}
	}
}

void DX11Renderer::CreateDomainShader(ObjectHandle& domainShader, const std::string& fileName, bool precompiled, const std::string& entryPoint /*= "main"*/)
{
	ID3D11DomainShader* domainShaderPtr = nullptr;
	int result;

	ID3DBlob* blob = nullptr;
	const std::string target = "ds_5_0";

	CompileShaderHelper(result, &blob, fileName, target, entryPoint);

	HR(m_renderData->m_pDevice->CreateDomainShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &domainShaderPtr));

	//If handle already exists, update data
	if (domainShader)
	{
		DomainShader& domainShaderObj = m_renderData->domainShaders[*domainShader];
		if (domainShaderObj.domainShader)
		{
			domainShaderObj.domainShader->Release();
			domainShaderObj.shaderBlob->Release();
		}

		domainShaderObj.domainShader = domainShaderPtr;
		domainShaderObj.shaderBlob = blob;
	}
	//Else create handle
	else
	{
		DomainShader domainShaderObj;
		domainShaderObj.domainShader = domainShaderPtr;
		domainShaderObj.shaderBlob = blob;

		int index = m_renderData->NextAvailableIndex(m_renderData->domainShaders);

		if (index == -1)
		{
			//No free space exists in container, push back
			m_renderData->domainShaders.push_back(domainShaderObj);
			domainShader = CreateHandle(ObjectType::DOMAIN_SHADER, m_renderData->domainShaders.size() - 1);
		}

		else
		{
			//Use available space in container
			m_renderData->domainShaders[index] = domainShaderObj;
			domainShader = CreateHandle(ObjectType::DOMAIN_SHADER, index);
		}
	}
}

void DX11Renderer::BindVertexShader(const ObjectHandle& vertexShader)
{
#if _DEBUG
	assert(vertexShader && vertexShader.GetType() == ObjectType::VERTEX_SHADER);
#endif
	const VertexShader& shader = m_renderData->vertexShaders[*vertexShader];

	//TODO: For now we're only using one input layout, make this work with multiple
	m_renderData->m_pImmediateContext->VSSetShader(shader.vertexShader, nullptr, 0);
	m_renderData->m_pImmediateContext->IASetInputLayout(shader.layout);
}

void DX11Renderer::BindPixelShader(const ObjectHandle& pixelShader)
{
#if _DEBUG
		assert(pixelShader && pixelShader.GetType() == ObjectType::PIXEL_SHADER);
#endif
		const PixelShader& shader = m_renderData->pixelShaders[*pixelShader];
		m_renderData->m_pImmediateContext->PSSetShader(shader.pixelShader, nullptr, 0);
}

void DX11Renderer::BindGeometryShader(const ObjectHandle& geometryShader)
{
#if _DEBUG
	assert(geometryShader && geometryShader.GetType() == ObjectType::GEOMETRY_SHADER);
#endif
	const GeometryShader& shader = m_renderData->geometryShaders[*geometryShader];
	m_renderData->m_pImmediateContext->GSSetShader(shader.geometryShader, nullptr, 0);
}

void DX11Renderer::BindComputeShader(const ObjectHandle& computeShader)
{
#if _DEBUG
		assert(computeShader && computeShader.GetType() == ObjectType::COMPUTE_SHADER);
#endif
		const ComputeShader& shader = m_renderData->computeShaders[*computeShader];
		m_renderData->m_pImmediateContext->CSSetShader(shader.computeShader, nullptr, 0);
}

void DX11Renderer::BindHullShader(const ObjectHandle& hullShader)
{
#if _DEBUG
	assert(hullShader && hullShader.GetType() == ObjectType::HULL_SHADER);
#endif
	const HullShader& shader = m_renderData->hullShaders[*hullShader];
	m_renderData->m_pImmediateContext->HSSetShader(shader.hullShader, nullptr, 0);
}

void DX11Renderer::BindDomainShader(const ObjectHandle& domainShader)
{
#if _DEBUG
	assert(domainShader && domainShader.GetType() == ObjectType::DOMAIN_SHADER);
#endif
	const DomainShader& shader = m_renderData->domainShaders[*domainShader];
	m_renderData->m_pImmediateContext->DSSetShader(shader.domainShader, nullptr, 0);
}

void DX11Renderer::BindTextureShaderResource(const ObjectType shaderType, unsigned int startSlot, unsigned int numViews, const ObjectHandle& objectWithSRV)
{
	if (objectWithSRV)
	{
		const TextureObjectBase* textureObj = nullptr;
		switch (objectWithSRV.GetType())
		{
		case ObjectType::TEXTURE_1D:
				textureObj = &m_renderData->textures1D[*objectWithSRV];
				break;
		case ObjectType::TEXTURE_2D:
				textureObj = &m_renderData->textures2D[*objectWithSRV];
				break;
		case ObjectType::TEXTURE_3D:
				textureObj = &m_renderData->textures3D[*objectWithSRV];
				break;
		case ObjectType::RENDER_TARGET:
				textureObj = &m_renderData->renderTargets[*objectWithSRV];
				break;
		default: //Non-TextureObject object type supplied
				return;
		}

				switch (shaderType)
				{
				case ObjectType::VERTEX_SHADER:
						m_renderData->m_pImmediateContext->VSSetShaderResources(startSlot, numViews, &textureObj->srv);
						break;
				case ObjectType::PIXEL_SHADER:
						m_renderData->m_pImmediateContext->PSSetShaderResources(startSlot, numViews, &textureObj->srv);
						break;
				case ObjectType::GEOMETRY_SHADER:
						m_renderData->m_pImmediateContext->GSSetShaderResources(startSlot, numViews, &textureObj->srv);
						break;
				case ObjectType::COMPUTE_SHADER:
						m_renderData->m_pImmediateContext->CSSetShaderResources(startSlot, numViews, &textureObj->srv);
						break;
				case ObjectType::HULL_SHADER:
						m_renderData->m_pImmediateContext->HSSetShaderResources(startSlot, numViews, &textureObj->srv);
						break;
				case ObjectType::DOMAIN_SHADER:
					m_renderData->m_pImmediateContext->DSSetShaderResources(startSlot, numViews, &textureObj->srv);
					break;

				default: //Non-Shader object type supplied
						break;
				}
		}
}

void DX11Renderer::DispatchComputeShader(const ObjectHandle& computeShader, const UINT threadGroupX, const UINT threadGroupY, const UINT threadGroupZ)
{
#if _DEBUG
		assert(computeShader && computeShader.GetType() == ObjectType::COMPUTE_SHADER);
#endif
		m_renderData->m_pImmediateContext->Dispatch(threadGroupX, threadGroupY, threadGroupZ);
}

void DX11Renderer::CreateTexture2D(ObjectHandle& textureHandle, const std::string& fileName, bool generateMipChain /*= true*/)
{
	const size_t r = fileName.find_last_of('.') + 1;
	const char l = tolower(fileName[r]);
	const auto wFileName = std::wstring(fileName.begin(), fileName.end());
	DirectX::ScratchImage image;
	
	DirectX::TexMetadata metaData;
	metaData.format = DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT;
	metaData.dimension = DirectX::TEX_DIMENSION::TEX_DIMENSION_TEXTURE2D;
	metaData.mipLevels = generateMipChain ? 6 : 1;
	metaData.depth = 1;

	HRESULT result;

	//in case the file is tga / NOTE: DO NOT USE .TIFF TEXTURES!!!
	if (l == 't')
	{
		result = DirectX::LoadFromTGAFile(wFileName.c_str(), &metaData, image);
	}
	//in case the file is dds
	else if (l == 'd')
	{
		result = DirectX::LoadFromDDSFile(wFileName.c_str(), 0, &metaData, image);
	}
	//in case the file is hdr
	else if (l == 'h') 
	{
		result = DirectX::LoadFromHDRFile(wFileName.c_str(), &metaData, image);
	}

	//else use wic
	else
	{
		result = DirectX::LoadFromWICFile(wFileName.c_str(), 0, &metaData, image);
	}

	if (FAILED(result))
		return;

	ID3D11ShaderResourceView* srv;
	result = DirectX::CreateShaderResourceView(m_renderData->m_pDevice, image.GetImages(), image.GetImageCount(), metaData, &srv);


	if (FAILED(result))
		return;

	//Assign newly created texture to the object handle
	if (textureHandle && textureHandle.GetType() == ObjectType::TEXTURE_2D)
	{
		//If the handle already exists, update the data
		Texture2D& texture = m_renderData->textures2D[*textureHandle];
		//texture.size = Area(image.Width, texDesc.Height);

		SafeRelease(texture.srv);
		SafeRelease(texture.texture2D);

		texture.srv = srv;
	}

	else
	{
		//Create the handle and assign the data
		Texture2D texture;
		//texture.size = Area(texDesc.Width, texDesc.Height);
		texture.srv = srv;

		int index = m_renderData->NextAvailableIndex(m_renderData->textures2D);

		//No free space in the texture container
		if (index == -1)
		{
			m_renderData->textures2D.emplace_back(texture);
			textureHandle = CreateHandle(ObjectType::TEXTURE_2D, m_renderData->textures2D.size() - 1);
		}

		//Insert into available slot
		else
		{
			m_renderData->textures2D[index] = texture;
			textureHandle = CreateHandle(ObjectType::TEXTURE_2D, index);
		}
	}
}

void DX11Renderer::CreateTexture2D(ObjectHandle& textureHandle, const int W, const int H, const DataFormat dataFormat, void* initialMemory, 
	bool generateMipChain)
{
	D3D11_TEXTURE2D_DESC textureDesc;
	HRESULT result;
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;

	// Initialize the render target texture description.
	ZeroMemory(&textureDesc, sizeof(textureDesc));

	// Setup the render target texture description.
	textureDesc.Width = W;
	textureDesc.Height = H;
	textureDesc.MipLevels = generateMipChain ? 5 : 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = m_renderData->dxgiFormatArrHelper[(int)dataFormat];
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	ID3D11Texture2D* tempTargetTexture = nullptr;

	// Create the render target texture.
	HR(m_renderData->m_pDevice->CreateTexture2D(&textureDesc, nullptr, &tempTargetTexture));

	// Setup the description of the shader resource view.
	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	ID3D11ShaderResourceView* shaderResourceView;

	HR(m_renderData->m_pDevice->CreateShaderResourceView(tempTargetTexture, &shaderResourceViewDesc, &shaderResourceView));

	//Assign newly created texture to the object handle
	if (textureHandle && textureHandle.GetType() == ObjectType::TEXTURE_2D)
	{
		//If the handle already exists, update the data
		Texture2D& texture = m_renderData->textures2D[*textureHandle];
		//texture.size = Area(image.Width, texDesc.Height);

		SafeRelease(texture.srv);
		SafeRelease(texture.texture2D);

		texture.srv = shaderResourceView;
		texture.texture2D = tempTargetTexture;
	}

	else
	{
		//Create the handle and assign the data
		Texture2D texture;
		texture.width = W; 
		texture.height = H;
		texture.srv = shaderResourceView;
		texture.texture2D = tempTargetTexture;

		int index = m_renderData->NextAvailableIndex(m_renderData->textures2D);

		//No free space in the texture container
		if (index == -1)
		{
			m_renderData->textures2D.emplace_back(texture);
			textureHandle = CreateHandle(ObjectType::TEXTURE_2D, m_renderData->textures2D.size() - 1);
		}

		//Insert into available slot
		else
		{
			m_renderData->textures2D[index] = texture;
			textureHandle = CreateHandle(ObjectType::TEXTURE_2D, index);
		}
	}
}

void DX11Renderer::ReleaseObject(const ObjectHandle& object)
{
	switch (object.GetType())
	{
	case ObjectType::BACK_BUFFER:
		break;
	case ObjectType::DEPTH_BUFFER:
		break;
	case ObjectType::TEXTURE_1D:
		break;
	case ObjectType::TEXTURE_2D:
		break;
	case ObjectType::TEXTURE_3D:
		break;
	case ObjectType::RENDER_TARGET:
		break;
	case ObjectType::SHADER_RESOURCE_VIEW:
		break;
	case ObjectType::VERTEX_SHADER:
		break;
	case ObjectType::PIXEL_SHADER:
		break;
	case ObjectType::GEOMETRY_SHADER:
		break;
	case ObjectType::COMPUTE_SHADER:
		break;
	case ObjectType::VERTEX_BUFFER:
	{
		auto& obj = m_renderData->vertexBuffers[object.GetHandleID()];
		SafeRelease(obj.buffer);
		obj.deleted = true;
		break;
	}
	case ObjectType::INDEX_BUFFER:
		break;
	case ObjectType::INSTANCE_BUFFER:
		break;
	case ObjectType::CONSTANT_BUFFER:
		break;
	case ObjectType::FONT_FACTORY:
		break;
	case ObjectType::FONT_WRAPPER:
		break;
	default:
		break;
	}
}

void DX11Renderer::EnableAlphaBlending()
{
	float blendFactor[] = { 0.f, 0.f, 0.f, 0.f };
	m_renderData->m_pImmediateContext->OMSetBlendState(m_renderData->m_alphaBlending, blendFactor, 0xffffffff);
}

void DX11Renderer::DisableColorBlending()
{
	//Set the default blend state (no blending) for opaque objects
	m_renderData->m_pImmediateContext->OMSetBlendState(nullptr, 0, 0xffffffff);
}

void DX11Renderer::EnableAdditiveBlending()
{
		float blendFactor[] = { 0.f, 0.f, 0.f, 0.f };
		m_renderData->m_pImmediateContext->OMSetBlendState(m_renderData->m_additiveBlending, blendFactor, 0xffffffff);
}

int DX11Renderer::GetRenderTargetWidth() const
{
		return m_renderTargetWidth;
}

int DX11Renderer::GetRenderTargetHeight() const
{
		return m_renderTargetHeight;
}

bool DX11Renderer::InitializeD3D(const int width, const int height, HWND hwnd)
{
	bool result = InitializeSwapChain(width, height, hwnd);
	result &= InitializeRasterizerStates();
	result &= InitializeDepthStates();
	result &= InitializeBlendStates();
	//result &= InitializeTextureSamplers();


	result &= ResizeBuffers(width, height);

	return result;
}

bool DX11Renderer::InitializeSwapChain(const int width, const int height, HWND hwnd)
{
	UINT createDeviceFlags = 0;

#ifdef _DEBUG
	//createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif //_DEBUG

	const D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE, //<- video card,best choice
		D3D_DRIVER_TYPE_WARP, //<- emulated
		D3D_DRIVER_TYPE_REFERENCE
	};

	const UINT numDriverTypes = ARRAYSIZE(driverTypes);

	const D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3
	};

	const UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	//Prepare double buffer description
	DXGI_SWAP_CHAIN_DESC swapDesc;
	ZeroMemory(&swapDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

	swapDesc.BufferCount = 1; // double buffered
	swapDesc.BufferDesc.Width = width;
	swapDesc.BufferDesc.Height = height;
	swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapDesc.OutputWindow = hwnd;
	swapDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapDesc.Windowed = true;
	swapDesc.SampleDesc.Count = 1;
	swapDesc.SampleDesc.Quality = 0;
	swapDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; //alt-enter fullscreen

															 //Loop through the driver types to find the best supported one.
	HRESULT result;
	for (UINT i = 0; i < numDriverTypes; ++i)
	{
		result = D3D11CreateDeviceAndSwapChain(0, driverTypes[i], 0, createDeviceFlags,
			featureLevels, numFeatureLevels, D3D11_SDK_VERSION, &swapDesc, &m_renderData->m_pSwapChain,
			&m_renderData->m_pDevice, &m_renderData->m_FeatureLevel, &m_renderData->m_pImmediateContext);

		if (SUCCEEDED(result)) {
			m_renderData->m_DriverType = driverTypes[i];
			return true;
		}
	}

	OutputDebugString("FAILED TO CREATE DEVICE AND SWAP CHAIN");
	HR(result);//<-- By this point it'll fail for sure
	return false;
}

bool DX11Renderer::InitializeConstBuffers()
{
	//Prepare the view buffer
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(PerObectBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	HR(m_renderData->m_pDevice->CreateBuffer(&bd, NULL, &m_renderData->testViewProjConstBuffer));

	return true;
}

bool DX11Renderer::InitializeRasterizerStates()
{
	// Setup rasterizer states
	D3D11_RASTERIZER_DESC rasterizerDesc;
	ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));

	rasterizerDesc.AntialiasedLineEnable = FALSE;
	rasterizerDesc.CullMode = D3D11_CULL_NONE;
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0.0f;
	rasterizerDesc.DepthClipEnable = TRUE;
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.FrontCounterClockwise = FALSE;
	rasterizerDesc.MultisampleEnable = FALSE;
	rasterizerDesc.ScissorEnable = FALSE;
	rasterizerDesc.SlopeScaledDepthBias = 0.0f;

	// Create the default rasterizer state object.
	m_renderData->m_pDevice->CreateRasterizerState(&rasterizerDesc, &m_renderData->m_d3dRasterStateSolCullNone);

	//Create the back face cull mode
	rasterizerDesc.CullMode = D3D11_CULL_BACK;
	m_renderData->m_pDevice->CreateRasterizerState(&rasterizerDesc, &m_renderData->m_d3dRasterStateSolCullBack);

	//Create the front face cull mode
	rasterizerDesc.CullMode = D3D11_CULL_FRONT;
	m_renderData->m_pDevice->CreateRasterizerState(&rasterizerDesc, &m_renderData->m_d3dRasterStateSolCullFront);

	//Create the wireframe rasterizer state object
	rasterizerDesc.CullMode = D3D11_CULL_NONE;
	rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
	//rasterizerDesc.AntialiasedLineEnable = TRUE;
	m_renderData->m_pDevice->CreateRasterizerState(&rasterizerDesc, &m_renderData->m_d3dRasterStateWireframe);

	D3D11_RASTERIZER_DESC RSDesc;
	memset(&RSDesc, 0, sizeof(D3D11_RASTERIZER_DESC));
	//create the rasterizer state for ImGui
	RSDesc.FillMode = D3D11_FILL_SOLID;
	RSDesc.CullMode = D3D11_CULL_NONE;
	RSDesc.FrontCounterClockwise = FALSE;
	RSDesc.DepthBias = 0;
	RSDesc.SlopeScaledDepthBias = 0.0f;
	RSDesc.DepthBiasClamp = 0;
	RSDesc.DepthClipEnable = TRUE;
	RSDesc.ScissorEnable = TRUE;
	RSDesc.AntialiasedLineEnable = FALSE;
	RSDesc.MultisampleEnable = FALSE; //swapDesc.SampleDesc.Count > 1 ? TRUE : FALSE;
	m_renderData->m_pDevice->CreateRasterizerState(&RSDesc, &m_renderData->m_d3dRasterStateImgui);

	//Create the rasterizer state for the skybox!
	rasterizerDesc.CullMode = D3D11_CULL_NONE;
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.FrontCounterClockwise = TRUE;

	m_renderData->m_pDevice->CreateRasterizerState(&RSDesc, &m_renderData->m_d3dRasterStateSkybox);

	m_renderData->m_pImmediateContext->RSSetState(m_renderData->m_currentRasterState = m_renderData->m_d3dRasterStateSolCullNone);

	return true;
}

bool DX11Renderer::InitializeDepthStates()
{
	D3D11_DEPTH_STENCIL_DESC dssDesc;
	ZeroMemory(&dssDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	dssDesc.DepthEnable = true;
	dssDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dssDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	// Stencil test parameters
	dssDesc.StencilEnable = false;
	dssDesc.StencilReadMask = 0xFF;
	dssDesc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing
	dssDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dssDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	dssDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_ZERO;
	dssDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing
	dssDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_ZERO;
	dssDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	dssDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dssDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	HR(m_renderData->m_pDevice->CreateDepthStencilState(&dssDesc, &m_renderData->m_DSLessEqual));

	return true;
}

bool DX11Renderer::InitializeTestData(const int width, const int height)
{
	using namespace DirectX;

	int hResult;

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.CPUAccessFlags = 0;

	// Create the constant buffers
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ViewProjBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	HR(m_renderData->m_pDevice->CreateBuffer(&bd, NULL, &m_renderData->testViewProjConstBuffer));

	bd.ByteWidth = sizeof(PerObectBuffer);
	HR(m_renderData->m_pDevice->CreateBuffer(&bd, NULL, &m_renderData->testPerObjectConstBuffer));

	bd.ByteWidth = sizeof(AnimationBuffer);
	HR(m_renderData->m_pDevice->CreateBuffer(&bd, NULL, &m_renderData->testAnimationConstBuffer));

	bd.ByteWidth = sizeof(SimpleCloth_ConstBuffer);
	HR(m_renderData->m_pDevice->CreateBuffer(&bd, NULL, &m_renderData->testSimpleClothConstBuffer));

	bd.ByteWidth = sizeof(Light) * s_maxShadowLights;
	HR(m_renderData->m_pDevice->CreateBuffer(&bd, NULL, &m_renderData->testLightWithShadowConstBuffer));

	bd.ByteWidth = sizeof(LightViewProj) * s_maxShadowLights;
	HR(m_renderData->m_pDevice->CreateBuffer(&bd, NULL, &m_renderData->testLightViewConstBuffer));

	bd.ByteWidth = sizeof(MeshEntryMaterial);
	HR(m_renderData->m_pDevice->CreateBuffer(&bd, NULL, &m_renderData->testMeshMaterialConstBuffer));

	bd.ByteWidth = sizeof(SimpleLight);//  * LightComponent::s_maxLights;
	HR(m_renderData->m_pDevice->CreateBuffer(&bd, NULL, &m_renderData->testLightNoShadowConstBuffer));

	bd.ByteWidth = sizeof(GlobalShaderProperties);
	HR(m_renderData->m_pDevice->CreateBuffer(&bd, NULL, &m_renderData->testGlobalShaderPropertiesConstBuffer));

	bd.ByteWidth = sizeof(LightVolumeProperties);
	HR(m_renderData->m_pDevice->CreateBuffer(&bd, NULL, &m_renderData->testLightVolumePropertiesConstBuffer));
	
	// Initialize the world matrices
	//m_renderData->testPerObjectBuffer.worldMtx = XMMatrixScaling(1,1,1) * DirectX::XMMatrixRotationX(XM_PIDIV2) * XMMatrixTranslation(-1, 1, 0);
	//m_renderData->testPerObjectBuffer.worldMtx = XMMatrixTranspose(m_renderData->testPerObjectBuffer.worldMtx);
	m_renderData->testPerObjectBuffer.worldMtx = XMMatrixTranspose(XMMatrixTranslation(0, 0, 0) *  DirectX::XMMatrixRotationX(XM_PIDIV2) * XMMatrixScaling(1,1,1));
	m_renderData->m_pImmediateContext->UpdateSubresource(m_renderData->testPerObjectConstBuffer, 0, NULL, &m_renderData->testPerObjectBuffer, 0, 0);

	// Initialize the view matrix
	const XMVECTOR Eye = XMVectorSet(0.0f, 0.0f, -17.f, 0.0f);
	const XMVECTOR At  = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	const XMVECTOR Up  = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	m_renderData->testViewProjBuffer.viewMtx = XMMatrixTranspose(XMMatrixLookAtLH(Eye, At, Up));

	// Initialize the projection matrix
	m_renderData->testViewProjBuffer.projectionMtx = XMMatrixTranspose(XMMatrixPerspectiveFovLH(XM_PIDIV4, width / (FLOAT)height, 0.01f, 1000.0f));
	m_renderData->m_pImmediateContext->UpdateSubresource(m_renderData->testViewProjConstBuffer, 0, NULL, &m_renderData->testViewProjBuffer, 0, 0);

	// Set primitive topology
	m_renderData->m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return true;
}

bool DX11Renderer::InitializeTextureSamplers()
{
	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));

	// Create a texture sampler state description.
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = 0;// D3D11_FLOAT32_MAX;

	// Create the texture sampler state.
	HR(m_renderData->m_pDevice->CreateSamplerState(&samplerDesc, &m_renderData->m_pWrapSamplerState));

	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;
	HR(m_renderData->m_pDevice->CreateSamplerState(&samplerDesc, &m_renderData->m_pMirrorSamplerState));

	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	HR(m_renderData->m_pDevice->CreateSamplerState(&samplerDesc, &m_renderData->m_pClampSamplerState));

	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	HR(m_renderData->m_pDevice->CreateSamplerState(&samplerDesc, &m_renderData->m_pBorderSamplerState));

	static ID3D11SamplerState* const samplerStates[4] = 
	{
		m_renderData->m_pWrapSamplerState,
		m_renderData->m_pMirrorSamplerState,
		m_renderData->m_pClampSamplerState,
		m_renderData->m_pBorderSamplerState
	};
	m_renderData->m_pImmediateContext->PSSetSamplers(0, 4, samplerStates);

	return true;
}

bool DX11Renderer::InitializeBlendStates()
{

	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(blendDesc));

	D3D11_RENDER_TARGET_BLEND_DESC rtbd;
	ZeroMemory(&rtbd, sizeof(rtbd));

	//Alpha blending
	rtbd.BlendEnable = true;
	rtbd.SrcBlend = D3D11_BLEND_SRC_COLOR;
	rtbd.DestBlend = D3D11_BLEND_BLEND_FACTOR;
	rtbd.BlendOp = D3D11_BLEND_OP_ADD;
	rtbd.SrcBlendAlpha = D3D11_BLEND_ONE;
	rtbd.DestBlendAlpha = D3D11_BLEND_ZERO;
	rtbd.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	rtbd.RenderTargetWriteMask = D3D10_COLOR_WRITE_ENABLE_ALL;

	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.RenderTarget[0] = rtbd;

	HR(m_renderData->m_pDevice->CreateBlendState(&blendDesc, &m_renderData->m_alphaBlending));

	//Additive blending
	ZeroMemory(&blendDesc, sizeof(blendDesc));
	ZeroMemory(&rtbd, sizeof(rtbd));
	rtbd.BlendEnable = true;
	rtbd.SrcBlend = D3D11_BLEND_SRC_ALPHA;
	rtbd.DestBlend = D3D11_BLEND_ONE;
	rtbd.BlendOp = D3D11_BLEND_OP_ADD;
	rtbd.SrcBlendAlpha = D3D11_BLEND_ZERO;
	rtbd.DestBlendAlpha = D3D11_BLEND_ONE;
	rtbd.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	rtbd.RenderTargetWriteMask = D3D10_COLOR_WRITE_ENABLE_ALL;

	blendDesc.RenderTarget[0] = rtbd;
	HR(m_renderData->m_pDevice->CreateBlendState(&blendDesc, &m_renderData->m_additiveBlending));

	return true;
}

bool DX11Renderer::ResizeBuffers(const int width, const int height)
{
	if (!m_renderData->m_pImmediateContext)
		return false;

	m_renderTargetWidth = width;
	m_renderTargetHeight = height;

	m_renderData->m_pImmediateContext->OMSetRenderTargets(0, nullptr, nullptr);
	m_renderData->m_pImmediateContext->OMSetDepthStencilState(nullptr, 0);

	//////////////////////////////////////////////////////////////////////////
	// Resize main back buffer
	SafeRelease(m_renderData->m_pBackBufferRenderTargetView);
	HR(m_renderData->m_pSwapChain->ResizeBuffers(1, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0));

	// Get buffer and create a render-target-view.
	ID3D11Texture2D* pBuffer;
	HR(m_renderData->m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),(void**)&pBuffer));
	// Perform error handling here!

	HR(m_renderData->m_pDevice->CreateRenderTargetView(pBuffer, nullptr, &(m_renderData->m_pBackBufferRenderTargetView)));
	// Perform error handling here!
	pBuffer->Release();

	//CREATE DEPTH STENCIL BUFFER, BUT ALSO FOR READING AS AN SRV
	D3D11_TEXTURE2D_DESC depthStencilDesc;
	depthStencilDesc.Width = width;
	depthStencilDesc.Height = height;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_R32_TYPELESS;//DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	SafeRelease(m_renderData->m_DepthStencilBuffer);
	HR(m_renderData->m_pDevice->CreateTexture2D(&depthStencilDesc, 0, &m_renderData->m_DepthStencilBuffer));

	SafeRelease(m_renderData->m_DepthStencilView);
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
	ZeroMemory(&dsvd, sizeof(dsvd));

	dsvd.Format = DXGI_FORMAT_D32_FLOAT;
	dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

	HR(m_renderData->m_pDevice->CreateDepthStencilView(m_renderData->m_DepthStencilBuffer, &dsvd, &m_renderData->m_DepthStencilView));

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	
	// Setup the description of the shader resource view.
	shaderResourceViewDesc.Format = DXGI_FORMAT_R32_FLOAT;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;
	m_renderData->m_pDevice->CreateShaderResourceView(m_renderData->m_DepthStencilBuffer, &shaderResourceViewDesc, 
		&m_renderData->m_DepthStencilShaderResourceView);

	//////////////////////////////////////////////////////////////////////////
	// Resize deferred render targets
	
	//Release all of the RTVs and re-create the render targets
	for (unsigned char i = 0; i < (unsigned char)DX11RendererData::GBufferRTType::COUNT; ++i)
	{
		auto& handle = m_renderData->m_GBufferObjHandles[i];
		CreateRenderTarget(handle, width, height, DataFormat::FLOAT4, true);
		m_renderData->m_pGbufferRTVs[i] = m_renderData->renderTargets[*(handle)].rtv;
	}

	//Create the 2 main render targets
	for (char i = 0; i < 2; ++i)
	{
		CreateRenderTarget(m_renderData->m_MainRenderTargets[i], width, height, DataFormat::FLOAT4, true);
	}

	//Create the RTs for SSAO
	CreateRenderTarget(m_renderData->m_AmbientOccMapRT, width , height, DataFormat::FLOAT1, false);
	CreateRenderTarget(m_renderData->AOtempBlurTexture, width, height, DataFormat::FLOAT1, false);

	//Create the Bloom render target, and the temp texture for blurring
	CreateRenderTarget(m_renderData->m_BloomBrightMap, width, height, DataFormat::FLOAT4, false);

	//BIND RENDER TARGET VIEW
	m_renderData->m_pImmediateContext->OMSetRenderTargets(1, &m_renderData->m_pBackBufferRenderTargetView, m_renderData->m_DepthStencilView);

	// Set up the viewport.
	m_renderData->m_mainViewport.Width = width;
	m_renderData->m_mainViewport.Height = height;
	m_renderData->m_mainViewport.TopLeftX = m_renderData->m_mainViewport.TopLeftY = 0.f;
	m_renderData->m_mainViewport.MinDepth = 0.0f;
	m_renderData->m_mainViewport.MaxDepth = 1.0f;

	m_renderData->m_pImmediateContext->RSSetViewports(1, &m_renderData->m_mainViewport);

	return true;
}

ObjectHandle DX11Renderer::CreateHandle(const ObjectType type, const int handle) const
{
	ObjectHandle newHandle;
	newHandle.SetType(type);
	newHandle.SetHandleID(handle);

	return newHandle;
}

void DX11Renderer::CompileShaderHelper(int& HResult, ID3D10Blob** blobPtrOut, const std::string& fileName,
	const std::string& target, const std::string& szEntryPoint) const
{
#ifdef _DEBUG
	// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
	// Setting this flag improves the shader debugging experience, but still allows 
	// the shaders to be optimized and to run exactly the way they will run in 
	// the release configuration of this program.
	//dwShaderFlags |= D3DCOMPILE_DEBUG;

	// Disable optimizations to further improve shader debugging
	//dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;

	static const DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	static const DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#endif

	ID3D10Blob* pErrorBlob = nullptr;
	const std::wstring _file(fileName.begin(), fileName.end());

	const HRESULT hr = D3DCompileFromFile(_file.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE/*nullptr*/,
		szEntryPoint.c_str(), target.c_str(), dwShaderFlags, 0, blobPtrOut, &pErrorBlob);

	if (FAILED(hr))
	{
		if (pErrorBlob)
		{
			char* compileErrors;
			unsigned long bufferSize, i;
			std::ofstream fout;

			// Get a pointer to the error message text buffer.
			compileErrors = (char*)(pErrorBlob->GetBufferPointer());

			// Get the length of the message.
			bufferSize = pErrorBlob->GetBufferSize();

			// Open a file to write the error message to.
			fout.open("shader-error.txt");

			// Write out the error message.
			for (i = 0; i < bufferSize; i++)
			{
				fout << compileErrors[i];
			}

			// Close the file.
			fout.close();

			OutputDebugStringA(reinterpret_cast<const char*>(pErrorBlob->GetBufferPointer()));
			std::cout << reinterpret_cast<const char*>(pErrorBlob->GetBufferPointer());
			pErrorBlob->Release();
		}

		HResult = hr;

		return;
	}

	if (pErrorBlob)
		pErrorBlob->Release();

	HResult = hr;
}

void DX11Renderer::UpdateGlobalProperties()
{
	m_renderData->m_pImmediateContext->UpdateSubresource(m_renderData->testGlobalShaderPropertiesConstBuffer,
		0, NULL, &m_renderData->testGlobalShaderProperties, 0, 0);
}
