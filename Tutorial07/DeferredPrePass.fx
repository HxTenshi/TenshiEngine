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
	float3 Normal	: NORMAL;
	float2 Tex		: TEXCOORD0;
	float4 VPos		: TEXCOORD1;
};

struct PS_OUTPUT_1
{
	float4 ColorAlbedo : SV_Target0;
	float4 ColorNormal : SV_Target1;
	float4 ColorDepth : SV_Target2;
};
//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS( VS_INPUT input )
{
	PS_INPUT output = (PS_INPUT)0;
	output.Pos = mul( input.Pos, World );
	output.VPos = mul( output.Pos, View );
	output.Pos = mul( output.VPos, Projection);

	output.Normal = mul(input.Normal, (float3x3)World);
	output.Normal = mul(output.Normal, (float3x3)View);

	output.Tex = input.Tex;
	
	return output;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
PS_OUTPUT_1 PS(PS_INPUT input)
{
	PS_OUTPUT_1 Out;

	// 法線の準備
	float3 N = normalize(input.Normal);
	N = N * 0.5 + 0.5;

	float farClip = 100;
	float D = input.VPos.z / farClip;
	//float D2 = input.Pos.z / input.Pos.w;

	
	//MSpecular

	// 出力カラー計算 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++( 開始 )

	float4 DifColor = float4(1.0, 1.0, 1.0, 1.0);
	if (UseTexture.x != 0.0)
		DifColor = txDiffuse.Sample(samLinear, input.Tex);


	Out.ColorAlbedo = DifColor;
	Out.ColorNormal = float4(N,1);
	Out.ColorDepth = float4(D, 0, 0, 1);
	return Out;
}
