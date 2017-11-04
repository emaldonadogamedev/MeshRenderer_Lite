#include <Utilities/precompiled.h>
#include <Systems/Graphics/DX11Renderer/DX11Renderer.h>

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
	//CLEANUP DIRECT3D
	SafeRelease(m_renderData->m_d3dRasterStateDefault);
	SafeRelease(m_renderData->m_d3dRasterStateSolCullBack);
	SafeRelease(m_renderData->m_d3dRasterStateSolCullFront);
	SafeRelease(m_renderData->m_d3dRasterStateWireframe);
	SafeRelease(m_renderData->m_d3dRasterStateImgui);

	if (m_renderData->m_pImmediateContext)
		m_renderData->m_pImmediateContext->ClearState();

	SafeRelease(m_renderData->m_pMainRenderTargetView);
	SafeRelease(m_renderData->m_pSwapChain);
	SafeRelease(m_renderData->m_pImmediateContext);
	SafeRelease(m_renderData->m_pDevice);
}

void DX11Renderer::ClearBuffer(void)
{
	m_renderData->m_pImmediateContext->ClearRenderTargetView(m_renderData->m_pMainRenderTargetView, m_renderData->m_clearColor.m128_f32);
	m_renderData->m_pImmediateContext->ClearDepthStencilView(m_renderData->m_DepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);
}

void DX11Renderer::SwapBuffers(void)
{
	HR(m_renderData->m_pSwapChain->Present(0, 0));
}

const DX11RendererData& DX11Renderer::GetRendererData() const
{
	return *m_renderData;
}

void DX11Renderer::Draw(unsigned vertexCount, unsigned startVertexLocation)
{
	m_renderData->m_pImmediateContext->Draw(vertexCount, startVertexLocation);
}

void DX11Renderer::DrawIndexed(unsigned indexCount, unsigned startIndexLocation, unsigned baseVertexLocation)
{
	m_renderData->m_pImmediateContext->DrawIndexed(indexCount, startIndexLocation, baseVertexLocation);
}

void DX11Renderer::DrawInstanced(unsigned vertexCount, unsigned instanceCount, unsigned startVertexLocation, unsigned startInstanceLocation)
{
	m_renderData->m_pImmediateContext->DrawInstanced(vertexCount, instanceCount, startVertexLocation, startInstanceLocation);
}

void DX11Renderer::DrawIndexedInstanced(unsigned indexCountPerInstance, unsigned instanceCount, unsigned startIndexLocation, unsigned baseVertexLocation, unsigned startInstanceLocation)
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

	default: break;
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

void DX11Renderer::CreateConstantBuffer(ObjectHandle& constantBuffer, unsigned size)
{
	//Create zeroed out buffer description
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));

	//Setup description
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	//Need to ensure that memory is aligned to 16 bytes.
	desc.ByteWidth = size;

	//Create Buffer
	ID3D11Buffer* buffer;

	HR(m_renderData->m_pDevice->CreateBuffer(&desc, nullptr, &buffer));

	if (constantBuffer)
	{
		Buffer& constBufferObj = m_renderData->constantBuffers[*constantBuffer];

		//Free buffer if it has data
		if (constBufferObj.buffer)
			constBufferObj.buffer->Release();

		//Update data
		constBufferObj.buffer = buffer;
		constBufferObj.usage = BufferUsage::USAGE_DYNAMIC;
		constBufferObj.size = size;
	}

	else
	{
		Buffer constBufferObj;

		//Update data
		constBufferObj.buffer = buffer;
		constBufferObj.usage = BufferUsage::USAGE_DYNAMIC;
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

	unsigned compileFlags = D3DCOMPILE_ENABLE_STRICTNESS;

#ifdef _DEBUG
	compileFlags |= D3DCOMPILE_DEBUG;
#endif

	CompileShaderHelper(result, &blob, fileName, target, entryPoint);

	HR(m_renderData->m_pDevice->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &pixelShaderPtr));

	//If handle already exists, update data
	if (pixelShader)
	{
		PixelShader& pixelShaderObj = m_renderData->pixelShaders[*pixelShader];
		if (pixelShaderObj.pixelShader)
			pixelShaderObj.pixelShader->Release();

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

void DX11Renderer::BindVertexShader(const ObjectHandle& vertexShader)
{
	//TODO: Make this check work
	//if (!vertexShader || m_renderData->lastVertexShader == vertexShader || vertexShader.GetType() != ObjectType::VERTEX_SHADER)
	//{
	//	return;
	//}

	const VertexShader& shader = m_renderData->vertexShaders[*vertexShader];

	//TODO: For now we're only using one input layout, make this work with multiple
	m_renderData->m_pImmediateContext->VSSetShader(shader.vertexShader, nullptr, 0);
	m_renderData->m_pImmediateContext->IASetInputLayout(shader.layout);
}

void DX11Renderer::BindPixelShader(const ObjectHandle& pixelShader)
{
	const PixelShader& shader = m_renderData->pixelShaders[*pixelShader];
	m_renderData->m_pImmediateContext->PSSetShader(shader.pixelShader, nullptr, 0);
}

bool DX11Renderer::InitializeD3D(const int width, const int height, HWND hwnd)
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

		if (SUCCEEDED(result))
		{
			m_renderData->m_DriverType = driverTypes[i];

			//OH SNAP! AN ACTUAL REASON TO USE A GOTO!!!
			goto continue_Init;
		}
	}

	OutputDebugString("FAILED TO CREATE DEVICE AND SWAP CHAIN");
	HR(result);//<-- By this point it'll fail for sure
	return false;

