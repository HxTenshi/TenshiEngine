#pragma once
#include "MySTL/ptr.h"
#include "Game/Script/GameObject.h"
class PhysXColliderComponent;

#define _LAYER_NUM 13
struct  Layer
{
	enum  Enum
	{
		None = (1 << 0),
		UserTag1 = (1 << 1),
		UserTag2 = (1 << 2),
		UserTag3 = (1 << 3),
		UserTag4 = (1 << 4),
		UserTag5 = (1 << 5),
		UserTag6 = (1 << 6),
		UserTag7 = (1 << 7),
		UserTag8 = (1 << 8),
		UserTag9 = (1 << 9),
		UserTag10 = (1 << 10),
		UserTag11 = (1 << 11),
		UserTag12 = (1 << 12),
		ALL = 0xFFFFFFFF,
	};
};

struct RaycastHit{
	Actor* hit;
	XMVECTOR position;
	XMVECTOR normal;
};

class PhysXEngine{
public:
	PhysXEngine(){}
	virtual ~PhysXEngine(){}

	virtual Actor* Raycast(const XMVECTOR& pos, const XMVECTOR& dir, float distance, Layer::Enum layer = Layer::ALL) = 0;
	virtual bool RaycastHit(const XMVECTOR& pos, const XMVECTOR& dir, float distance, RaycastHit* result, Layer::Enum layer = Layer::ALL) = 0;
	virtual int OverlapHitMultiple(weak_ptr<PhysXColliderComponent> collder, const std::function<void(GameObject)>& collback, Layer::Enum layer = Layer::ALL) = 0;

};