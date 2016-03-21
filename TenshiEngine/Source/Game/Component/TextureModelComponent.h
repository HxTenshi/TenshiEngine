#pragma once

#include "IModelComponent.h"

class Model;

class TextureModelComponent : public IModelComponent{
public:
	TextureModelComponent();
	~TextureModelComponent();

	void Initialize() override;

	void SetMatrix() override;

	void CreateInspector() override;

	void IO_Data(I_ioHelper* io) override;
};