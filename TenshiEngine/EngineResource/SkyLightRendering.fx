//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
Texture2D NormalTex : register(t0);
SamplerState NormalSamLinear : register(s0);
Texture2D DepthTex : register(t1);
SamplerState DepthSamLinear : register(s1);
Texture2D SpecularTex : register(t2);
SamplerState SpecularSamLinear : register(s2);


Texture2D SkyBoxTex : register(t7);
SamplerState SkyBoxSampler : register(s7);

cbuffer cbNeverChanges : register(b0)
{
	matrix View;
	matrix ViewInv;
};
cbuffer cbChangeOnResize : register(b1)
{
	matrix Projection;
	matrix ProjectionInv;
};

cbuffer cbNearFar : register(b12)
{
	float Near;
	float Far;
	float2 NULLnf;
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
PS_INPUT VS(VS_INPUT input)
{
	PS_INPUT output;
	output.Pos = input.Pos;
	output.Tex = input.Tex;

	return output;
}
struct PS_OUTPUT_1
{
	float4 Diffuse : SV_Target0;
	float4 Specular : SV_Target1;
};


//projection depth 0~1
inline float3 DepthToPosition2(float2 textureCoord, float depth)
{
	float3 vpos;
	// スクリーン座標　左上(-1,+1) 右下(+1,-1)
	float2 spos = textureCoord * float2(2, -2) + float2(-1, 1);
		vpos.z = Projection._m23 / (depth/* - Projection._m22*/);
	vpos.xy = (spos.xy*vpos.z) / float2(Projection._m00, Projection._m11);

	return vpos;
}

//view depth 0~1
inline float ToPerspectiveDepth(float viewDepth)
{
	float4 projectedPosition = mul(float4(0, 0, viewDepth, 1.0), Projection);
		return 1.0 - (projectedPosition.z / projectedPosition.w);
}

// V を計算するための便利関数
inline float G1V(float dotNV, float k)
{
	return 1.0f / (dotNV * (1.0f - k) + k);
}

const static float PI = 3.141592653589793f;

// GGX を使ったスペキュラシェーディングの値を返す関数
inline float LightingFuncGGX_REF(float3 N, float3 V, float3 L,
	// ラフネス
	float roughness,
	// フレネル反射率 F(0°)
	float F0)
{
	// α = ラフネス^2
	float alpha = roughness * roughness;

	// ハーフベクトル
	float3 H = normalize(V + L);

		// ベクトルの内積関連
		float dotNL = saturate(dot(N, L));
	float dotNV = saturate(dot(N, V));
	float dotNH = saturate(dot(N, H));
	float dotLH = saturate(dot(L, H));

	float F, D, vis;

	// D
	float alphaSqr = alpha * alpha;
	float denom = dotNH * dotNH * (alphaSqr - 1.0) + 1.0f;
	D = alphaSqr / (PI * denom * denom);

	// F
	float dotLH5 = pow(1.0f - dotLH, 5);
	F = F0 + (1.0 - F0) * (dotLH5);

	// V
	float k = alpha / 2.0f;
	vis = G1V(dotNL, k) * G1V(dotNV, k);

	float specular = dotNL * D * F * vis;

	return specular;
}


float3 GetEnvironmentPanorama(float3 dir, float roughness){

	dir = mul(dir, (float3x3)ViewInv);

	const float PI = 3.14159265359;
	const float TwoPI = PI + PI;
	float phi = acos(dir.y);
	float theta = atan2(-1.0 * dir.x, dir.z) + PI;
	float2 tex = float2(theta / TwoPI, phi / PI);

		return pow(SkyBoxTex.SampleLevel(SkyBoxSampler, tex.xy, 8.0f + roughness*1.5f).rgb,2.2);
}

float3 GetEnvironmentPanoramaSpec(float3 ray, float3 N, float roughness){
	float3 ref = reflect(ray, N);       // 反射ベクトル
		ref = mul(ref, (float3x3)ViewInv);

	const float PI = 3.14159265359;
	const float TwoPI = PI + PI;
	float phi = acos(ref.y);
	float theta = atan2(-1.0 * ref.x, ref.z) + PI;
	float2 tex = float2(theta / TwoPI, phi / PI);

		return pow(SkyBoxTex.SampleLevel(SkyBoxSampler, tex.xy, roughness * 10.0f).rgb, 2.2);
}


inline PS_OUTPUT_1 main(PS_INPUT input, float normalVec){
	PS_OUTPUT_1 Out;
	Out.Diffuse = float4(1, 1, 1, 1);
	Out.Specular = float4(0, 0, 0, 1);

	float4 norCol = NormalTex.Sample(NormalSamLinear, input.Tex);
		// 法線の準備
		float3 N = norCol.xyz;
		N = N * 2 - 1;
	N *= normalVec;

	if (N.x == 0 && N.y == 0 && N.z == 0){
		return Out;
	}


	float roughness = norCol.a - 1;
	roughness = max(roughness, 0.1f);
	float3 env;
	{
		env = GetEnvironmentPanorama(N, roughness);
	}


	// ライトデプスの準備
	float4 LVPos = DepthTex.Sample(DepthSamLinear, input.Tex).zwyx;

	// 位置(view座標系)
	float dep = LVPos.w;
	dep = ToPerspectiveDepth(dep);
	float3 vpos = DepthToPosition2(input.Tex, dep);


	//float spec = LightingFuncGGX_REF(N, -normalize(vpos), -N, roughness, 0.1);
	//spec = 0;

	float3 envSpec = GetEnvironmentPanoramaSpec(normalize(vpos), N, roughness);


	//const float PI = 3.14159265359;
	//Out.Diffuse.rgb = env * 1.0f / PI;
	Out.Diffuse.rgb = env;
	Out.Diffuse.a = 1;

	Out.Specular.rgb = envSpec;
	Out.Specular.a = 1;

	return Out;

}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
PS_OUTPUT_1 PS(PS_INPUT input)
{
	PS_OUTPUT_1 Out;
	Out = main(input, 1);

	float rebirth = SpecularTex.Sample(SpecularSamLinear, input.Tex).a;

	//[branch]
	if (rebirth > 0.1){
		PS_OUTPUT_1 reb;
		reb = main(input, -1);
		Out.Diffuse.rgb = ((1 - rebirth) * Out.Diffuse.rgb) + reb.Diffuse.rgb * rebirth;
		Out.Specular.rgb = ((1 - rebirth) * Out.Specular.rgb) + reb.Specular.rgb * rebirth;
	}
	else{

	}

	return Out;
}
