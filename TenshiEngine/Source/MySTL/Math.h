#pragma once

struct BoundingBox {
	XMVECTOR Max = XMVectorZero();
	XMVECTOR Min = XMVectorZero();
	XMVECTOR GetCenter() {
		return (Max + Min) / 2.0f;
	}
	float GetLength() {
		return XMVector3Length(Max - Min).x;
	}
};
struct BoundingSphere {
	XMVECTOR Center = XMVectorZero();
	float Radius = 0.0f;
};