continue_Init:

	//CREATE RENDER TARGET VIEW
	ID3D11Texture2D* pBackBufferTex = 0;
	HR(m_renderData->m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pBackBufferTex)));

	HR(m_renderData->m_pDevice->CreateRenderTargetView(pBackBufferTex, NULL, &m_renderData->m_pMainRenderTargetView));
	SafeRelease(pBackBufferTex);

	//CREATE DEPTH STENCIL BUFFER
	D3D11_TEXTURE2D_DESC depthStencilDesc;
	depthStencilDesc.Width = width;
	depthStencilDesc.Height = height;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	HR(result = m_renderData->m_pDevice->CreateTexture2D(&depthStencilDesc, 0, &m_renderData->m_DepthStencilBuffer));

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
	ZeroMemory(&dsvd, sizeof(dsvd));

	dsvd.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

	HR(result = m_renderData->m_pDevice->CreateDepthStencilView(m_renderData->m_DepthStencilBuffer, &dsvd, &m_renderData->m_DepthStencilView));

	//BIND RENDER TARGET VIEW
	m_renderData->m_pImmediateContext->OMSetRenderTargets(1, &m_renderData->m_pMainRenderTargetView, m_renderData->m_DepthStencilView);

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
	m_renderData->m_pDevice->CreateRasterizerState(&rasterizerDesc, &m_renderData->m_d3dRasterStateDefault);

	//Create the back face cull mode
	rasterizerDesc.CullMode = D3D11_CULL_BACK;
	m_renderData->m_pDevice->CreateRasterizerState(&rasterizerDesc, &m_renderData->m_d3dRasterStateSolCullBack);

	//Create the front face cull mode
	rasterizerDesc.CullMode = D3D11_CULL_FRONT;
	m_renderData->m_pDevice->CreateRasterizerState(&rasterizerDesc, &m_renderData->m_d3dRasterStateSolCullFront);

	//Create the wireframe rasterizer state object
	rasterizerDesc.CullMode = D3D11_CULL_NONE;
	rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
	rasterizerDesc.AntialiasedLineEnable = TRUE;
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
	RSDesc.MultisampleEnable = swapDesc.SampleDesc.Count > 1 ? TRUE : FALSE;
	m_renderData->m_pDevice->CreateRasterizerState(&RSDesc, &m_renderData->m_d3dRasterStateImgui);

	m_renderData->m_pImmediateContext->RSSetState(m_renderData->m_d3dRasterStateDefault);

	//VIEWPORT CREATION
	m_renderData->m_mainViewport.Width = static_cast<float>(width);
	m_renderData->m_mainViewport.Height = static_cast<float>(height);
	m_renderData->m_mainViewport.TopLeftX = m_renderData->m_mainViewport.TopLeftY = 0;
	m_renderData->m_mainViewport.MinDepth = 0.0f;
	m_renderData->m_mainViewport.MaxDepth = 1.0f;

	//Bind viewport
	m_renderData->m_pImmediateContext->RSSetViewports(1, &m_renderData->m_mainViewport);

	return true;
}

bool DX11Renderer::InitializeConstBuffers()
{
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(PerObectBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	HR(m_renderData->m_pDevice->CreateBuffer(&bd, NULL, &m_renderData->testViewProjConstBuffer));

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

	// Initialize the world matrices
	m_renderData->testPerObjectBuffer.worldMtx = XMMatrixScaling(1,1,1) * DirectX::XMMatrixRotationX(XM_PIDIV2) * XMMatrixTranslation(0, 0, 0);
	m_renderData->testPerObjectBuffer.worldMtx = XMMatrixTranspose(m_renderData->testPerObjectBuffer.worldMtx);
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
	return true;
}

ObjectHandle DX11Renderer::CreateHandle(const ObjectType type, const int handle)
{
	ObjectHandle newHandle;
	newHandle.SetType(type);
	newHandle.SetHandleID(handle);

	return newHandle;
}

void DX11Renderer::CompileShaderHelper(int& HResult, ID3D10Blob** blobPtrOut, const std::string& fileName,
	const std::string& target, const std::string& szEntryPoint)
{
	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
	// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
	// Setting this flag improves the shader debugging experience, but still allows 
	// the shaders to be optimized and to run exactly the way they will run in 
	// the release configuration of this program.
	dwShaderFlags |= D3DCOMPILE_DEBUG;

	// Disable optimizations to further improve shader debugging
	dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
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