//Deferred Rendering Render Targets
Texture2D positionRT : register(t20);
Texture2D normalsRT : register(t21);

Texture2D inputAOmap : register(t0);
RWTexture2D<float> outputAOmap : register(u0);
StructuredBuffer<float> weights : register(t1);

groupshared float sharedMemAOfactors[128 + 51];
groupshared float sharedMemDepths[128 + 51];
groupshared float3 sharedMemNormals[128 + 51];

static const float Rterm = 1.0f / sqrt(6.2831853f * 0.01f);

[numthreads(128, 1, 1)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID, uint3 groupThreadId : SV_GroupThreadID)
{
    uint numStructs; //number of elements in weights buffer
    uint stride; //number of bytes per element in weights buffer - THIS IS A REQUIRED VARIABLE, but not used in the shader code
    weights.GetDimensions(numStructs, stride);

    const int halfSize = int(numStructs) / 2;

    uint outAOmapWidth, outAOmapHeight;
    outputAOmap.GetDimensions(outAOmapWidth, outAOmapHeight);

    const int2 pixelCoords = dispatchThreadId.xy;

    int2 coords = int2(max(pixelCoords.x - halfSize, 0), pixelCoords.y);
    sharedMemAOfactors[groupThreadId.x] = inputAOmap[coords].x;
    sharedMemDepths[groupThreadId.x] = positionRT[coords].w * 100.f;
    sharedMemNormals[groupThreadId.x] = normalsRT[coords].xyz;
    if (groupThreadId.x < (halfSize * 2))
    {
        // read extra 2*w pixels
        int2 coords = int2(min(pixelCoords.x + 128 - halfSize, outAOmapWidth - 1), pixelCoords.y);

        sharedMemAOfactors[groupThreadId.x + 128] = inputAOmap[coords];
        sharedMemDepths[groupThreadId.x + 128] = positionRT[coords].w * 100.f;
        sharedMemNormals[groupThreadId.x + 128] = normalsRT[coords].xyz;
    }

    float3 currentN = normalsRT[pixelCoords].xyz;
    float currentD = positionRT[pixelCoords].w * 100.f;

    AllMemoryBarrierWithGroupSync();

    float R, tempDeltaD;

    float finalNumerator = 0.f;
    float finalDenominator = 0.f;

    int sharedMemIdx;
    for (int i = -halfSize, w = 0; i <= halfSize; ++i, ++w)
    {
        sharedMemIdx = groupThreadId.x + i + halfSize;
        tempDeltaD = sharedMemDepths[sharedMemIdx] - currentD;
        R = max(0.f, dot(sharedMemNormals[sharedMemIdx], currentN)) * Rterm * pow(2.71828f, -(tempDeltaD * tempDeltaD / 0.02f));

        finalNumerator += weights[w] * R * sharedMemAOfactors[sharedMemIdx];
        finalDenominator += weights[w] * R;
    }

    outputAOmap[pixelCoords] = saturate(finalNumerator / finalDenominator);
    //outputAOmap[pixelCoords] = finalNumerator;
}