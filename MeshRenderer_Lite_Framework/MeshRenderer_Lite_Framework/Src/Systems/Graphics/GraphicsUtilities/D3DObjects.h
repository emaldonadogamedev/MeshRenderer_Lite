#pragma once

#include <Systems/Graphics/GraphicsUtilities/GraphicsTypes.h>
#include <Systems/Graphics/GraphicsUtilities/ObjectHandle.h>

struct ID3D11Texture1D;
struct ID3D11Texture2D;
struct ID3D11Texture3D;

struct ID3D11RenderTargetView;
struct ID3D11DepthStencilView;
struct ID3D11ShaderResourceView;

struct ID3D10Blob;
struct ID3D11VertexShader;
struct ID3D11InputLayout;
struct ID3D11PixelShader;
struct ID3D11GeometryShader;
struct ID3D11ComputeShader;
struct ID3D11HullShader;

struct ID3D11Buffer;

struct D3DObject
{
	D3DObject(void) : deleted(false) {}
	bool deleted;
};

struct TextureObject : D3DObject
{
		ID3D11ShaderResourceView* srv = nullptr;
};

struct Texture1D : TextureObject
{
	ID3D11Texture1D* texture1D;
	int size;
};

struct Area
{
	unsigned int width, height;
};

struct Texture2D : TextureObject
{
	ID3D11Texture2D* texture2D;
	Area size;
};

struct Texture3D : TextureObject
{
	ID3D11Texture3D* texture3D;
	Area sizePerTexture;
};
typedef Texture3D CubeMap;

struct RenderTarget : TextureObject
{
	ID3D11Texture2D* texture = nullptr;
	ID3D11RenderTargetView* rtv = nullptr;
	DataFormat format;

	//Associated depth stencil
	ID3D11DepthStencilView* depthStencilView = nullptr;
	ID3D11Texture2D* depthBuffer = nullptr;

	int width = 0, height = 0;

	float GetAspectRatio() const 
	{
			return float(width) / float(height);
	}

	//float downSamplePercentage;
	//Viewport viewport;
};

struct ShaderBase : D3DObject
{
	ID3D10Blob* shaderBlob;
};

struct VertexShader : ShaderBase
{
	ID3D11VertexShader* vertexShader;
	ID3D11InputLayout* layout;
};

struct PixelShader : ShaderBase
{
	ID3D11PixelShader* pixelShader;
};

struct GeometryShader : ShaderBase
{
	ID3D11GeometryShader* geometryShader;
};

struct ComputeShader : ShaderBase
{
	ID3D11ComputeShader* computeShader;
};

struct HullShader : ShaderBase
{
	ID3D11HullShader* hullShader;
};

struct Buffer : D3DObject
{
	ID3D11Buffer* buffer;
	BufferUsage usage;
	unsigned int size;
};

struct DepthBuffer : D3DObject
{
	ID3D11ShaderResourceView* srv;
	ID3D11DepthStencilView* depthStencilView;
	ObjectHandle toResizeWith;
	Area size;
};