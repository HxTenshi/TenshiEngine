#pragma once


//16の倍数になるようにする

struct CBNeverChanges
{
	XMMATRIX mView;
	XMMATRIX mViewInv;
};

struct CBChangeOnResize
{
	XMMATRIX mProjection;
	XMMATRIX mProjectionInv;
};

struct CBChangesEveryFrame
{
	XMMATRIX mWorld;
	XMMATRIX mBeforeWorld;
};


struct cbChangesLight
{
	XMFLOAT4 LightVect;
	XMFLOAT4 LightColor;
};

struct cbChangesMaterial
{
	XMFLOAT4 Diffuse;
	XMFLOAT4 Specular;
	XMFLOAT4 Ambient;
	XMFLOAT2 TexScale;
	XMFLOAT2 HeightPower;
	XMFLOAT4 MNormaleScale;
	XMFLOAT2 MOffset;
	float EmissivePowor;
	float MNULL;
};
struct cbChangesUseTexture
{
	XMFLOAT4 UseTexture;
	XMFLOAT4 UseTexture2;
};
struct cbBoneMatrix
{
	//XMVECTOR
	XMFLOAT4 BoneMatrix[3];
};

struct CBBillboard
{
	XMMATRIX mBillboardMatrix;
};

struct cbChangesPointLight
{
	XMFLOAT4 ViewPosition;
	XMFLOAT4 Color;
	XMFLOAT4 Param;
};

struct cbChangesLightCamera
{
	XMMATRIX mLViewProjection[4];
	XMFLOAT4 mSplitPosition;
};

struct CBChangesPaticleParam
{
	//w=ランダムスピード
	XMFLOAT4 Vector;
	//w=[Sphere=0]~[Box=1]
	XMFLOAT4 Point;
	//w=空気抵抗
	XMFLOAT4 Rot;
	//x = min, y = max, z=反射力, w=摩擦減衰
	XMFLOAT4 MinMaxScale;
	//x = min, y = max, z=Z-Collision, w=VeloRot&Bura
	XMFLOAT4 Time;
	XMFLOAT4 G;
	//x = num, y=impact, z=pointG-Rot
	XMFLOAT4 Param;
	XMFLOAT4 Wind;
	XMFLOAT4 SmoothAlpha;
};

struct cbGameParameter//register(b11)
{
	//x=AllCount,y=DeltaTimeCount,z=DeltaTime
	XMFLOAT4 Time;
};

struct cbNearFar
{
	float Near;
	float Far;
	XMFLOAT2 NULLnf;
};

//13
struct cbScreen
{
	XMFLOAT2 ScreenSize;
	XMFLOAT2 NULLss;
};

//10
struct cbFreeParam
{
	XMFLOAT4 free;
};

//CS
struct cbTextureSize//register(b0)
{
	XMUINT4 TextureSize;
};

struct cbHDRBloomParam// : register(b1)
{
	//x=光の伸び pow(p, X ), y=十字の伸び(重み) lerp(bulr+center * X ,bulr, p ), z=十字の伸び(比率) lerp(bulr+center * p ,bulr, X ), w=十字の発生位置 pow(center, X );
	XMFLOAT4 BloomParam;
};
