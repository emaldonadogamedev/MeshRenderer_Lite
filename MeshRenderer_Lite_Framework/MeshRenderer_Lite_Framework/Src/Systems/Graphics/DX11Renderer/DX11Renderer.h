#pragma once

//TODO: REMOVE AFTER TESTING
#include <DirectXMath.h>
#include<memory>
#include<Systems/Graphics/GraphicsUtilities/ObjectHandle.h>
#include <Systems/Graphics/GraphicsUtilities/InputData.h>

using DirectX::XMMATRIX;
using DirectX::XMVECTOR;

class GraphicsSystem;
class DX11RendererData;

enum class ObjectType : char;

class DX11Renderer
{
public:
	DX11Renderer();
	~DX11Renderer();

	bool InitializeRenderer(const int width, const int height, HWND hwnd);
	void ReleaseData();

	void ClearMainBuffer() const;
	void SwapBuffers(void) const;

	DX11RendererData& GetRendererData();

	//////////////////////////////////////////////////////////////////////////
	//Drawing Functions
	void Draw(unsigned vertexCount, unsigned startVertexLocation = 0) const;
	void DrawIndexed(unsigned indexCount, unsigned startIndexLocation = 0, unsigned baseVertexLocation = 0) const;
	void DrawInstanced(unsigned vertexCount, unsigned instanceCount, unsigned startVertexLocation = 0,
		unsigned startInstanceLocation = 0) const;
	void DrawIndexedInstanced(unsigned indexCountPerInstance, unsigned instanceCount, unsigned startIndexLocation = 0,
		unsigned baseVertexLocation = 0, unsigned startInstanceLocation = 0) const;

	//////////////////////////////////////////////////////////////////////////
	//Buffer functions
	//Create
	void CreateVertexBuffer(ObjectHandle& vertexBuffer, const BufferUsage bufferUsage, unsigned size, const void* initialData = nullptr);
	void CreateIndexBuffer(ObjectHandle& indexBuffer, const BufferUsage bufferUsage, unsigned size, const void* initialData = nullptr);
	void CreateConstantBuffer(ObjectHandle& constantBuffer, unsigned size);

	//Bind
	void BindNullVertexBuffer();
	void BindVertexBuffer(const ObjectHandle& vertexBuffer, unsigned stride);
	void BindIndexBuffer(const ObjectHandle& indexBuffer);
	void BindConstantBuffer(unsigned slot, const ObjectHandle& constantBuffer, const ObjectType& shaderType);


	//////////////////////////////////////////////////////////////////////////
	//Shader functions
	//Create
	void CreateVertexShader(ObjectHandle& vertexShader, const std::string& fileName, const InputLayout& inputLayout,
		bool precompiled, const std::string& entryPoint = "main");
	void CreatePixelShader(ObjectHandle& pixelShader, const std::string& fileName, bool precompiled, const std::string& entryPoint = "main");
	void CreateGeometryShader(ObjectHandle& geometryShader, const std::string& fileName, bool precompiled, const std::string& entryPoint = "main");
	//Bind
	void BindVertexShader(const ObjectHandle& vertexShader);
	void BindPixelShader(const ObjectHandle& pixelShader);
	void BindGeometryShader(const ObjectHandle& geometryShader);
	void BindComputeShader(const ObjectHandle& computeShader);

	void BindTextureShaderResource(const ObjectType shaderType, unsigned int startSlot, unsigned int numViews, const ObjectHandle& objectWithSRV);

	//////////////////////////////////////////////////////////////////////////
	//Texture functions
	void CreateTexture2D(ObjectHandle& textureHandle, const std::string& fileName, bool generateMipChain = true);
	void CreateTexture2D(ObjectHandle& textureHandle, const int W, const int H, const DataFormat dataFormat,
		bool generateMipChain = true);
	ObjectHandle GetTexture2D(const std::string& fileName);

	//////////////////////////////////////////////////////////////////////////
	//Render Target functions
	void CreateRenderTarget(ObjectHandle& rt, const int W, const int H, const DataFormat dataFormat, bool useDepthBuffer = true);
	void BindRenderTarget(const ObjectHandle& rt, const bool useDepthMap = true);
	void BindNullRenderTarget(const ObjectHandle& depthMapHandle = ObjectHandle::Null());
	void ClearRenderTarget(const ObjectHandle& rt, const float colorArr[4]);
	void ClearRenderTarget(const ObjectHandle& rt, const XMVECTOR& clearColor);

	//////////////////////////////////////////////////////////////////////////
	//Release
	void ReleaseObject(const ObjectHandle& object);

	//////////////////////////////////////////////////////////////////////////
	//Misc.
	//Color blending
	void DisableColorBlending();
	void EnableAlphaBlending();
	void EnableAdditiveBlending();

	int GetRenderTargetWidth() const;
	int GetRenderTargetHeight() const;

protected:
	bool InitializeD3D(const int width, const int height, HWND hwnd);
	bool InitializeSwapChain(const int width, const int height, HWND hwnd);
	bool InitializeConstBuffers();
	bool InitializeRasterizerStates();
	bool InitializeTextureSamplers();
	bool InitializeBlendStates();
	bool ResizeBuffers(const int width, const int height);

	//////////////////////////////////////////////////////////////////////////
	//FOR TESTING
	bool InitializeTestData(const int width, const int height);

	std::unique_ptr<DX11RendererData> m_renderData;
	ObjectHandle CreateHandle(const ObjectType type, const int handle) const;

	bool m_isInitialized = false;

	friend GraphicsSystem;
	friend class TextureManager;

protected:
	void CompileShaderHelper(int& HResult, ID3D10Blob** blobPtrOut, const std::string& fileName,
		const std::string& target, const std::string& szEntryPoint) const;

	int m_renderTargetWidth, m_renderTargetHeight;
};