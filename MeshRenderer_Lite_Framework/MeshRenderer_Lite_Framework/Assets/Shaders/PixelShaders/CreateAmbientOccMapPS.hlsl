
#include "../ShaderIncludes.hlsli"
#include "PixelShaderIncludes.hlsli"

float main(PixelInputType pixel) : SV_TARGET
{
    const float2 uv = pixel.uv;
    const float4 positionAndDepth = positionRT.Sample(textureSamplerWrap, uv);
    const float3 normal = normalsRT.Sample(textureSamplerWrap, uv).xyz;
    
    uint RTwidth, RTheight;
    positionRT.GetDimensions(RTwidth, RTheight);

    const uint pixX = uv.x * float(RTwidth);
    const uint pixY = uv.y * float(RTheight);

    //integer weird spiral radius
    const int phi = (30 * pixX ^ pixY) + 10 * pixX * pixY;
    const float c = 0.1f * gAmbOccWorldSpaceRange;

    //temp storages
    float alpha;
    float h;
    float theta;
    float2 uvi;
    float4 PiAndDi;
    float3 wi;
    float wiLength;

    float sum = 0.f;
    float numerator;
    float denominator;

    for (int i = 0; i < gAmbientOccPointsOfSample; ++i)
    {
        alpha = (float(i) + 0.5f) / float(gAmbientOccPointsOfSample);
        h = alpha * gAmbOccWorldSpaceRange / (positionAndDepth.w * 100.0f);
        theta = TWO_PI * alpha * ((7.f * float(gAmbientOccPointsOfSample)) / 9.f) + float(phi);

        uvi = uv + float2(h * cos(theta), h * sin(theta));
        PiAndDi = positionRT.Sample(textureSamplerWrap, uvi);

        wi = PiAndDi.xyz - positionAndDepth.xyz;
        wiLength = length(wi);

        numerator = max(0.f, dot(normal, wi) - (0.001f * (PiAndDi.w * 100.f))) * ((gAmbOccWorldSpaceRange - wiLength) < 0.f ? 0.f : 1.f);
        denominator = max(c * c, dot(wi, wi));
        sum += denominator <= 0.0001f ? 0.f : numerator / denominator;
    }

    const float Sterm1 = TWO_PI * c / float(gAmbientOccPointsOfSample);
    float S = Sterm1 * sum;

    //final Ambient factor of the pixel
    return saturate(pow(max(0.f, 1.0f - gAmbienOcc_S_scaleFactor * S), gAmbienOcc_K_scaleFactor));
}