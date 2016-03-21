//--------------------------------------------------------------------------------------
// File: Tutorial07.fx
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
struct VS_INPUT
{
	float4 Pos		: POSITION;
};

struct PS_INPUT
{
	float4 Pos		: SV_POSITION;
};

cbuffer cbNeverChanges : register(b0)
{
	matrix View;
};

cbuffer cbChangeOnResize : register(b1)
{
	matrix Projection;
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
};
cbuffer cbChangesUseTexture : register(b6)
{
	float4 UseTexture;
};
//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS( VS_INPUT input )
{
	PS_INPUT output = (PS_INPUT)0;
	output.Pos = mul(input.Pos, World);
	output.Pos = mul(output.Pos, View);
	output.Pos = mul(output.Pos, Projection);
	
	return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(PS_INPUT input) : SV_Target
{
	float4 col = MDiffuse;
	return col;
}
