#pragma once

struct RaycastHit{
	Actor* hit;
	XMVECTOR position;
	XMVECTOR normal;
};

class PhysXEngine{
public:
	PhysXEngine(){}
	virtual ~PhysXEngine(){}

	virtual Actor* Raycast(const XMVECTOR& pos, const XMVECTOR& dir, float distance) = 0;
	virtual bool RaycastHit(const XMVECTOR& pos, const XMVECTOR& dir, float distance, RaycastHit* result) = 0;

};