#pragma once

#include<memory>
#include<Systems/Graphics/GraphicsUtilities/ObjectHandle.h>

//TODO: REMOVE AFTER TESTING
#include <DirectXMath.h>
using DirectX::XMMATRIX;

class GraphicsSystem;
class InputLayout;
class DX11RendererData;
enum class ObjectType : char;

class DX11Renderer
{
public:
	DX11Renderer();
	~DX11Renderer();

	bool InitializeRenderer(const int width, const int height, HWND hwnd);
	void ReleaseData();

	void ClearBuffer(void);
	void SwapBuffers(void);

	//////////////////////////////////////////////////////////////////////////
	//Drawing Functions
	void Draw(unsigned vertexCount, unsigned startVertexLocation = 0);
	void DrawIndexed(unsigned indexCount, unsigned startIndexLocation = 0, unsigned baseVertexLocation = 0);
	void DrawInstanced(unsigned vertexCount, unsigned instanceCount, unsigned startVertexLocation = 0,
		unsigned startInstanceLocation = 0);
	void DrawIndexedInstanced(unsigned indexCountPerInstance, unsigned instanceCount, unsigned startIndexLocation = 0,
		unsigned baseVertexLocation = 0, unsigned startInstanceLocation = 0);

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

protected:
	bool InitializeD3D(const int width, const int height, HWND hwnd);

	//////////////////////////////////////////////////////////////////////////
	//FOR TESTING
	bool InitializeTestData(const int width, const int height);
	struct CBChangesEveryFrame
	{
		XMMATRIX worldMtx;
	};
	CBChangesEveryFrame testPerObjectBuffer;

	struct CBNeverChanges
	{
		XMMATRIX viewMtx, projectionMtx;
	};
	CBNeverChanges testViewProjBuffer;

	bool InitializeTextureSamplers();

	std::unique_ptr<DX11RendererData> m_renderData;

	bool m_isInitialized = false;

	friend GraphicsSystem;

private:
	void CompileShaderHelper(int& HResult, ID3D10Blob** blobPtrOut, const std::string& fileName,
		const std::string& target, const std::string& szEntryPoint);
};