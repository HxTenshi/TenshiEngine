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
	output.Pos.x = input.Pos.x;
	output.Pos.y = input.Pos.y;
	output.Pos.xy = output.Pos.xy * 2-1;
	output.Tex = input.Tex;
	
	return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(PS_INPUT input) : SV_Target
{
	float4 col = txDiffuse.Sample(samLinear, input.Tex);
	col.r = 0;
	return col;
}