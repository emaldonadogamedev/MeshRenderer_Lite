
#include "../ShaderIncludes.hlsli"
#include "PixelShaderIncludes.hlsli"

float main(PixelInputType pixel) : SV_TARGET
{
    const float2 uv = pixel.uv;
    const float4 positionAndDepth = positionRT.Sample(textureSamplerWrap, uv);
    const float3 normal = normalsRT.Sample(textureSamplerWrap, uv).xyz;
    
    const int pixX = (int) pixel.position.x;
    const int pixY = (int) pixel.position.y;

    //integer weird spiral radius
    const int phi = (30 * pixX ^ pixY) + (10 * pixX * pixY);
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

    for (int n = 0; n < gAmbientOccPointsOfSample; ++n)
    {
        alpha = (float(n) + 0.5f) / float(gAmbientOccPointsOfSample);
        h = alpha * gAmbOccWorldSpaceRange / positionAndDepth.w;
        theta = TWO_PI * alpha * ((7.f * n) / 9.f) + float(phi);

        uvi = uv + float2(h * cos(theta), h * sin(theta));
        PiAndDi = positionRT.Sample(textureSamplerWrap, uvi);

        wi = PiAndDi.xyz - positionAndDepth.xyz;
        wiLength = length(wi);

        numerator = max(0.f, dot(normal, wi) - (0.001f * PiAndDi.w)) * (gAmbOccWorldSpaceRange - wiLength < 0.f ? 0.f : 1.f);
        denominator = max(c * c, dot(wi, wi));
        sum += numerator / denominator;
    }

    const float term1 = TWO_PI * c / float(n);
    float S = term1 * sum;

    //final Ambient factor of the pixel
    return pow(max(0.f, 1.0f - gAmbienOcc_S_scaleFactor * S), gAmbienOcc_K_scaleFactor);
}