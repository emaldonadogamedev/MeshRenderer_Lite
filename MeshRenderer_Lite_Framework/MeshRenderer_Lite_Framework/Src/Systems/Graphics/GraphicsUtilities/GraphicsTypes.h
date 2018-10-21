#pragma once

enum class ObjectType : char
{
	NONE = -1,
	BACK_BUFFER,
	DEPTH_BUFFER,
	STRUCTURED_BUFFER_RW_TEXTURE1D,
	STRUCTURED_BUFFER_RW_TEXTURE2D,
	TEXTURE_1D,
	TEXTURE_2D,
	TEXTURE_3D,
	RENDER_TARGET,
	SHADER_RESOURCE_VIEW,
	VERTEX_SHADER,
	PIXEL_SHADER,
	GEOMETRY_SHADER,
	COMPUTE_SHADER,
	HULL_SHADER,
	VERTEX_BUFFER,
	INDEX_BUFFER,
	INSTANCE_BUFFER,
	CONSTANT_BUFFER,
	FONT_FACTORY,
	FONT_WRAPPER,

	COUNT
};

enum class DataFormat : char
{
  UNKNOWN,
	R_UNORM,
	RG_UNORM,
	RGBA_UNORM,
	BGRA_UNORM,
	FLOAT4,
	FLOAT3,
	FLOAT2,
	FLOAT1,
	UNSIGNED1,
	UNSIGNED2,
	UNSIGNED3,
	UNSIGNED4,
	INT1,
	INT2,
	INT3,
	INT4,
	COUNT,
};

enum class UavDimension : char 
{
		UNKNOWN = 0,
		BUFFER = 1,
		TEXTURE1D = 2,
		TEXTURE1DARRAY = 3,
		TEXTURE2D = 4,
		TEXTURE2DARRAY = 5,
		TEXTURE3D = 8
};

enum class FeatureLevel : char
{
	FEATURE_LEVEL_DX9_1 = 0,
	FEATURE_LEVEL_DX9_2,
	FEATURE_LEVEL_DX9_3,
	FEATURE_LEVEL_DX10_0,
	FEATURE_LEVEL_DX10_1,
	FEATURE_LEVEL_DX11_0,
	FEATURE_LEVEL_DX11_1,
	FEATURE_LEVEL_DX12_0,
	FEATURE_LEVEL_DX12_1,
	FeatureLevel_COUNT,
};

enum class PrimitiveTopology : char
{
  PRIMITIVE_TOPOLOGY_UNDEFINED,
  PRIMITIVE_TOPOLOGY_POINTLIST,
  PRIMITIVE_TOPOLOGY_LINELIST,
  PRIMITIVE_TOPOLOGY_LINESTRIP,
  PRIMITIVE_TOPOLOGY_TRIANGLELIST,
  PRIMITIVE_TOPOLOGY_TRIANGLESTRIP
};

enum class BufferUsage : char
{
  USAGE_DEFAULT = 0,
  USAGE_IMMUTABLE,
  USAGE_DYNAMIC,
  USAGE_STAGING,
};

enum class FillMode : char
{
  FILL_SOLID = 0,
  FILL_WIREFRAME,
};

enum class CullMode : char
{
  CULL_MODE_NONE = 0,
  CULL_MODE_FRONT_FACE,
  CULL_MODE_BACK_FACE,
  CULL_MODE_SCISSOR,
  CULL_MODE_IMGUI,
};

enum class BlendMode : char
{
  BLEND_MODE_NOT_SET = -1,
  BLEND_MODE_NONE,
  BLEND_MODE_ALPHA,
  BLEND_MODE_ADDITIVE,
  BLEND_MODE_ADDITIVE_RENDER_TARGET,
};

enum class TextureFilter : char
{
  FILTER_POINT = 0,
  FILTER_BILINEAR,
  FILTER_TRILINEAR,
  FILTER_ANISOTROPIC,
};

enum class TextureAdressing : char
{
  CLAMP = 0,
  MIRROR,
  WRAP,
  BORDER,
};

enum class Anisotropy : char
{
  ANISOTROPY_1X = 0,
  ANISOTROPY_2X,
  ANISOTROPY_4X,
  ANISOTROPY_8X,
  ANISOTROPY_16X
};

enum class MSAALevel : char
{
  MSAA_LEVEL_NONE = 0,
  MSAA_LEVEL_2X,
  MSAA_LEVEL_4X,
  MSAA_LEVEL_8X,
  MSAA_LEVEL_16X,
  MSAA_LEVEL_32X
};

enum class FrustumPlane : char
{
  NEAR_PLANE,
  FAR_PLANE,
  LEFT_PLANE,
  RIGHT_PLANE,
  TOP_PLANE,
  BOTTOM_PLANE,
};

struct Rect 
{
  int left;
  int right;
  int top;
  int bottom;
};