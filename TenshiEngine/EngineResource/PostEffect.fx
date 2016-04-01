//--------------------------------------------------------------------------------------
// File: Tutorial07.fx
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
Texture2D PostTex : register(t0);
SamplerState PostSamLinear : register(s0);
Texture2D ScreenTex : register(t1);
SamplerState ScreenSamLinear : register(s1);
Texture2D AlbedoTex : register(t2);
SamplerState AlbedoSamLinear : register(s2);
Texture2D SpecularTex : register(t3);
SamplerState SpecularSamLinear : register(s3);
Texture2D NormalTex : register(t4);
SamplerState NormalSamLinear : register(s4);
Texture2D DepthTex : register(t5);
SamplerState DepthSamLinear : register(s5);
Texture2D LightTex : register(t6);
SamplerState LightSamLinear : register(s6);
Texture2D LightSpeTex : register(t7);
SamplerState LightSpeSamLinear : register(s7);
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


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(PS_INPUT input) : SV_Target
{
	float4 col = PostTex.Sample(PostSamLinear, input.Tex);
	return col;
}
