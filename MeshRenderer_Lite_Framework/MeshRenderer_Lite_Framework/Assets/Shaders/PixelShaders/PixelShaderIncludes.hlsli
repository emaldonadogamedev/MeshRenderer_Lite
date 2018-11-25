#include "../TextureShaderIncludes.hlsli"

// Object Material
struct PhongMaterial
{
	float4 ambientKa;
	float4 diffuseKd;

	float3 specularKs;
	float specularPowerNs;

	float4 emissiveKe;

	int useDiffuseTexture;
	int useNormalMap;
	int useAlphaBlending;
	int isShadowCaster;
};

cbuffer ObjectMaterial : register(b6)
{
	PhongMaterial meshMaterial;
}

//Scene Lights

 //-- light types
static const int LT_DIRECTIONAL = 0;
static const int LT_POINT = 1;
static const int LT_SPOT = 2;

struct Light 
{
	int m_lightType;
	float3 m_position;

	float3 m_Iambient;
	float m_spotInnerAngle;

	float4 m_Idiffuse;

	float3 m_Ispecular;
	float m_spotOutterAngle;

	float3 m_spotDirection;
	float m_ConstantAttenuation;

	float m_LinearAttenuation;
	float m_QuadraticAttenuation;
	float roughness;
	int isActive;

	int isTaken;
	int isUsingShadows;
	int isUsingSoftShadows;
	int padding;
};

static const unsigned int s_maxLights = 15;
cbuffer SceneLights : register(b7)
{
		Light sceneLights[s_maxLights];
}

struct SimpleLight
{
	float3 m_position;
	float m_range;

	float3 m_Iambient;
	int isTaken;

	float3 m_Idiffuse;
	int isActive;

	float attConst;
	float attLinear;
	float attQuadratic;
	float padding;
};

cbuffer SceneLightsNoShadow : register(b8)
{
		SimpleLight sceneLightNoShadow;
}

//Debug information types
static const int G_DEBUG_NONE = 0;
static const int G_DEBUG_POSITION = 1;
static const int G_DEBUG_DEPTH = 2;
static const int G_DEBUG_UV_COORDS = 3;
static const int G_DEBUG_NORMALS = 4;
static const int G_DEBUG_DIFFUSE = 5;
static const int G_DEBUG_SPECULAR = 6;
static const int G_DEBUG_COUNT = 7;

cbuffer GlobalShaderProperties : register(b9)
{
	float4 gClearColor;

	float3 gGlobalAmbient;
	int gDebugInfoType;

	int gIsUsingDeferred;
	int gIsUsingIBL;
	float toneMappingExposureControl;
	float toneMappingExtraExpControl;

	int gIsUsingIrrMap;
	int3 b9_padding;
};