
//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------

TextureCube SkyBoxCubeTex : register(t6);
SamplerState SkyBoxCubeSampler : register(s6);

Texture2D SkyBoxTex : register(t6);
SamplerState SkyBoxSampler : register(s6);

cbuffer cbNeverChanges : register( b0 )
{
	matrix View;
	matrix ViewInv;
};
cbuffer cbChangeOnResize : register(b1)
{
	matrix Projection;
	matrix ProjectionInv;
};
//--------------------------------------------------------------------------------------
struct VS_INPUT
{
	float4 Pos		: POSITION;
	float2 Tex		: TEXCOORD0;
};

struct PS_INPUT
{
	float4 Pos		: SV_POSITION;
	float2 Tex		: TEXCOORD0;
	float3 WPos		: TEXCOORD1;
};

struct PS_OUTPUT_1
{
	float4 ColorAlbedo : SV_Target0;
	float4 ColorSpecular : SV_Target1;
	float4 ColorNormal : SV_Target2;
	float4 ColorDepth : SV_Target3;
	float4 ColorVelocity : SV_Target4;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS(VS_INPUT input)
{
	PS_INPUT output = (PS_INPUT)0;
	output.Pos = input.Pos;
	output.Pos.z = 0.9999999;
	output.Tex = input.Tex;
	output.WPos = mul(input.Pos, ProjectionInv);
	output.WPos = mul(output.WPos, (float3x3)ViewInv);
	return output;
}

float3 GetEnvironmentPanorama(float3 dir){

	const float PI = 3.14159265359;
	const float TwoPI = PI + PI;
	float phi = acos(dir.y);
	float theta = atan2(-1.0 * dir.x, dir.z) + PI;
	float2 tex = float2(theta / TwoPI, phi / PI);

	return pow(SkyBoxTex.SampleLevel(SkyBoxSampler, tex.xy, 0).rgb, 2.2);
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
PS_OUTPUT_1 PS(PS_INPUT input)
{
	PS_OUTPUT_1 Out;
	//SkyBoxCubeTex.Sample(SkyBoxCubeSampler, normalize(input.WPos));
	Out.ColorAlbedo.rgb = GetEnvironmentPanorama(normalize(input.WPos));
	Out.ColorAlbedo.a = 1.0f;
	Out.ColorSpecular = float4(0, 0, 0, 0);
	Out.ColorNormal = float4(0.5, 0.5, 0.5, 1);
	Out.ColorDepth = float4(0, 0, 0, 1);
	Out.ColorVelocity = float4(0.5, 0.5, 0, 1);
	return Out;
}
