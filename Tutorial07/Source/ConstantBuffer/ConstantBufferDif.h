#pragma once

#include "XNAMath/xnamath.h"

struct CBNeverChanges
{
	XMMATRIX mView;
};

struct CBChangeOnResize
{
	XMMATRIX mProjection;
};

struct CBChangesEveryFrame
{
	XMMATRIX mWorld;
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
};
struct cbChangesUseTexture
{
	XMFLOAT4 UseTexture;
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
