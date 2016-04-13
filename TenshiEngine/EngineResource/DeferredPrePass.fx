//--------------------------------------------------------------------------------------
// File: Tutorial07.fx
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
Texture2D AlbedoTex : register( t0 );
SamplerState AlbedoSamLinear : register(s0);
Texture2D NormalTex : register(t1);
SamplerState NormalSamLinear : register(s1);
Texture2D HeightTex : register(t2);
SamplerState HeightSamLinear : register(s2);

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
	matrix BeforeWorld;
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
	float2 MTexScale;
	float2 MHightPower;
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
	float3 Bin	: BINORMAL;
	float3 Tan	: TANGENT;
	float2 Tex		: TEXCOORD0;
	uint4 BoneIdx		: BONEINDEX;
	uint4 BoneWeight	: BONEWEIGHT;
};

struct PS_INPUT
{
	float4 Pos		: SV_POSITION;
	float3 Normal	: NORMAL;
	float3 Bin	: TEXCOORD0;
	float3 Tan	: TEXCOORD1;
	float2 Tex		: TEXCOORD2;
	float4 VPos		: TEXCOORD3;
	float4 BefPos		: TEXCOORD4;

	float4 LPos[4]		: TEXCOORD5;
};

struct PS_OUTPUT_1
{
	float4 ColorAlbedo : SV_Target0;
	float4 ColorSpecular : SV_Target1;
	float4 ColorNormal : SV_Target2;
	float4 ColorDepth : SV_Target3;
	float4 ColorVelocity : SV_Target4;
};
//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS( VS_INPUT input )
{
	PS_INPUT output = (PS_INPUT)0;
	output.Pos = mul(input.Pos, World);

	//output.LVPos = mul(output.Pos, LView);
	//output.LPos = mul(output.LVPos, LProjection);
	output.LPos[0] = mul(output.Pos, LViewProjection[0]);
	output.LPos[1] = mul(output.Pos, LViewProjection[1]);
	output.LPos[2] = mul(output.Pos, LViewProjection[2]);
	output.LPos[3] = mul(output.Pos, LViewProjection[3]);


	output.VPos = mul( output.Pos, View );
	output.Pos = mul( output.VPos, Projection);

	output.BefPos = mul(input.Pos, BeforeWorld);
	output.BefPos = mul(output.BefPos, View);

	//output.Bin = -cross(input.Normal, input.Tan);
	//output.Tan = cross(input.Normal, output.Bin);

	//output.Normal = mul(input.Normal, (float3x3)World);
	//output.Normal = mul(output.Normal, (float3x3)View);
	//output.Bin = mul(input.Bin, (float3x3)World);
	//output.Bin = mul(output.Bin, (float3x3)View);
	//output.Tan = mul(input.Tan, (float3x3)World);
	//output.Tan = mul(output.Tan, (float3x3)View);


	//output.Bin = -cross(output.Normal, output.Tan);
	//output.Tan = cross(output.Normal, output.Bin);
	output.Normal = input.Normal;
	output.Bin = input.Bin;
	output.Tan = input.Tan;

	output.Tex = input.Tex * MTexScale;
	
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
	float3 nor = input.Normal;


	float3 bpos = float3(0, 0, 0);
	float3 bnor = float3(0, 0, 0);
	[unroll]
	for (uint i = 0; i < 4; i++){
		float4x3 bm = getBoneMatrix(input.BoneIdx[i]);
		bpos += input.BoneWeight[i] * mul(pos, bm).xyz;
		bnor += input.BoneWeight[i] * mul(nor, (float3x3)bm);
	}

	pos.xyz = bpos / 100.0;
	nor = bnor / 100.0;



	PS_INPUT output = (PS_INPUT)0;
	output.Pos = mul(pos, World);

	//output.LVPos = mul(output.Pos, LView);
	//output.LPos = mul(output.LVPos, LProjection);
	output.LPos[0] = mul(output.Pos, LViewProjection[0]);
	output.LPos[1] = mul(output.Pos, LViewProjection[1]);
	output.LPos[2] = mul(output.Pos, LViewProjection[2]);
	output.LPos[3] = mul(output.Pos, LViewProjection[3]);

	output.VPos = mul(output.Pos, View);
	output.Pos = mul(output.VPos, Projection);

	output.BefPos = mul(pos, BeforeWorld);
	output.BefPos = mul(output.BefPos, View);

	//output.Normal = mul(nor, (float3x3)World);
	//output.Normal = mul(output.Normal, (float3x3)View);

	//ここなおせ
	//output.Bin = mul(input.Bin/*ここ*/, (float3x3)World);
	//output.Bin = mul(output.Bin, (float3x3)View);
	//output.Tan = mul(input.Tan/*ここ*/, (float3x3)World);
	//output.Tan = mul(output.Tan, (float3x3)View);

	output.Normal = input.Normal;
	output.Bin = input.Bin;
	output.Tan = input.Tan;

	output.Tex = input.Tex;

	return output;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
PS_OUTPUT_1 PS(PS_INPUT input)
{
	PS_OUTPUT_1 Out;

	float2 texcood = input.Tex;

	float3 ray = input.VPos.xyz;
	//ray.z = -ray.z;
	ray = normalize(ray);

	if (UseTexture.z != 0.0){

		float3 x = normalize(float3(World._m11, World._m21, World._m31));
		float3 y = normalize(float3(World._m12, World._m22, World._m32));
		float3 z = normalize(float3(World._m13, World._m23, World._m33));
		float3x3 NoScale = float3x3(x, y, z);

		float3 nor = mul(input.Normal, NoScale);
		nor = mul(nor, (float3x3)View);
		nor = normalize(nor);

		float pow = 1 -HeightTex.Sample(HeightSamLinear, input.Tex).r ;
		float2 h = nor.xy * pow *  MHightPower.x / 100;
		texcood += h *float2(1, -1);
	}


	// 法線の準備
	float3 N;

	if (UseTexture.y != 0.0){
		float3 bump = NormalTex.Sample(NormalSamLinear, texcood).rgb * 2 - 1.0;
			bump.g *= -1;

		//bump = normalize(bump);

		//bump = mul(bump, (float3x3)World);
		//bump = mul(bump, (float3x3)View);
		
		float3 Nor = input.Normal;
		float3 Bin = input.Bin;
		float3 Tan = input.Tan;

		//視線ベクトルを頂点座標系に変換する
		float3 n;
		n.x = dot(bump, Tan);
		n.y = dot(bump, Bin);
		n.z = dot(bump, Nor);
		bump = n;

		float3 x = normalize(float3(World._m11, World._m21, World._m31));
		float3 y = normalize(float3(World._m12, World._m22, World._m32));
		float3 z = normalize(float3(World._m13, World._m23, World._m33));
		float3x3 NoScale = float3x3(x, y, z);

		//bump = mul(bump, NoScale);
		bump = mul(bump, (float3x3)World);
		bump = mul(bump, (float3x3)View);
		bump = normalize(bump);

		N = bump * 0.5 + 0.5;
	}else{
		N = mul(input.Normal, (float3x3)World);
		N = mul(N, (float3x3)View);
		N = normalize(N);
		N = N * 0.5 + 0.5;
	}

	float farClip = 100;
	float D = input.VPos.z / farClip;
	//float D2 = input.Pos.z / input.Pos.w;


	float3 LD;

	float dist = input.VPos.z;
	if (dist < SplitPosition.x)
	{
		LD.xy = (input.LPos[0].xy + 1.0) / 2.0;
		LD.z = input.LPos[0].z / input.LPos[0].w;
	}
	else if (dist < SplitPosition.y)
	{
		LD.xy = (input.LPos[1].xy + 1.0) / 2.0;
		LD.z = input.LPos[1].z / input.LPos[1].w;
	}
	else if (dist < SplitPosition.z)
	{
		LD.xy = (input.LPos[2].xy + 1.0) / 2.0;
		LD.z = input.LPos[2].z / input.LPos[2].w;
	}
	else
	{
		LD.xy = (input.LPos[3].xy + 1.0) / 2.0;
		LD.z = input.LPos[3].z / input.LPos[3].w;
	}
	
	float2 velocity;
	{
		float2 b = input.BefPos.xy / input.BefPos.z;
		float2 p = input.VPos.xy / input.VPos.z;
		velocity = p - b;
		//velocity = normalize(velocity);
		velocity = velocity * 0.5 + 0.5;
	}

	// 出力カラー計算 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++( 開始 )

	float4 DifColor = float4(1.0, 1.0, 1.0, 1.0);
		if (UseTexture.x != 0.0)
			DifColor = AlbedoTex.Sample(AlbedoSamLinear, texcood);


	ray = ray*0.5 + 0.5;

	Out.ColorAlbedo = DifColor * MDiffuse;
	Out.ColorSpecular = float4(ray.x, ray.y, ray.z, MSpecular.r);
	Out.ColorNormal = float4(N, MSpecular.a);
	Out.ColorDepth = float4(D, 1 - LD.z, LD.x, 1.0 - LD.y);
	Out.ColorVelocity = float4(velocity.x, velocity.y, 0, 1);
	return Out;
}
