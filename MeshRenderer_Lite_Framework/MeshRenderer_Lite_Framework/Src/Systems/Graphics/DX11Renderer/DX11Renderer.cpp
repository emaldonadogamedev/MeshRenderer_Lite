#include <Utilities/precompiled.h>
#include <Systems/Graphics/DX11Renderer/DX11Renderer.h>

#include <Systems/Graphics/DX11Renderer/DX11RendererData.h>

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

	return m_isInitialized = true;
}

void DX11Renderer::ReleaseData()
{

}

bool DX11Renderer::InitializeD3D(const int width, const int height, HWND hwnd)
{
	UINT createDeviceFlags = 0;

#ifdef _DEBUG
	//createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif //_DEBUG

	D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE, //<- video card,best choice
		D3D_DRIVER_TYPE_WARP, //<- emulated
		D3D_DRIVER_TYPE_REFERENCE
	};

	UINT numDriverTypes = ARRAYSIZE(driverTypes);

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3
	};

	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

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

	HR(m_renderData->m_pDevice->CreateRenderTargetView(pBackBufferTex, NULL, &m_renderData->m_pRenderTargetView));
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
	m_renderData->m_pImmediateContext->OMSetRenderTargets(1, &m_renderData->m_pRenderTargetView, m_renderData->m_DepthStencilView);

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

bool DX11Renderer::InitializeTextureSamplers()
{
	return true;
}
