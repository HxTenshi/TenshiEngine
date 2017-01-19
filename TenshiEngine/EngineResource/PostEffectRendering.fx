//--------------------------------------------------------------------------------------
// File: Tutorial07.fx
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
Texture2D txDiffuse : register( t0 );
SamplerState samLinear : register(s0);

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
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS( VS_INPUT input )
{
	PS_INPUT output = (PS_INPUT)0;
	output.Pos = input.Pos;
	output.Tex = input.Tex;
	return output;
}

float3 Uncharted2Tonemap(float3 x)
{
	float A = 0.15;
	float B = 0.50;
	float C = 0.10;
	float D = 0.20;
	float E = 0.02;
	float F = 0.30;
	return ((x*(A*x + C*B) + D*E) / (x*(A*x + B) + D*F)) - E / F;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(PS_INPUT input) : SV_Target
{
	float4 col = saturate(txDiffuse.Sample(samLinear, input.Tex));

	col.rgb *= 2.0f;
	float ExposureBias = 2.0f;
	float3 curr = Uncharted2Tonemap(ExposureBias*col.rgb);

	float W = 11.2;
	float3 whiteScale = 1.0f / Uncharted2Tonemap(W);
	float3 color = curr*whiteScale;

	col.rgb = pow(color, (1 / 2.2));
	return col;
}
