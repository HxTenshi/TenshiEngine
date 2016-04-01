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
Texture2D SpecularTex : register(t1);
SamplerState SpecularSamLinear : register(s1);
Texture2D NormalTex : register(t2);
SamplerState NormalSamLinear : register(s2);
Texture2D LightTex : register(t3);
SamplerState LightSamLinear : register(s3);
Texture2D LightSpeTex : register(t4);
SamplerState LightSpeSamLinear : register(s4);
Texture2D EnvironmentTex : register(t5);
SamplerState EnvironmentSamLinear : register(s5);
Texture2D EnvironmentRTex : register(t6);
SamplerState EnvironmentRSamLinear : register(s6);
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
cbuffer cbChangesLight : register(b3)
{
	float4 LightVect;
	float4 LightColor;
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

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(PS_INPUT input) : SV_Target
{
	float4 albedo = AlbedoTex.Sample(AlbedoSamLinear, input.Tex);
	float4 dif = LightTex.Sample(LightSamLinear, input.Tex);
	float4 lSpe = LightSpeTex.Sample(LightSpeSamLinear, input.Tex);

	float4 norcol = NormalTex.Sample(NormalSamLinear, input.Tex);
	float3 nor = norcol.xyz * 2 - 1.0;
	float4 spc = SpecularTex.Sample(SpecularSamLinear, input.Tex);
	float spcPow = spc.a;
	float3 ray = spc.xyz * 2 - 1.0;
		//ray.z = ray.z;
		float3 ref = reflect(nor, ray);       // ”½ËƒxƒNƒgƒ‹
		//ref.xy = ref.xy * ref.z;

		//ref.xy *= abs(ref.z) + 1;
		//ref = normalize(ref);
		ref.xy = ref.xy*float2(0.5,-0.5) + 0.5;
		//ref.xy *= 0.9;
		//ref.xy = saturate(ref.xy);
		//if (ref.y >= 1){
		//	ref.y = ref.y * -1;
		//}
		//else if (ref.y <= 0){
		//	ref.y = ref.y * -1;
		//}
		//ref = normalize(ref);



	float roughness = norcol.a;
	float3 env = EnvironmentTex.Sample(EnvironmentSamLinear, ref.xy).rgb;
	float3 envR = EnvironmentRTex.Sample(EnvironmentRSamLinear, ref.xy).rgb;
	env = lerp(env, envR, pow(roughness,0.5f));



	//float s = LightingFuncGGX_REF(nor, -ray, -LightVect.xyz, roughness, 1);


	float4 col = albedo * dif;
	//col.rgb += env * spcPow;
	col.rgb = lerp(col.rgb, env, spcPow);
	col.rgb += lSpe.rgb * spcPow;

	//float l = length( -ray - nor);
	//l = abs(l);
	//col = float4(l,l,l, 1);

	return saturate(col);
}
