#pragma once

#include "IModelComponent.h"

class Model;

class ModelComponent :public IModelComponent{
public:
	ModelComponent();
	~ModelComponent();
	void Initialize() override;
	void Update() override;

	void SetMatrix() override;

	void CreateInspector() override;

	void IO_Data(I_ioHelper* io) override;

	std::string mFileName;
	std::string mBoneFileName;

};
