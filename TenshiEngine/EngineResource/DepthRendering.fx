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

cbuffer cbNeverChanges : register( b0 )
{
	matrix View;
	matrix ViewInv;
};

cbuffer cbChangeOnResize : register( b1 )
{
	matrix Projection;
};

cbuffer cbChangesEveryFrame : register( b2 )
{
	matrix World;
};

cbuffer cbChangesLight : register( b3 )
{
	float4 LightVect;
	float4 LightColor;
};
cbuffer cbChangesMaterial : register( b4 )
{
	float4 MDiffuse;
	float4 MSpecular;
	float4 MAmbient;
};
cbuffer cbChangesLightMaterial : register( b5 )
{
	float4 LDiffuse;
	float4 LSpecular;
	float4 LAmbient;
};

cbuffer cbChangesUseTexture : register( b6 )
{
	float4 UseTexture;
};
cbuffer cbBoneMatrix : register(b7)
{
	//float4x3 BoneMatrix[153];
	float4x3 BoneMatrix[2] :  packoffset(c0);
	float4x3 BoneMatrixDmmy[253] :  packoffset(c6);
	float4x3 BoneMatrixEnd :  packoffset(c765);
}

cbuffer cbChangesLightCamera : register(b10)
{
	matrix LViewProjection[4];
	float4 SplitPosition;
};

//--------------------------------------------------------------------------------------
struct VS_INPUT
{
	float4 Pos		: POSITION;
	float3 Normal	: NORMAL;
	float2 Tex		: TEXCOORD0;
	uint4 BoneIdx		: BONEINDEX;
	uint4 BoneWeight	: BONEWEIGHT;
};

struct PS_INPUT
{
	float4 Pos		: SV_POSITION;
	//float4 VPos		: TEXCOORD0;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS( VS_INPUT input )
{
	PS_INPUT output = (PS_INPUT)0;
	output.Pos = mul( input.Pos, World );
	//output.VPos = mul(output.Pos, LView);
	//output.Pos = mul(output.VPos, LProjection);
	output.Pos = mul(output.Pos, LViewProjection[0]);
	
	return output;
}


float4x3 getBoneMatrix(int idx)
{
	return BoneMatrix[idx];
}

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VSSkin(VS_INPUT input)
{

	float4 pos = input.Pos;


	float3 bpos = float3(0, 0, 0);
	[unroll]
	for (uint i = 0; i < 4; i++){
		float4x3 bm = getBoneMatrix(input.BoneIdx[i]);
		bpos += input.BoneWeight[i] * mul(pos, bm).xyz;
	}

	pos.xyz = bpos / 100.0;

	PS_INPUT output = (PS_INPUT)0;
	output.Pos = mul(pos, World);
	//output.VPos = mul(output.Pos, LView);
	//output.Pos = mul(output.VPos, LProjection);
	output.Pos = mul(output.Pos, LViewProjection[0]);

	return output;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(PS_INPUT input) : SV_Target
{
	//float farClip = 100;
	float D = input.Pos.z / input.Pos.w;
	return float4(1 - D, 0, 0, 1);
}
