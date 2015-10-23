//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
Texture2D txDiffuse : register( t0 );
SamplerState samLinear : register(s0);

cbuffer cbScreen : register(b13)
{
	float2 ScreenSize;
	float2 NULLss;
};
cbuffer cbFreeParam : register(b10)
{
	float4 AddPow;
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

cbuffer cbChangesEveryFrame : register(b2)
{
	matrix World;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS( VS_INPUT input )
{
	PS_INPUT output = (PS_INPUT)0;
	output.Pos = mul(input.Pos, World);
	output.Pos.z = 0;
	float y = ScreenSize.y / ScreenSize.x;
	output.Tex = input.Tex *float2(1, y);
	
	return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(PS_INPUT input) : SV_Target
{
	float4 col = txDiffuse.Sample(samLinear, input.Tex) * AddPow;
	return col;
}
