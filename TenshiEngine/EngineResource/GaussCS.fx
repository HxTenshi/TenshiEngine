
// 入力テクスチャ
Texture2D    rInputTex : register(t0);

// 出力テクスチャ
RWTexture2D<float4>  rwOutputTex : register(u0);

cbuffer cbTextureSize : register(b0)
{
	uint4 TextureSize;
};
cbuffer cbHDRBloomParam : register(b1)
{
	//x=光の伸び pow(p, X ), y=十字の伸び(重み) bulr+center * X, z=十字の伸び(比率) lerp(center ,bulr, X ), w=十字の発生位置 pow(center, X );
	float4 BloomParam;
};


#define MaxWidth 768

// 共有メモリ
groupshared float4 shPixel[MaxWidth];

#define BuraPow (16)

void Gauss(uint x, uint y, int TNum){


	uint maxX = TextureSize.x-1;


	uint2 basePos = uint2(x, y);
	uint2 outPos = uint2(y, x);
	uint index0 = x;

	// 入力テクスチャからピクセルを参照する
	shPixel[index0] = (x < maxX) ? rInputTex[basePos] : float4(0, 0, 0, 1);

	// すべてのグループ内スレッドはここで同期をとる
	GroupMemoryBarrierWithGroupSync();

	if (x >= maxX)return;

	float4 col = float4(0, 0, 0, 0);

	// 自身のピクセルを取得する
	float count = 0;

	float SamplePow = BloomParam.x;

	[unroll]
	for (int i = -BuraPow; i < BuraPow; i++){
		int idx = min(max(i + index0, 0), maxX);

		//サンプリング数を多く見せかけてから減らす
		float sampNum = BuraPow + 1.0;
		float p = (float)abs(i);
		float powor = (sampNum - p) / sampNum;

		col += shPixel[idx] * pow(powor, SamplePow);
		count += powor;
	}
	col /= count;

	//float texY = (float)TextureSize.x;
	//float souY = (float)x;
	//souY *= 2;
	//souY = texY - souY;
	//float crossPowor = 1.0f - (abs(souY) / texY) + 0.0001f;
	//crossPowor = pow(crossPowor, BloomParam.w)/2;
	//crossPowor *= BloomParam.y;
	//col *= 1 - crossPowor;
	//col += shPixel[index0] * crossPowor;
	col.w = 1;

	rwOutputTex[outPos] = col;
}


[numthreads(768, 1, 1)]
void CS_768(uint3 gtd : SV_GroupThreadID, uint3 gid : SV_GroupID)
{ 
	Gauss(gtd.x, gid.x, 768);
}


[numthreads(512, 1, 1)]
void CS_512(uint3 gtd : SV_GroupThreadID, uint3 gid : SV_GroupID)
{
	Gauss(gtd.x, gid.x, 512);
}

[numthreads(256, 1, 1)]
void CS_256(uint3 gtd : SV_GroupThreadID, uint3 gid : SV_GroupID)
{
	Gauss(gtd.x, gid.x, 256);
}


[numthreads(128, 1, 1)]
void CS_128(uint3 gtd : SV_GroupThreadID, uint3 gid : SV_GroupID)
{
	Gauss(gtd.x,gid.x,128);

}

[numthreads(64, 1, 1)]
void CS_64(uint3 gtd : SV_GroupThreadID, uint3 gid : SV_GroupID)
{
	Gauss(gtd.x, gid.x, 64);

}
[numthreads(32, 1, 1)]
void CS_32(uint3 gtd : SV_GroupThreadID, uint3 gid : SV_GroupID)
{
	Gauss(gtd.x, gid.x, 32);

}

[numthreads(16, 1, 1)]
void CS_16(uint3 gtd : SV_GroupThreadID, uint3 gid : SV_GroupID)
{
	Gauss(gtd.x, gid.x, 16);

}

