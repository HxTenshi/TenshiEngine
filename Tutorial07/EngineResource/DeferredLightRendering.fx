//--------------------------------------------------------------------------------------
// File: Tutorial07.fx
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
Texture2D NormalTex : register( t0 );
SamplerState NormalSamLinear : register(s0);
Texture2D DepthTex : register(t1);
SamplerState DepthSamLinear : register(s1);

cbuffer cbNeverChanges : register(b0)
{
	matrix View;
};
cbuffer cbChangesLight : register(b3)
{
	float4 LightVect;
	float4 LightColor;
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

	// 法線の準備
	float3 N = NormalTex.Sample(NormalSamLinear, input.Tex).xyz;
	N = N * 2 - 1;

	// ライトのベクトル
	float3 L = LightVect.xyz;
	L = mul(L, (float3x3)View).xyz;
	L = normalize(L);

	// ディフューズ角度減衰率計算
	float DifGen = saturate(dot(N, -L));
	DifGen = DifGen*0.5 + 0.25;

	float4 col = LightColor * DifGen;
	col.a = LightColor.a;
	return col;
}
