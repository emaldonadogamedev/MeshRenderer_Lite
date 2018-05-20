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

//Shadow map textures(15)
#define SHADOW_MAP(x) shadowMap_##x

Texture2D shadowMap_0 : register(t5);
Texture2D shadowMap_1 : register(t6);
Texture2D shadowMap_2 : register(t7);
Texture2D shadowMap_3 : register(t8);
Texture2D shadowMap_4 : register(t9);
Texture2D shadowMap_5 : register(t10);
Texture2D shadowMap_6 : register(t11);
Texture2D shadowMap_7 : register(t12);
Texture2D shadowMap_8 : register(t13);
Texture2D shadowMap_9 : register(t14);
Texture2D shadowMap_10 : register(t15);
Texture2D shadowMap_11 : register(t16);
Texture2D shadowMap_12 : register(t17);
Texture2D shadowMap_13 : register(t18);
Texture2D shadowMap_14 : register(t19);


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