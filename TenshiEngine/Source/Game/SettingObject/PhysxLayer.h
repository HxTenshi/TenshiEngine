#pragma once

#include "Game/Actor.h"


class PhysX3Main;
class PhysxLayer :public Actor{
public:
	PhysxLayer();
	~PhysxLayer();

#ifdef _ENGINE_MODE
	void CreateInspector() override;
#endif

	void _ExportData(I_ioHelper* io, bool childExport = false) override;
	void _ImportData(I_ioHelper* io) override;

	std::vector<std::string>& GetSelects(){ return mSelects; };

private:
	void SetLayerFlag(int l1,int l2, bool f);

	std::vector<std::string> mSelects;
	std::unordered_map<int, bool> mCollideFiler;
};