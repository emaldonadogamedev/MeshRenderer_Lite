SamplerState textureSamplerWrap		: register(s0);
SamplerState textureSamplerMirror	: register(s1);
SamplerState textureSamplerClamp	: register(s2);
SamplerState textureSamplerBorder	: register(s3);

Texture2D diffTexture : register(t0);
Texture2D specTexture : register(t1);
Texture2D normalTexture : register(t2);

//cubemap faces
TextureCube skyBoxTextures : register(t3);
TextureCube dynCubeMapTextures : register(t4);

//Scene Lights

 //-- light types
static const int LT_DIRECTIONAL = 0;
static const int LT_POINT = 1;
static const int LT_SPOT = 2;

struct Light {
	int m_lightType;
	float3 m_position;

	float4 m_Iambient;
	float4 m_Idiffuse;
	float4 m_Ispecular;

	float3 m_spotDirection;
	float m_spotInnerAngle;

	float m_spotOutterAngle;
	float m_ConstantAttenuation;
	float m_LinearAttenuation;
	float m_QuadraticAttenuation;

	float roughness;
	int isActive;
	int isTaken;
	int padding;
};

static const unsigned int s_maxLights = 15;

cbuffer SceneLights : register(b5)
{
	Light sceneLights[s_maxLights];
}