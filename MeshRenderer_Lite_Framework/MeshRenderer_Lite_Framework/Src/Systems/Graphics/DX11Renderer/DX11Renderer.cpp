#include <Utilities/precompiled.h>
#include <Systems/Graphics/DX11Renderer/DX11Renderer.h>

#include <Systems/Graphics/DX11Renderer/DX11RendererData.h>

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
	SafeRelease(m_renderData->m_d3dRasterStateDefault);
	SafeRelease(m_renderData->m_d3dRasterStateSolCullBack);
	SafeRelease(m_renderData->m_d3dRasterStateSolCullFront);
	SafeRelease(m_renderData->m_d3dRasterStateWireframe);
	SafeRelease(m_renderData->m_d3dRasterStateImgui);

	//CLEANUP DIRECT3D
	if (m_renderData->m_pImmediateContext)
		m_renderData->m_pImmediateContext->ClearState();

	SafeRelease(m_renderData->m_pMainRenderTargetView);
	SafeRelease(m_renderData->m_pSwapChain);
	SafeRelease(m_renderData->m_pImmediateContext);
	SafeRelease(m_renderData->m_pDevice);
}

void DX11Renderer::ClearBuffer(void)
{
	m_renderData->m_pImmediateContext->ClearRenderTargetView(m_renderData->m_pMainRenderTargetView, m_renderData->m_clearColor);
	m_renderData->m_pImmediateContext->ClearDepthStencilView(m_renderData->m_DepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);
}

void DX11Renderer::SwapBuffers(void)
{
	HR(m_renderData->m_pSwapChain->Present(0, 0));
}

void DX11Renderer::Draw(unsigned vertexCount, unsigned startVertexLocation /*= 0*/)
{
	m_renderData->m_pImmediateContext->Draw(vertexCount, startVertexLocation);
}

void DX11Renderer::DrawIndexed(unsigned indexCount, unsigned startIndexLocation /*= 0*/, unsigned baseVertexLocation /*= 0*/)
{
	m_renderData->m_pImmediateContext->DrawIndexed(indexCount, startIndexLocation, baseVertexLocation);
}

void DX11Renderer::DrawInstanced(unsigned vertexCount, unsigned instanceCount, unsigned startVertexLocation /*= 0*/, unsigned startInstanceLocation /*= 0*/)
{
	m_renderData->m_pImmediateContext->DrawInstanced(vertexCount, instanceCount, startVertexLocation, startInstanceLocation);
}

void DX11Renderer::DrawIndexedInstanced(unsigned indexCountPerInstance, unsigned instanceCount, unsigned startIndexLocation /*= 0*/, unsigned baseVertexLocation /*= 0*/, unsigned startInstanceLocation /*= 0*/)
{
	m_renderData->m_pImmediateContext->DrawIndexedInstanced(indexCountPerInstance, instanceCount, startIndexLocation, baseVertexLocation, startInstanceLocation);
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

bool DX11Renderer::InitializeTestData(const int width, const int height)
{
	using namespace DirectX;

	int hResult;
	ID3D10Blob* blobVS, *blobPS;
	CompileShaderHelper(hResult, &blobVS, "../MeshRenderer_Lite_Framework/Assets/Shaders/VertexShaders/testVS.hlsl", "vs_5_0", "main");
	HR(m_renderData->m_pDevice->CreateVertexShader(blobVS->GetBufferPointer(), blobVS->GetBufferSize(), NULL, &m_renderData->testVertexShader));

	// Define the input layout
	D3D11_INPUT_ELEMENT_DESC layout[] 
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT numElements = ARRAYSIZE(layout);

	// Create the input layout
	HR(m_renderData->m_pDevice->CreateInputLayout(layout, numElements, blobVS->GetBufferPointer(),
		blobVS->GetBufferSize(), &m_renderData->m_pVSInputLayoutVertexWire));
	blobVS->Release();

	// Set the input layout
	m_renderData->m_pImmediateContext->IASetInputLayout(m_renderData->m_pVSInputLayoutVertexWire);

	CompileShaderHelper(hResult, &blobPS, "../MeshRenderer_Lite_Framework/Assets/Shaders/PixelShaders/testPS.hlsl", "ps_5_0", "main");
	HR(m_renderData->m_pDevice->CreatePixelShader(blobPS->GetBufferPointer(), blobPS->GetBufferSize(), NULL, &m_renderData->testPixelShader));
	blobPS->Release();

	// Create vertex buffer
	const VertexWire vertices[] =
	{
		{XMFLOAT3(0.0f, 0.25f, 0)  , XMFLOAT4(0,1,0,1)},
		{XMFLOAT3(0.25f, 0, 0) , XMFLOAT4(0,1,0,1)},
		{XMFLOAT3(-0.25f, 0, 0), XMFLOAT4(0,1,0,1)}
	};

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(VertexWire) * 3;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));

	// Set vertex buffer
	UINT stride = sizeof(VertexWire);
	UINT offset = 0;
	m_renderData->m_pImmediateContext->IASetVertexBuffers(0, 1, &m_renderData->testVertBuffer, &stride, &offset);

	// Set primitive topology
	m_renderData->m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Create the constant buffers
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(CBNeverChanges);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	HR(m_renderData->m_pDevice->CreateBuffer(&bd, NULL, &m_renderData->testViewProjConstBuffer));


	bd.ByteWidth = sizeof(CBChangesEveryFrame);
	HR(m_renderData->m_pDevice->CreateBuffer(&bd, NULL, &m_renderData->testPerObjectConstBuffer));

	// Initialize the world matrices
	testPerObjectBuffer.worldMtx = DirectX::XMMatrixScaling(0.20, 0.20, 0.23) * DirectX::XMMatrixRotationZ(XM_PIDIV4) * DirectX::XMMatrixTranslation(-2.0, -1, 0);
	testPerObjectBuffer.worldMtx = XMMatrixTranspose(testPerObjectBuffer.worldMtx);
	m_renderData->m_pImmediateContext->UpdateSubresource(m_renderData->testPerObjectConstBuffer, 0, NULL, &testPerObjectBuffer, 0, 0);

	// Initialize the view matrix
	DirectX::XMVECTOR Eye = DirectX::XMVectorSet(0.0f, 0.0f, -20.0f, 0.0f);
	DirectX::XMVECTOR At  = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	DirectX::XMVECTOR Up  = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	testViewProjBuffer.viewMtx = DirectX::XMMatrixTranspose(DirectX::XMMatrixLookAtLH(Eye, At, Up));

	// Initialize the projection matrix
	testViewProjBuffer.projectionMtx = XMMatrixTranspose(DirectX::XMMatrixPerspectiveFovLH(XM_PIDIV4, width / (FLOAT)height, 0.01f, 1000.0f));
	m_renderData->m_pImmediateContext->UpdateSubresource(m_renderData->testViewProjConstBuffer, 0, NULL, &testViewProjBuffer, 0, 0);

	// prepare the triangle
	m_renderData->m_pImmediateContext->VSSetShader(m_renderData->testVertexShader, NULL, 0);
	m_renderData->m_pImmediateContext->VSSetConstantBuffers(0, 1, &m_renderData->testPerObjectConstBuffer);
	m_renderData->m_pImmediateContext->VSSetConstantBuffers(1, 1, &m_renderData->testViewProjConstBuffer);
	m_renderData->m_pImmediateContext->PSSetShader(m_renderData->testPixelShader, NULL, 0);

	return true;
}

bool DX11Renderer::InitializeTextureSamplers()
{
	return true;
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