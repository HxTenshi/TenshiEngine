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

cbuffer cbNeverChanges : register(b0)
{
	matrix View;
	matrix ViewInv;
};

cbuffer cbChangeOnResize : register(b1)
{
	matrix Projection;
};

cbuffer cbChangesEveryFrame : register(b2)
{
	matrix World;
	matrix BeforeWorld;
};

cbuffer cbChangesMaterial : register(b4)
{
	float4 MDiffuse;
	float4 MSpecular;
	float4 MAmbient;
	float2 MTexScale;
	float2 MHightPower;
	float4 MNormaleScale;
};

cbuffer CBBillboard  : register(b8)
{
	matrix mBillboardMatrix;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS( VS_INPUT input )
{
	matrix W = mul(mBillboardMatrix, World);
	matrix WV = mul(W, View);
	matrix WVP = mul(WV, Projection);
	WVP._11_12_13 *= -1;

	PS_INPUT output = (PS_INPUT)0;
	output.Pos = mul(input.Pos, WVP);
	output.Tex = input.Tex;
	
	return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(PS_INPUT input) : SV_Target
{
	float4 col = txDiffuse.Sample(samLinear, input.Tex);
	if (col.a <= 0.1f)discard;
	return col * MDiffuse;
}
