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

cbuffer cbChangesEveryFrame : register(b2)
{
	matrix World;
};

cbuffer cbChangesMaterial : register(b4)
{
	float4 MDiffuse;
	float4 MSpecular;
	float4 MAmbient;
	float2 MTexScale;
	float2 MHightPower;
	float4 MNormaleScale;
	float2 MOffset;
	float EmissivePowor;
	float MNull;
};
//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS( VS_INPUT input )
{
	PS_INPUT output = (PS_INPUT)0;
	output.Pos = mul(input.Pos, World);
	output.Pos.z = 0;
	output.Tex = input.Tex * MTexScale + MOffset;
	
	return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(PS_INPUT input) : SV_Target
{
	float4 col = txDiffuse.Sample(samLinear, input.Tex);
	if (col.a <= 0.001f)discard;
	return col * MDiffuse * MHightPower.y;
}
