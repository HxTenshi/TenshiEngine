#pragma once

class PhysXEngine{
public:
	PhysXEngine(){}
	virtual ~PhysXEngine(){}

	virtual Actor* Raycast(const XMVECTOR& pos, const XMVECTOR& dir, float distance) = 0;

};