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

Texture2D LightDepthTex1 : register(t2);
SamplerState LightDepthSamLinear1 : register(s2);
Texture2D LightDepthTex2 : register(t3);
SamplerState LightDepthSamLinear2 : register(s3);
Texture2D LightDepthTex3 : register(t4);
SamplerState LightDepthSamLinear3 : register(s4);
Texture2D LightDepthTex4 : register(t5);
SamplerState LightDepthSamLinear4 : register(s5);

cbuffer cbNeverChanges : register(b0)
{
	matrix View;
};
cbuffer cbChangeOnResize : register(b1)
{
	matrix Projection;
	matrix ProjectionInv;
};
cbuffer cbChangesLight : register(b3)
{
	float4 LightVect;
	float4 LightColor;
};

cbuffer cbChangesLightCamera : register(b10)
{
	matrix LViewProjection[4];
	float4 SplitPosition;
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

//projection depth 0~1
float3 DepthToPosition2(float2 textureCoord, float depth)
{
	float3 vpos;
	// スクリーン座標　左上(-1,+1) 右下(+1,-1)
	float2 spos = textureCoord * float2(2, -2) + float2(-1, 1);
		vpos.z = Projection._m23 / (depth/* - Projection._m22*/);
	vpos.xy = (spos.xy*vpos.z) / float2(Projection._m00, Projection._m11);

	return vpos;
}

//view depth 0~1
float ToPerspectiveDepth(float viewDepth)
{
	float4 projectedPosition = mul(float4(0, 0, viewDepth, 1.0), Projection);
		return 1.0 - (projectedPosition.z / projectedPosition.w);
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(PS_INPUT input) : SV_Target
{

	// 法線の準備
	float3 N = NormalTex.Sample(NormalSamLinear, input.Tex).xyz;
	N = N * 2 - 1;

	if (N.x == 0 && N.y == 0 && N.z == 0){
		return float4(1, 1, 1, 1);
	}

	// ライトのベクトル
	float3 L = LightVect.xyz;
		L = mul(L, (float3x3)View).xyz;
	L = normalize(L);


	float NLDot = dot(N, -L);


	float offset = 0.0001 + (NLDot) * 0.0001;
	float shadow = 0.0;
	float shadowCount = 1.0f;
	// ライトデプスの準備
	float4 LVPos = DepthTex.Sample(DepthSamLinear, input.Tex).zwyx;

	//float LD = LightDepthTex1.Sample(LightDepthSamLinear1, LVPos.xy).x;
	//shadow += step(LVPos.z + offset, LD);


	// 位置(view座標系)
	float dep = LVPos.w;
	dep = ToPerspectiveDepth(dep);
	float3 vpos = DepthToPosition2(input.Tex, dep);

		//float4 debugColor = float4(0, 0, 0, 1);

	float dist = vpos.z - 0.01f;
	//float dist = dep;
	if (dist < SplitPosition.x)
	{
		//index = 0;
		float LD = LightDepthTex1.Sample(LightDepthSamLinear1, LVPos.xy).x;
		shadow += step(LVPos.z + offset, LD);
		//debugColor.r = 1;
	}
	else if (dist < SplitPosition.y)
	{
		float LD = LightDepthTex2.Sample(LightDepthSamLinear2, LVPos.xy).x;
		shadow += step(LVPos.z + offset, LD);
		//debugColor.g = 1;
	}
	else if (dist < SplitPosition.z)
	{
		float LD = LightDepthTex3.Sample(LightDepthSamLinear3, LVPos.xy).x;
		shadow += step(LVPos.z + offset, LD);
		//debugColor.b = 1;
	}
	else
	{
		float LD = LightDepthTex4.Sample(LightDepthSamLinear4, LVPos.xy).x;
		shadow += step(LVPos.z + offset, LD);
		//debugColor.rg = 1;
	}

	shadow = 1 - (shadow);


	// ディフューズ角度減衰率計算
	float DifGen = saturate(NLDot);
	DifGen *= shadow;
	DifGen = DifGen + 0.25;
	DifGen = saturate(DifGen);

	//debugColor *= float4(DifGen, DifGen, DifGen, 1);
	//debugColor = float4(1, 1, 1, 2) - debugColor;

	float4 col = LightColor * DifGen;
	col.a = LightColor.a;
	return col;
}
