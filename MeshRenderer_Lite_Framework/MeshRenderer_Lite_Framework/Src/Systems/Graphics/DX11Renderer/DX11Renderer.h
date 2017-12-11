#pragma once

//TODO: REMOVE AFTER TESTING
#include <DirectXMath.h>
#include<memory>
#include<Systems/Graphics/GraphicsUtilities/ObjectHandle.h>
#include <Systems/Graphics/GraphicsUtilities/InputData.h>

using DirectX::XMMATRIX;

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

	void ClearBuffer(void) const;
	void SwapBuffers(void) const;

	DX11RendererData& GetRendererData() const;

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

	//////////////////////////////////////////////////////////////////////////
	//Texture functions
	void CreateTexture2D(ObjectHandle& texture, const std::string& fileName, bool generateMipChain = true);
	
	void BindTexture2D(unsigned slot, const ObjectHandle& texture);

	//////////////////////////////////////////////////////////////////////////
	//Release
	void ReleaseObject(const ObjectHandle& object);

protected:
	bool InitializeD3D(const int width, const int height, HWND hwnd);
	bool InitializeConstBuffers();
	bool InitializeTextureSamplers();

	//////////////////////////////////////////////////////////////////////////
	//FOR TESTING
	bool InitializeTestData(const int width, const int height);

	std::unique_ptr<DX11RendererData> m_renderData;
	ObjectHandle CreateHandle(const ObjectType type, const int handle) const;

	bool m_isInitialized = false;

	friend GraphicsSystem;
	friend class TextureManager;

private:
	void CompileShaderHelper(int& HResult, ID3D10Blob** blobPtrOut, const std::string& fileName,
		const std::string& target, const std::string& szEntryPoint) const;
};