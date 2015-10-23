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

cbuffer CBBillboard  : register(b8)
{
	matrix mBillboardMatrix;
};

// ジオメトリシェーダーの入力パラメータ
struct VS_IN
{
	float3 pos : POSITION;  // 座標
	float3 v0  : NORMAL;    // 初速度
	float time : TEXCOORD;  // 時間
};

typedef VS_IN VS_OUT;

// 頂点シェーダーではそのまま出力
VS_OUT VS0_Main(VS_IN In)
{
	return In;
}

typedef VS_OUT GS_IN;
typedef GS_IN GS0_OUT;

[maxvertexcount(6)]   // ジオメトリシェーダーで出力する最大頂点数
// ジオメトリシェーダー
void GS0_Main(point GS_IN In[1],                   // ポイント プリミティブの入力情報
	inout PointStream<GS0_OUT> PStream   // ポイント プリミティブの出力ストリーム
	)
{
	GS0_OUT Out;

	float g_LimitTime = 10.0f;

	// パーティクルの時間が g_LimitTime を超えたので初期化する
	if (In[0].time > g_LimitTime)
	{
		Out.pos = float3(0, 0, 0);
		Out.v0 = In[0].v0;
		Out.time = 0;
	}
	else
	{
		// X 位置 = 初速度 × 時間
		Out.pos.x = In[0].v0.x * In[0].time;

		// 自由落下運動の公式により、Y 座標を計算する。
		// 9.80665f は重力加速度。
		Out.pos.y = -0.1f * 9.80665f * In[0].time * In[0].time + In[0].v0.y * In[0].time;

		// Z 位置 = 初速度 × 時間
		Out.pos.z = In[0].v0.z * In[0].time;

		Out.v0 = In[0].v0;

		// 時間を進める
		Out.time = In[0].time + 0.1f;
	}

	PStream.Append(Out);
	PStream.RestartStrip();
}

struct GS1_OUT
{
	float4 pos    : SV_POSITION;  // パーティクルの位置
	float4 color  : COLOR;        // パーティクルの色
	float2 texel  : TEXCOORD0;    // テクセル
};

[maxvertexcount(4)]   // ジオメトリシェーダーで出力する最大頂点数
// ジオメトリシェーダー
void GS1_Main(point GS_IN In[1],                       // ポイント プリミティブの入力情報
	inout TriangleStream<GS1_OUT> TriStream  // トライアングル プリミティブの出力ストリーム
	)
{
	GS1_OUT Out;

	float g_LimitTime = 10.0f;
	float g_Scale = 0.2f;

	// 時間は 0.0f ～ g_LimitTime の範囲内で変化する
	float c = 1.0f - In[0].time / g_LimitTime;

	// 頂点カラーは、時間の経過に伴い徐々に黄色から赤っぽくかつ透明になるようにしている。
	float4 color = float4(1, c * c, 0, c);


	matrix matWVP = mBillboardMatrix;
	matWVP._41_42_43_44 = World._14_24_34_44;
	matWVP = mul(matWVP, View);
	matWVP = mul(matWVP, Projection);
	//matrix matWVP = World;

	Out.pos = mul(float4(In[0].pos.x + g_Scale, In[0].pos.y + g_Scale, In[0].pos.z, 1.0f), matWVP);
	Out.color = color;
	Out.texel = float2(1, 0);
	TriStream.Append(Out);

	Out.pos = mul(float4(In[0].pos.x - g_Scale, In[0].pos.y + g_Scale, In[0].pos.z, 1.0f), matWVP);
	Out.color = color;
	Out.texel = float2(0, 0);
	TriStream.Append(Out);

	Out.pos = mul(float4(In[0].pos.x + g_Scale, In[0].pos.y - g_Scale, In[0].pos.z, 1.0f), matWVP);
	Out.color = color;
	Out.texel = float2(1, 1);
	TriStream.Append(Out);

	Out.pos = mul(float4(In[0].pos.x - g_Scale, In[0].pos.y - g_Scale, In[0].pos.z, 1.0f), matWVP);
	Out.color = color;
	Out.texel = float2(0, 1);
	TriStream.Append(Out);

	TriStream.RestartStrip();
}

typedef GS1_OUT PS_IN;

// ピクセルシェーダ
float4 PS1_Main(PS_IN In) : SV_TARGET
{
	return txDiffuse.Sample(samLinear, In.texel) * In.color;
	//return float4(In.color.xyz,1);
}