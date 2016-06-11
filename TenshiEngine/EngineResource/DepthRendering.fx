//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
Texture2D AlbedoTex : register(t0);
SamplerState AlbedoSamLinear : register(s0);

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
cbuffer cbChangesUseTexture : register(b6)
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

float4x3 getBoneMatrix(uint idx)
{
	return BoneMatrix[idx];
}


//--------------------------------------------------------------------------------------
struct VS_INPUT
{
	float4 Pos		: POSITION;
	float3 Normal	: NORMAL;
	float2 Tex		: TEXCOORD0;
	uint4 BoneIdx		: BONEINDEX;
	float4 BoneWeight	: BONEWEIGHT;
};

struct PS_INPUT
{
	float4 Pos		: SV_POSITION;
	float2 Tex		: TEXCOORD0;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS(float4 pos : POSITION, float2 tex : TEXCOORD0)
{

	PS_INPUT Out;

	pos = mul( pos, World );
	Out.Pos = mul(pos, LViewProjection[0]);
	Out.Tex = tex;
	return Out;
}

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VSSkin(VS_INPUT input)
{
	PS_INPUT Out;

	float4 bpos = float4(0, 0, 0, 1);
	float total = input.BoneWeight[0] + input.BoneWeight[1] + input.BoneWeight[2] + input.BoneWeight[3];
	[branch]
	if (total < 0.0001){
		float4x3 bm = getBoneMatrix(input.BoneIdx[0]);
			bpos.xyz = mul(input.Pos, bm).xyz;
	} else{
		[unroll]
		for (uint i = 0; i < 4; i++){
			[branch]
			if (input.BoneWeight[i] < 0.0001)continue;
			float4x3 bm = getBoneMatrix(input.BoneIdx[i]);
				bpos.xyz += input.BoneWeight[i] * mul(input.Pos, bm).xyz;
		}
	}

	bpos = mul(bpos, World);
	Out.Pos = mul(bpos, LViewProjection[0]);

	Out.Tex = input.Tex;

	return Out;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float PS(PS_INPUT input) : SV_Target
{
	if (UseTexture.x != 0.0){
		if (AlbedoTex.Sample(AlbedoSamLinear, input.Tex).a < 0.01)discard;
	}
	return 1 - input.Pos.z / Far;
}
