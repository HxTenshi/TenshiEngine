#pragma once

#include "Game/Actor.h"


class PhysX3Main;
class PhysxLayer :public Actor{
public:
	PhysxLayer();
	~PhysxLayer();

	void SetPhysX(PhysX3Main *ptr){
		mPhysX3Main = ptr;
	}
#ifdef _ENGINE_MODE
	void CreateInspector() override;
#endif



	std::vector<std::string>& GetSelects(){ return mSelects; };

private:
	void SetLayerFlag(int l1,int l2, bool f);
	PhysX3Main *mPhysX3Main;

	bool _0x0;
	bool _0x1;
	bool _1x1;
	int i;
	std::vector<std::string> mSelects;
};