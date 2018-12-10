// Input control point
struct VS_CONTROL_POINT_OUTPUT
{
    float4 worldPos : WORLDPOS;
    float2 uv : UV;
};

// Output control point
struct HS_CONTROL_POINT_OUTPUT
{
	float3 vPosition : WORLDPOS; 
    float2 uv : UV;
};

// Output patch constant data.
struct HS_CONSTANT_DATA_OUTPUT
{
	float EdgeTessFactor[4]			: SV_TessFactor; // e.g. would be [4] for a quad domain
	float InsideTessFactor[2]		: SV_InsideTessFactor; // e.g. would be Inside[2] for a quad domain
	// TODO: change/add other stuff
};

#define NUM_CONTROL_POINTS 4

// Patch Constant Function
HS_CONSTANT_DATA_OUTPUT CalcHSPatchConstants(
	InputPatch<VS_CONTROL_POINT_OUTPUT, NUM_CONTROL_POINTS> ip,
	uint PatchID : SV_PrimitiveID)
{
	HS_CONSTANT_DATA_OUTPUT Output;

	// Insert code to compute Output here
	Output.EdgeTessFactor[0] = 
	Output.EdgeTessFactor[1] = 
	Output.EdgeTessFactor[2] = 
    Output.EdgeTessFactor[3] = 
	Output.InsideTessFactor[0] =
    Output.InsideTessFactor[1] = 256; // e.g. could calculate dynamic tessellation factors instead

	return Output;
}

//The main entry point of the hull shader
[domain("quad")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(4)]
[patchconstantfunc("CalcHSPatchConstants")]
HS_CONTROL_POINT_OUTPUT main( 
	InputPatch<VS_CONTROL_POINT_OUTPUT, NUM_CONTROL_POINTS> ip, 
	uint i : SV_OutputControlPointID,
	uint PatchID : SV_PrimitiveID )
{
	HS_CONTROL_POINT_OUTPUT Output;

	// Insert code to compute Output here
    Output.vPosition = ip[i].worldPos;
    Output.uv = ip[i].uv;


	return Output;
}
