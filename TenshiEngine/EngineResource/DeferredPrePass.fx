
//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
Texture2D AlbedoTex : register( t0 );
SamplerState AlbedoSamLinear : register(s0);
Texture2D NormalTex : register(t1);
SamplerState NormalSamLinear : register(s1);
Texture2D HeightTex : register(t2);
SamplerState HeightSamLinear : register(s2);
Texture2D SpcTex : register(t3);
SamplerState SpcSamLinear : register(s3);
Texture2D RoughTex : register(t4);
SamplerState RoughSamLinear : register(s4);
Texture2D EmissiveTex : register(t5);
SamplerState EmissiveSamLinear : register(s5);

//Texture2D EnvironmentTex : register(t6);
//SamplerState EnvironmentSamLinear : register(s6);
Texture2D EnvironmentRTex : register(t7);
SamplerState EnvironmentRSamLinear : register(s7);

Texture2D EnvironmentTex : register(t6);
SamplerState EnvironmentSamLinear : register(s6);

TextureCube EnvironmentTexCube : register(t6);
SamplerState EnvironmentCubeSamLinear : register(s6);

cbuffer cbNeverChanges : register( b0 )
{
	matrix View;
	matrix ViewInv;
};

cbuffer cbChangeOnResize : register( b1 )
{
	matrix Projection;
	matrix ProjectionInv;
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
	float4 MNormaleScale;
	float2 MOffset;
	float EmissivePowor;
	float MNull;
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
	float4 UseTexture2;
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
	float3 Normal	: NORMAL;
	float3 Bin	: BINORMAL;
	float3 Tan	: TANGENT;
	float2 Tex		: TEXCOORD0;
	uint4 BoneIdx		: BONEINDEX;
	float4 BoneWeight	: BONEWEIGHT;
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

float4x3 getBoneMatrix(uint idx)
{
	return BoneMatrix[idx];
}

//#define GAMMA 2.2
#define GAMMA 1


//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS( VS_INPUT input )
{
	PS_INPUT output = (PS_INPUT)0;
	output.Pos = mul(input.Pos, World);

	output.LPos[0] = mul(output.Pos, LViewProjection[0]);
	output.LPos[1] = mul(output.Pos, LViewProjection[1]);
	output.LPos[2] = mul(output.Pos, LViewProjection[2]);
	output.LPos[3] = mul(output.Pos, LViewProjection[3]);


	output.VPos = mul( output.Pos, View );
	output.Pos = mul( output.VPos, Projection);

	output.BefPos = mul(input.Pos, BeforeWorld);
	output.BefPos = mul(output.BefPos, View);

	//output.Normal = input.Normal;
	//output.Bin = input.Bin;
	//output.Tan = input.Tan;

	float3x3 WV = mul((float3x3)World, (float3x3)View);

	output.Normal = mul(input.Normal, WV);
	output.Bin = mul(input.Bin, WV);
	output.Tan = mul(input.Tan, WV);

	output.Tex = input.Tex * MTexScale + MOffset;
	
	return output;
}


//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VSSkin(VS_INPUT input)
{

	float4 pos = input.Pos;
	float3 nor = input.Normal;
	float3 bin = input.Bin;
	float3 tan = input.Tan;
	PS_INPUT output = (PS_INPUT)0;


	float3 bpos = float3(0, 0, 0);
	float3 bnor = float3(0, 0, 0);
	float3 bbin = float3(0, 0, 0);
	float3 btan = float3(0, 0, 0);
	float total = input.BoneWeight[0] + input.BoneWeight[1] + input.BoneWeight[2] + input.BoneWeight[3];
	if (total < 0.0001){
		float4x3 bm = getBoneMatrix(input.BoneIdx[0]);
		bpos = mul(pos, bm).xyz;
		bnor = mul(nor, (float3x3)bm);
		bbin = mul(bin, (float3x3)bm);
		btan = mul(tan, (float3x3)bm);
	}
	else{
		[unroll]
		for (uint i = 0; i < 4; i++){
			if (input.BoneWeight[i] < 0.0001)continue;
			float4x3 bm = getBoneMatrix(input.BoneIdx[i]);
			bpos += input.BoneWeight[i] * mul(pos, bm).xyz;
			bnor += input.BoneWeight[i] * mul(nor, (float3x3)bm);
			bbin += input.BoneWeight[i] * mul(bin, (float3x3)bm);
			btan += input.BoneWeight[i] * mul(tan, (float3x3)bm);
		}
	}


	//pos.xyz = bpos / 100.0;
	//output.Normal = bnor / 100.0;
	//output.Bin = bbin / 100.0;
	//output.Tan = btan / 100.0;
	pos.xyz = bpos;

	float3x3 WV = mul((float3x3)World, (float3x3)View);

	output.Normal = mul(bnor, WV);
	output.Bin = mul(bbin, WV);
	output.Tan = mul(btan, WV);

	output.Pos = mul(pos, World);

	output.LPos[0] = mul(output.Pos, LViewProjection[0]);
	output.LPos[1] = mul(output.Pos, LViewProjection[1]);
	output.LPos[2] = mul(output.Pos, LViewProjection[2]);
	output.LPos[3] = mul(output.Pos, LViewProjection[3]);

	output.VPos = mul(output.Pos, View);
	output.Pos = mul(output.VPos, Projection);

	output.BefPos = mul(pos, BeforeWorld);
	output.BefPos = mul(output.BefPos, View);

	output.Tex = input.Tex * MTexScale + MOffset;

	return output;
}
float3 GetEnvironmentSphere(float3 ray, float3 N, float roughness){
	float3 ref = reflect(ray, N);       // ���˃x�N�g��
	ref.xy = ref.xy*float2(0.5, -0.5) + 0.5;
	return pow(EnvironmentTex.SampleLevel(EnvironmentSamLinear, ref.xy, roughness * 10.0f).rgb, GAMMA);
}

float3 GetEnvironmentCube(float3 ray, float3 N, float roughness){
	float3 ref = reflect(ray, N);       // ���˃x�N�g��
	float3 WorldRef = mul(ref, (float3x3)ViewInv);
	return pow(EnvironmentTexCube.SampleLevel(EnvironmentCubeSamLinear, normalize(WorldRef), roughness * 10.0f).rgb, GAMMA);
}

float3 GetEnvironmentPanorama(float3 ray, float3 N, float roughness){
	float3 ref = reflect(ray, N);       // ���˃x�N�g��
	ref = mul(ref, (float3x3)ViewInv);

	const float PI = 3.14159265359;
	const float TwoPI = PI + PI;
	float phi = acos(ref.y);
	float theta = atan2(-1.0 * ref.x, ref.z) + PI;
	float2 tex = float2(theta / TwoPI, phi / PI);

		return pow(EnvironmentTex.SampleLevel(EnvironmentSamLinear, tex.xy, roughness * 10.0f).rgb, GAMMA);
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

	const int Count = 8;
	const int HightPowerDiv = Count * 100;
	[unroll]
	for( int i = 0; i < Count; i++ ) {
		if( UseTexture.z != 0.0 ) {

			const float3x3 TangentMat = float3x3(normalize(input.Tan),
				normalize(input.Bin),
				normalize(input.Normal));
			//TangentMat = transpose(TangentMat);
			const float3 TangentRay = mul(TangentMat, ray);

			const float pow = HeightTex.Sample(HeightSamLinear, texcood).r;
			const float2 h = TangentRay.xy * pow * (MHightPower.x / HightPowerDiv);
			//i.uv += ray * height.r * _HeightFactor
			//texcood += h;
			texcood += h * float2(-1, 1);
		}
	}


	float4 DifColor = float4(1.0, 1.0, 1.0, 1.0);
		if (UseTexture.x != 0.0){
			DifColor = pow(AlbedoTex.Sample(AlbedoSamLinear, texcood), GAMMA);
			//DifColor = AlbedoTex.SampleLevel(AlbedoSamLinear, texcood,0);
		}
	if (DifColor.a < 0.01)discard;


	// �@���̏���
	float3 N;

	//[branch]
	//if (UseTexture.y != 0.0){
	//	float3 bump = NormalTex.Sample(NormalSamLinear, texcood).rgb * 2 - 1.0;
	//	bump *= MNormaleScale.xyz;
	//
	//	//�����x�N�g���𒸓_���W�n�ɕϊ�����
	//	float3 n;
	//	n.x = dot(bump, input.Tan);
	//	n.y = dot(bump, input.Bin);
	//	n.z = dot(bump, input.Normal);
	//	N = n;
	//}else{
	//	N = input.Normal;
	//}
	//
	//N = mul(N, (float3x3)World);
	//N = mul(N, (float3x3)View);
	//N = normalize(N);

	[branch]
	if (UseTexture.y != 0.0){
		float3 bump = NormalTex.Sample(NormalSamLinear, texcood).rgb * 2 - 1.0;
		bump *= MNormaleScale.xyz;

		//�����x�N�g���𒸓_���W�n�ɕϊ�����
		const float3x3 normat = float3x3(normalize(input.Tan),
		normalize(input.Bin),
		normalize(input.Normal));
		N = mul(bump, normat);
	}
	else{
		N = input.Normal;
	}

	N = normalize(N);

	float D = input.VPos.z / Far;

	float3 LD;

	float dist = input.VPos.z + input.VPos.z*0.0001 + 0.01;

	float3 LD_normal = float3(2.0, 2.0, Far);
	[branch]
	if (dist < SplitPosition.x)
	{
		input.LPos[0].xy += 1.0;
		LD = input.LPos[0].xyz / LD_normal;
	}
	else if (dist < SplitPosition.y)
	{
		input.LPos[1].xy += 1.0;
		LD = input.LPos[1].xyz / LD_normal;
	}
	else if (dist < SplitPosition.z)
	{
		input.LPos[2].xy += 1.0;
		LD = input.LPos[2].xyz / LD_normal;
	}
	else
	{
		input.LPos[3].xy += 1.0;
		LD = input.LPos[3].xyz / LD_normal;
	}
	

	//float2 velocity;
	//{
	//	float2 b = input.BefPos.xy / input.BefPos.z;
	//	float2 p = input.VPos.xy / input.VPos.z;
	//	velocity = p - b;
	//	//velocity = normalize(velocity);
	//	velocity = velocity * 0.5 + 0.5;
	//}

	float4 SpcColor = float4(1.0, 1.0, 1.0, 1.0);
		if (UseTexture.w != 0.0)
			SpcColor = pow(SpcTex.Sample(SpcSamLinear, texcood), GAMMA);
	SpcColor.rgb = SpcColor.r * MSpecular.rgb;

	float4 RghColor = float4(1.0, 1.0, 1.0, 1.0);
		if (UseTexture2.x != 0.0)
			RghColor = pow(RoughTex.Sample(RoughSamLinear, texcood), GAMMA);
	RghColor.r *= MSpecular.a;
	RghColor.r = max(RghColor.r, 0.01);


	//float3 env;
	//{
	//	env = GetEnvironmentPanorama(ray, N, RghColor.r);
	//	env *= SpcColor.rgb;
	//}
	float4 emi;
	if (UseTexture2.y != 0.0){
		emi = pow(EmissiveTex.Sample(EmissiveSamLinear, texcood), GAMMA);
		emi.rgb *= EmissivePowor;
		emi.a = -1.0f;
	}
	
	N = N * 0.5 + 0.5;

	DifColor.rgb *= 1-SpcColor.rgb;

	Out.ColorAlbedo = DifColor * MDiffuse * MHightPower.y;
	if (UseTexture2.y != 0.0){
		Out.ColorSpecular = emi;
	}
	else{
		//Out.ColorSpecular = float4(env.rgb, MAmbient.a);
		Out.ColorSpecular = float4(SpcColor.rgb, MAmbient.a);
	}
	Out.ColorNormal = float4(N, 1+RghColor.r);
	Out.ColorDepth = float4(D, 1 - LD.z, LD.x, 1.0 - LD.y);
	Out.ColorVelocity = float4(0.5,0.5,0,1);//float4(velocity.x, velocity.y, 0, 1);
	return Out;
}
