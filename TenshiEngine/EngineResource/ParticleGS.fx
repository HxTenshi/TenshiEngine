
//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
Texture2D txDiffuse : register( t0 );
SamplerState samLinear : register(s0);
Texture2D DepthMap : register(t1);
SamplerState DepthSamLinear : register(s1);

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

cbuffer CBBillboard  : register(b8)
{
	matrix mBillboardMatrix;
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


cbuffer CBChangesPaticleParam : register(b10)
{
	//w=ランダムスピード
	float4 Vector;
	//w=[Sphere=0]~[Box=1]
	float4 Point;
	//w=空気抵抗
	float4 RotVec;
	//x = min, y = max, z=反射力, w=摩擦減衰
	float4 MinMaxScale;
	//x = min, y = max, z=Z-Collision, w=VeloRot&Bura
	float4 Time;
	//w=pointG-Pow
	float4 G;
	//x = num, y=impact, z=pointG-Rot, w=time
	float4 Param;
};

// ジオメトリシェーダーの入力パラメータ
struct VS_IN
{
	float3 pos : POSITION;  // 座標
	float3 v0  : NORMAL;    // 初速度
	float3 v  : BINORMAL;    // 速度
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

float GetRandomNumber(float2 texCoord, int Seed)
{
	return frac(sin(dot(texCoord.xy, float2(12.9898, 78.233)) + Seed) * 43758.5453);
}

[maxvertexcount(1)]   // ジオメトリシェーダーで出力する最大頂点数
// ジオメトリシェーダー
void GS0_Main(point GS_IN In[1],                   // ポイント プリミティブの入力情報
	inout PointStream<GS0_OUT> PStream,   // ポイント プリミティブの出力ストリーム
	uint ID : SV_PrimitiveID
	)
{
	GS0_OUT Out;
	Out.time = In[0].time;

	if (((int)Param.x) <= ID){
		Out.time = -10000.0f;
		Out.v0.xz = 0;
	}
	if (Out.time <= -0.017f){
		Out.pos = In[0].pos;
		Out.v0 = In[0].v0;
		Out.v = In[0].v;

		if (In[0].time <= -9999.0f){
			Out.pos = In[0].pos;
			Out.v0 = In[0].v0;
			Out.v = In[0].v;

			float rand = GetRandomNumber(float2(Param.w / 12345.0f, Param.w / 543.0f), 354 + ID + Param.w / 28.0f);
			float time = GetRandomNumber(float2(rand, rand), 361 + ID);
			Out.time = -(Time.y)*time * (1-Param.y);

		}
		else{
			Out.time = In[0].time + 0.016f;
		}

	}else
	// パーティクルの時間が g_LimitTime を超えたので初期化する
	if (Out.time <= 0.0f)
	{
		float rand = GetRandomNumber(float2(Param.w / 12345.0f, Param.w / 543.0f), 354 + ID + Param.w/28.0f);
		float2 rand2 = float2(rand, 1-rand);

		Out.pos = World._41_42_43;

		float px = GetRandomNumber(rand2, 353 + ID);
		float py = GetRandomNumber(rand2, 352 + ID);
		float pz = GetRandomNumber(rand2, 351 + ID);
		float3 posv = float3(px, py, pz) * 2 - 1;
		float3 nposv = posv;
		if (length(posv) != 0){
			nposv = normalize(posv);
		}
		Out.pos += lerp(nposv, posv, Point.w) * Point.xyz;


		float scale = GetRandomNumber(rand2, 355 + ID);
		Out.v0.x = ((MinMaxScale.y - MinMaxScale.x)*scale + MinMaxScale.x)/2.0f;
		Out.v0.y = 0;
		Out.v0.z = 0;

		float x = GetRandomNumber(rand2, 356 + ID);
		float y = GetRandomNumber(rand2, 357 + ID);
		float z = GetRandomNumber(rand2, 358 + ID);
		float w = GetRandomNumber(rand2, 359 + ID);

		

		float3 rot3 = (RotVec.xyz / 360.0f) * (3.1415926535f);
		rot3 *= float3(x, y, z) * 2 - 1;

		float3x3 ZRot =
			float3x3(
			cos(rot3.z), sin(rot3.z), 0,
			-sin(rot3.z), cos(rot3.z), 0,
			0, 0, 1);
		float3x3 XRot =
			float3x3(
			1,0,0,
			0, cos(rot3.x), sin(rot3.x),
			0, -sin(rot3.x), cos(rot3.x)
			);
		float3x3 YRot =
			float3x3(
			cos(rot3.y), 0, -sin(rot3.y),
			0,1,0,
			sin(rot3.y), 0, cos(rot3.y)
			);

		
		Out.v = mul(Vector.xyz, mul(mul(XRot, YRot), ZRot));
		Out.v -= Vector.w * Out.v * w;

		Out.v = mul(Out.v, (float3x3)World);

		float time = GetRandomNumber(rand2, 360 + ID);
		Out.time = (Time.y - Time.x)*time + Time.x;

	}
	else
	{

		matrix matWVP = mul(View, Projection);

		Out.v0 = In[0].v0;
		Out.v = In[0].v;

		//ポイントグラビティ
		float3 tar = In[0].pos - World._41_42_43;
			if (length(tar) != 0){
				//Point
				float3 posG = normalize(tar) * G.w;
				Out.v += posG;

				//Rotate
				float3 nor = normalize(tar) * -1;
					float3 bv = Out.v*-1;
					float vl = length(Out.v);
				if (vl != 0){
					bv = normalize(bv);
				}
				bv = nor - dot(nor, bv) * 2 * bv;
				bv *= vl;
				Out.v = lerp(Out.v, bv, Param.z);
			}


		Out.pos = In[0].pos + Out.v;



		if (Time.w != 0){
			float3 scrV = mul(Out.v, (float3x3)matWVP);
			Out.v0.y = atan2(scrV.x, scrV.y);
			Out.v0.z = length(scrV.xy) * Time.w;
		}

		Out.v *= RotVec.w;

		// 時間を進める
		Out.time = In[0].time - 0.016f;


		float4 screenpos = mul(float4(Out.pos, 1), matWVP);

		float depth = screenpos.z / 100.0f;
		screenpos.xy /= screenpos.w;
		screenpos.xy = screenpos.xy * 0.5 + 0.5;
		screenpos.y = 1-screenpos.y;
		screenpos.xyz *= float3(1200,800,0);
		float r = DepthMap.Load(screenpos).r;
		float3 nor = txDiffuse.Load(screenpos).rgb * 2 - 1;

		//Out.pos = DepthMap.Load(screenpos).rgb;
		if (r != 0 && abs(r - depth)<Time.z)
		{

			nor = mul(nor, (float3x3)ViewInv);
			Out.pos -= Out.v;
			Out.v = Out.v - dot(Out.v, nor)*(MinMaxScale.z+1)*nor;
			Out.v *= MinMaxScale.w;

			//Out.v += -G.xyz;
		}
		else{
			Out.v += G.xyz;
		}
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
	float g_Scale = In[0].v0.x;
	float g_ScaleY = In[0].v0.z;

	float bc = (g_Scale / (g_Scale + g_ScaleY*1.5));
	float fc = (bc + 1)/2.0;
	float4 fcol = float4(fc, fc, fc, 1);
	float4 bcol = float4(bc, bc, bc, 1);

	float rot = In[0].v0.y;
	float4x4 ZRot = 
		float4x4(
		 cos(rot), sin(rot), 0,0,
		 -sin(rot), cos(rot), 0, 0,
		        0,        0, 1,0,
				0,0,0,1);


	//matrix matWVP = mBillboardMatrix;
	matrix W = mul(ZRot, mBillboardMatrix);
	W._41_42_43 = float3(In[0].pos.x, In[0].pos.y, In[0].pos.z);

	matrix WVP = mul(mul(W, View), Projection);

	//float4 scale = mul(float4(g_Scale, g_Scale, 0, 0), matWVP);

	//float4(In[0].pos.x + g_Scale, In[0].pos.y + g_Scale, In[0].pos.z, 1.0f)
	Out.pos = mul(float4(g_Scale, g_Scale, 0, 1), WVP);
	Out.color = fcol;
	Out.texel = float2(1, 0);
	TriStream.Append(Out);

	Out.pos = mul(float4(-g_Scale, g_Scale, 0, 1), WVP);
	Out.color = fcol;
	Out.texel = float2(0, 0);
	TriStream.Append(Out);

	Out.pos = mul(float4(g_Scale, -g_Scale - g_ScaleY, 0, 1), WVP);
	Out.color = bcol;
	Out.texel = float2(1, 1);
	TriStream.Append(Out);

	Out.pos = mul(float4(-g_Scale, -g_Scale - g_ScaleY, 0, 1), WVP);
	Out.color = bcol;
	Out.texel = float2(0, 1);
	TriStream.Append(Out);

	TriStream.RestartStrip();
}

typedef GS1_OUT PS_IN;

// ピクセルシェーダ
float4 PS1_Main(PS_IN In) : SV_TARGET
{
	return txDiffuse.Sample(samLinear, In.texel) * MDiffuse * MHightPower.y * In.color;
	//return DepthMap.Sample(DepthSamLinear, In.texel) * In.color;
	//float2 xy = In.texel;
	//xy.x *= 1200;
	//xy.y *= 800;
	//float r = DepthMap.Load(float3(xy, 0)).r;
	//return float4(r,r,r,1);
	//return float4(In.color.xyz,1);
}