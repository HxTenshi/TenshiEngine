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

Texture2D txSphereMap : register(t1);
SamplerState samSphereMapLinear : register(s1);

Texture2D txToonMap : register(t2);
SamplerState samToonMapLinear : register(s2);

cbuffer cbNeverChanges : register( b0 )
{
	matrix View;
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

struct PS_OUTPUT
{
	float4 Color0 : SV_Target0;
	float4 Color1 : SV_Target1;
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


float4x3 getBoneMatrix(int idx)
{
	return BoneMatrix[idx];
}


//-------------------------
//ボーン変形＆頂点ブレンド
PS_INPUT VS_Skin(VS_INPUT input)
{
	PS_INPUT output = (PS_INPUT)0;
	float4 pos = input.Pos;
	float3 nor = input.Normal;
	float4x3 bm0 = getBoneMatrix(input.BoneIdx.x);
	float4x3 bm1 = getBoneMatrix(input.BoneIdx.y);
	//input.BoneWeight[0]
	float3 bpos0 = input.BoneWeight[0] * mul(pos, bm0);
	float3 bpos1 = input.BoneWeight[1] * mul(pos, bm1);
	pos.xyz = (bpos0.xyz + bpos1.xyz) / 100.0;

	//input.BoneWeight[0]
	float3 bnor0 = input.BoneWeight[0] * mul(nor, (float3x3)bm0);
	float3 bnor1 = input.BoneWeight[1] * mul(nor, (float3x3)bm1);
	nor = (bnor0.xyz + bnor1.xyz) / 100.0;
	nor = mul(nor, (float3x3)World);
	nor = mul(nor, (float3x3)View);


	pos = mul(pos, World);
	output.VPos = mul(pos, View);
	output.Pos = mul(output.VPos, Projection);
	output.Tex = input.Tex;
	output.Normal = normalize(nor);

	return output;
}



//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
PS_OUTPUT PS(PS_INPUT input)
{
	PS_OUTPUT Out;

	// 法線の準備
	float3 N = normalize(input.Normal);
	
	// ライトのベクトル
	float3 L = normalize(LightVect.xyz);
	
	// 頂点座標から視点へのベクトルを正規化
	float3 V = normalize(-input.VPos.xyz);
	
	// ディフューズ角度減衰率計算
	float DifGen = saturate(dot(N, -L));
	
	// ハーフベクトルの計算 norm( ( norm( 頂点位置から視点へのベクトル ) + ライトの方向 ) )
	float H = normalize(V - L);

	float SpeGen = pow(max(0.0f, dot(N, H)), MSpecular.a);

	float3 SpeColor = SpeGen * LSpecular.rgb;


		// 出力カラー計算 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++( 開始 )

	float4 DifColor = float4(1.0, 1.0, 1.0, 1.0);
	if (UseTexture.x != 0.0)
		DifColor = txDiffuse.Sample(samLinear, input.Tex);
	// トゥーンテクスチャカラーをライトのディフューズ減衰率から取得
	float4 ToonGen = 1.0;
	if (UseTexture.z != 0.0)
		ToonGen = txToonMap.Sample(samToonMapLinear, DifGen);
	//float4 ToonGen = 1.0;// DifGen;

	// ディフューズカラー = ライトのディフューズカラー蓄積値 * マテリアルのディフューズカラー
	float4 LMDif = LDiffuse * MDiffuse;
	float4 LMAmb = LAmbient * MAmbient;
	// スペキュラカラー = ライトのスペキュラカラー蓄積値 * マテリアルのスペキュラカラー
	float4 LMSpe = LSpecular * MSpecular;

	float4 Color;
	// 出力 = saturate( saturate( ディフューズカラー * アンビエントカラーの蓄積値 ) * トゥーンテクスチャカラー + スペキュラカラー ) * テクスチャカラー
	Color.rgb = saturate(saturate(LMDif.rgb + LMAmb.rgb) * ToonGen.rgb + LMSpe.rgb) * DifColor.rgb;

	// アルファ値 = ディフューズアルファ * マテリアルのディフューズアルファ * 不透明度
	Color.a = DifColor.a * MDiffuse.a;


	N = N * float3(0.5f, 0.5f, 0.5f) + float3(0.5f, 0.5f, 0.5f);
	if (UseTexture.y == 0.0){
		Out.Color0 = saturate(Color);
		Out.Color1 = float4(saturate(N), 1);
		return Out;
	}

	// スフィアマップの色を取得
	float3 SphereTex = N;
	SphereTex.y = 1 - SphereTex.y;
	//float4 SphereMapColor = tex2D(ToonSphereMapTexture, SphereTex.xy);
	float4 SphereMapColor = txSphereMap.Sample(samSphereMapLinear, SphereTex.xy);

	/* スフィアマップの効果が乗算の場合はこちらを加算の２行の代わりに使います
	// スフィアマップの色を出力に乗算
	PSOutput.Color0.rgb *= lToonSphereMapColor.rgb ;
	*/
	// スフィアマップの色を出力に加算
	Color.rgb += SphereMapColor.rgb;

	Out.Color0 = saturate(Color);
	Out.Color1 = float4(saturate(N),1);


	return Out;
}
