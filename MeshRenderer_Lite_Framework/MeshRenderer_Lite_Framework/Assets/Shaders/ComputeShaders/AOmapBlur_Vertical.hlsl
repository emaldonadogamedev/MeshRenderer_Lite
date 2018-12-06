//Deferred Rendering Render Targets
Texture2D positionRT : register(t20);
Texture2D normalsRT : register(t21);

Texture2D inputAOmap : register(t0);
RWTexture2D<float> outputAOmap : register(u0);
StructuredBuffer<float> weights : register(t1);

groupshared float sharedMemAOfactors[128 + 15];
groupshared float sharedMemDepths[128 + 15];
groupshared float3 sharedMemNormals[128 + 15];

static const float Rterm = 1.0f / sqrt(6.2831853f * 0.01f);

[numthreads(1, 128, 1)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID, uint3 groupThreadId : SV_GroupThreadID)
{
    uint numStructs; //number of elements in weights buffer
    uint stride; //number of bytes per element in weights buffer - THIS IS A REQUIRED VARIABLE, but not used in the shader code
    weights.GetDimensions(numStructs, stride);

    const int halfSize = int(numStructs) / 2;

    uint outAOmapWidth, outAOmapHeight;
    outputAOmap.GetDimensions(outAOmapWidth, outAOmapHeight);

    const int2 pixelCoords = dispatchThreadId.xy;

    sharedMemAOfactors[groupThreadId.y] = inputAOmap[int2(pixelCoords.x, max(pixelCoords.y - halfSize, 0))].x;
    if (groupThreadId.y < numStructs)
    {
        // read extra 2*w pixels
        int2 coords = int2(pixelCoords.x, min(pixelCoords.y + 128 - halfSize, outAOmapHeight - 1));

        sharedMemAOfactors[groupThreadId.y + 128] = inputAOmap[coords];
        sharedMemDepths[groupThreadId.y + 128] = positionRT[coords].w;
        sharedMemNormals[groupThreadId.y + 128] = normalsRT[coords].xyz;
    }

    float3 currentN = normalsRT[pixelCoords].xyz;
    float currentD = positionRT[pixelCoords].w;

    AllMemoryBarrierWithGroupSync();

    float R, tempDeltaD;

    float finalNumerator = 0.f;
    float finalDenominator = 0.f;

    int sharedMemIdx;
    for (int i = -halfSize, w = 0; i <= halfSize; ++i, ++w)
    {
        sharedMemIdx = groupThreadId.y + i + halfSize;
        tempDeltaD = sharedMemDepths[sharedMemIdx] - currentD;
        R = max(0.f, dot(sharedMemNormals[sharedMemIdx], currentN)) * Rterm * pow(2.71828f, -((tempDeltaD * tempDeltaD) / 0.02f));

        finalNumerator += weights[w] * R * sharedMemAOfactors[sharedMemIdx];
        finalDenominator += weights[w] * R;
    }

    outputAOmap[pixelCoords] = finalDenominator <= 0.0001f ? 0.f : finalNumerator / finalDenominator;
}