#pragma once

#include <Systems/Graphics/GraphicsUtilities/GraphicsTypes.h>
#include <Systems/Graphics/GraphicsUtilities/ObjectHandle.h>

//struct ID3D11Texture1D;
//struct ID3D11Texture2D;
//struct ID3D11Texture3D;
//
//struct ID3D11RenderTargetView;
//struct ID3D11DepthStencilView;
//struct ID3D11ShaderResourceView;
//
//struct ID3D10Blob;
//struct ID3D11VertexShader;
//struct ID3D11InputLayout;
//struct ID3D11PixelShader;
//struct ID3D11GeometryShader;
//struct ID3D11ComputeShader;
//struct ID3D11HullShader;
//
//struct ID3D11Buffer;

struct D3DObjectBase
{
	D3DObjectBase(void) : deleted(false) {}
	bool deleted;
};

struct TextureObjectBase : D3DObjectBase
{
		ID3D11ShaderResourceView* srv = nullptr;
};

struct Texture1D : TextureObjectBase
{
	ID3D11Texture1D* texture1D;
	int size;
};

struct Texture2D : TextureObjectBase
{
	ID3D11Texture2D* texture2D;
	int width = 0, height = 0;

	float GetAspectRatio() const
	{
			return float(width) / float(height);
	}
};

struct RenderTarget : Texture2D
{
	ID3D11RenderTargetView* rtv = nullptr;
	ID3D11UnorderedAccessView* uav = nullptr;
	DataFormat format;

	//Associated depth/stencil buffer
	ID3D11Texture2D* depthBuffer = nullptr;
	ID3D11DepthStencilView* depthStencilView = nullptr;
	ID3D11ShaderResourceView* depthMapSrv = nullptr;

	//float downSamplePercentage;
	//Viewport viewport;
};

struct DepthStencilBuffer : Texture2D
{
		//Associated depth/stencil buffer
		ID3D11DepthStencilView* depthStencilView = nullptr;
};

struct Texture3D : TextureObjectBase
{
		ID3D11Texture3D* texture3D;
		int widthPerTexture = 0, heightPerTexture = 0;
};
typedef Texture3D CubeMap;

struct ShaderObjectBase : D3DObjectBase
{
	ID3D10Blob* shaderBlob;
};

struct VertexShader : ShaderObjectBase
{
	ID3D11VertexShader* vertexShader;
	ID3D11InputLayout* layout;
};

struct PixelShader : ShaderObjectBase
{
	ID3D11PixelShader* pixelShader;
};

struct GeometryShader : ShaderObjectBase
{
	ID3D11GeometryShader* geometryShader;
};

struct ComputeShader : ShaderObjectBase
{
	ID3D11ComputeShader* computeShader;
};

struct HullShader : ShaderObjectBase
{
	ID3D11HullShader* hullShader;
};

struct Buffer : D3DObjectBase
{
	ID3D11Buffer* buffer;
	BufferUsage usage;
	unsigned int size;
};

struct StructuredBufferRW_Texture2D : Texture2D
{
		ID3D11UnorderedAccessView* uav;
};

struct DepthBuffer : D3DObjectBase
{
	ID3D11ShaderResourceView* srv;
	ID3D11DepthStencilView* depthStencilView;
	ObjectHandle toResizeWith;
	int width = 0, height = 0;
};