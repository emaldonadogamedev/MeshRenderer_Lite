#include "../TextureShaderIncludes.hlsli"

Texture2D inputTexture2D: register(t0);
RWTexture2D<float4> outputTexture2D : register(u1);

[numthreads(32, 32, 1)]
void main( uint3 dispatchThreadId : SV_DispatchThreadID )
{
		float4 result = float4(0, 0, 0, 0);
		uint w, h;
		inputTexture2D.GetDimensions(w, h);

		for (uint x = dispatchThreadId.x - 1; x < dispatchThreadId.x + 1; ++x)
		{
				for (uint y = dispatchThreadId.y - 1; y < dispatchThreadId.y + 1; ++y)
				{
						if (x >= 0 && x < w && y >= 0 && y < h)
						{
								result += inputTexture2D[dispatchThreadId.xy];
						}
				}
		}

		result /= 9.0f;

		outputTexture2D[dispatchThreadId.xy] = result;
}