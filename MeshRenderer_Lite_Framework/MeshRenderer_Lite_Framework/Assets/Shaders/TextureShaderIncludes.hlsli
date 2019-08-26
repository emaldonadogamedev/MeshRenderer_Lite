SamplerState textureSamplerWrap		: register(s0);
SamplerState textureSamplerMirror	: register(s1);
SamplerState textureSamplerClamp	: register(s2);
SamplerState textureSamplerBorder	: register(s3);

//Model material textures
Texture2D diffTexture : register(t0);
Texture2D specTexture : register(t1);
Texture2D normalTexture : register(t2);

//cubemap faces
TextureCube skyBoxTextures : register(t3);
TextureCube dynCubeMapTextures : register(t4);

//Shadow map textures(15)
#define SHADOW_MAP(x) shadowMap_##x
Texture2D shadowMaps[15] : register(t5);

//Deferred Rendering Render Targets
Texture2D positionRT : register(t20);
Texture2D normalsRT : register(t21);
Texture2D diffuseRT : register(t22);
Texture2D specularAndNsRT : register(t23);
Texture2D previouslyUsedRT : register(t24);

//IBL resources
Texture2D iblMap2D : register(t25);
Texture2D irradianceMap2D : register(t26);
StructuredBuffer<float2> sampleWeights : register(t27);

//Volumetric light resource
Texture2D volumLightShadowMap : register(t29); //pos t28 reserved for AO map, in other branch
Texture2D volumeLight_F_512 : register(t30);
Texture2D volumeLight_G_0 : register(t31);
Texture2D volumeLight_G_20 : register(t32);
Texture2D mainRT_Depth : register(t33);