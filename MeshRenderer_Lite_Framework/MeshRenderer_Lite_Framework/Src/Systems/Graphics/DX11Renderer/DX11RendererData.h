#pragma once

#include <Systems/Graphics/GraphicsUtilities/D3DObjects.h>
#include <Systems/Graphics/BufferClasses/ConstantBuffers.h>
#include <Systems/Graphics/CameraClasses/Camera.h>
#include <Systems/Graphics/ModelClasses/MeshMaterial/MeshEntryMaterial.h>

class GraphicsSystem;

class DX11RendererData
{
public:
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pImmediateContext = nullptr;
	IDXGISwapChain* m_pSwapChain = nullptr;

	//Pointer to the main render target stored on the GPU
	ID3D11RenderTargetView* m_pBackBufferRenderTargetView = nullptr;

	//deferred rendering render targets
	enum class GBufferRTType : byte
	{
		POSITION,
		NORMALS,
		DIFFUSE,
		SPECULAR_AND_NS,

		COUNT
	};

	ObjectHandle m_GBufferObjHandles[(int)GBufferRTType::COUNT] = { };
	ID3D11RenderTargetView* m_pGbufferRTVs[(int)GBufferRTType::COUNT] = { nullptr };

	ObjectHandle m_MainRenderTargets[2] = {};
	bool m_currentMainRTindex = false;

	ObjectHandle m_AmbientOccRT;

	ObjectHandle m_BloomBrightMap;

	ID3D11Texture2D* m_DepthStencilBuffer = nullptr;
	ID3D11DepthStencilView* m_DepthStencilView = nullptr;
	ID3D11ShaderResourceView* m_DepthStencilShaderResourceView = nullptr;

	D3D_DRIVER_TYPE m_DriverType;
	D3D_FEATURE_LEVEL m_FeatureLevel;
	D3D_PRIMITIVE_TOPOLOGY m_currentPrimitiveTopology;
	D3D11_VIEWPORT m_mainViewport;

	//rasterizer states
	ID3D11RasterizerState* m_currentRasterState = nullptr;
	ID3D11RasterizerState* m_d3dRasterStateSolCullNone = nullptr;
	ID3D11RasterizerState* m_d3dRasterStateSolCullBack = nullptr;
	ID3D11RasterizerState* m_d3dRasterStateSolCullFront = nullptr;
	ID3D11RasterizerState* m_d3dRasterStateWireframe = nullptr;
	ID3D11RasterizerState* m_d3dRasterStateImgui = nullptr;
	ID3D11RasterizerState* m_d3dRasterStateSkybox = nullptr;
	
	ID3D11InputLayout* m_pVSInputLayoutVertexWire = nullptr;
	ID3D11InputLayout* m_pVSInputLayoutVertexNormal = nullptr;
	ID3D11InputLayout* m_pVSInputLayoutVertexTexture = nullptr;
	ID3D11InputLayout* m_pVSInputLayoutVertexAnimation = nullptr;

	//Blend states
	ID3D11BlendState* m_alphaBlending = nullptr;
	ID3D11BlendState* m_additiveBlending = nullptr;

	//Depth-Stencil states
	ID3D11DepthStencilState* m_DSLessEqual = nullptr;

	//Resource containers
	std::vector<Texture1D> textures1D;
	std::vector<Texture2D> textures2D;
	std::vector<RenderTarget> renderTargets;
	std::vector<DepthBuffer> depthBuffers;
	std::vector<Texture3D> textures3D;
	std::vector<VertexShader> vertexShaders;
	std::vector<PixelShader> pixelShaders;
	std::vector<GeometryShader> geometryShaders;
	std::vector<ComputeShader> computeShaders;
	std::vector<HullShader> hullShaders;
	std::vector<DomainShader> domainShaders;
	std::vector<Buffer> vertexBuffers;
	std::vector<Buffer> indexBuffers;
	std::vector<Buffer> constantBuffers;
	std::vector<StructuredBuffer> structuredBuffers;
	std::vector<StructuredBufferRW> structuredBuffersRW;
	std::vector<StructuredBufferRW_Texture2D> structuredBuffersRW_Texture2D;

	//Sampler states for textures
	ID3D11SamplerState* m_pWrapSamplerState = nullptr;
	ID3D11SamplerState* m_pMirrorSamplerState = nullptr;
	ID3D11SamplerState* m_pClampSamplerState = nullptr;
	ID3D11SamplerState* m_pBorderSamplerState = nullptr;

	//////////////////////////////////////////////////////////////////////////
	// TEST RESOURCES!
	PerObectBuffer testPerObjectBuffer;
	ID3D11Buffer* testPerObjectConstBuffer = nullptr;

	ViewProjBuffer testViewProjBuffer;
	ID3D11Buffer* testViewProjConstBuffer = nullptr;

	AnimationBuffer testAnimationBuffer;
	ID3D11Buffer* testAnimationConstBuffer = nullptr;

	SimpleCloth_ConstBuffer testSimpleClothBuffer;
	ID3D11Buffer* testSimpleClothConstBuffer = nullptr;

	ID3D11Buffer* testLightWithShadowConstBuffer = nullptr;

	ID3D11Buffer* testLightViewConstBuffer = nullptr;

	MeshEntryMaterial testMeshMaterialBuffer = MeshEntryMaterial::GetPresetMaterial(PredefinedMaterials::GreenPlastic);
	ID3D11Buffer* testMeshMaterialConstBuffer = nullptr;

	ID3D11Buffer* testLightNoShadowConstBuffer = nullptr;

	GlobalShaderProperties testGlobalShaderProperties;
	ID3D11Buffer* testGlobalShaderPropertiesConstBuffer = nullptr;
	
	LightVolumeProperties testLightVolumeProperties;
	ID3D11Buffer* testLightVolumePropertiesConstBuffer = nullptr;

	ObjectHandle iblSamplesHandle;
	int iblSampleCount = 0;

	ObjectHandle AOblurSampleBuffer;

	bool m_isUsingBloom = false;

	//Cameras
	std::unique_ptr<Camera> testCamera = std::make_unique<Camera>();

private:
	template<typename Container>
	int NextAvailableIndex(const Container& container)
	{
		//Iterate through container
		for (unsigned i = 0; i < container.size(); ++i)
		{
			//If the element is a free and already deleted handle,
			//return it's index
			if (container[i].deleted)
				return i;
		}

		//No free spaces
		return -1;
	}

	const DXGI_FORMAT dxgiFormatArrHelper[(int)DataFormat::COUNT] =
	{
		DXGI_FORMAT_UNKNOWN,
		DXGI_FORMAT_R8_UNORM,
		DXGI_FORMAT_R8G8_UNORM,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_FORMAT_B8G8R8A8_UNORM,
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		DXGI_FORMAT_R32G32B32_FLOAT,
		DXGI_FORMAT_R32G32_FLOAT,
		DXGI_FORMAT_R32_FLOAT,
		DXGI_FORMAT_R32_UINT,
		DXGI_FORMAT_R32G32_UINT,
		DXGI_FORMAT_R32G32B32_UINT,
		DXGI_FORMAT_R32G32B32A32_UINT,
		DXGI_FORMAT_R32_SINT,
		DXGI_FORMAT_R32G32_SINT,
		DXGI_FORMAT_R32G32B32_SINT,
		DXGI_FORMAT_R32G32B32A32_SINT
	};

	friend GraphicsSystem;
	friend DX11Renderer;
};