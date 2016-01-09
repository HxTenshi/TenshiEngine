#pragma once

#include "IComponent.h"

class ITextComponent :public Component{
public:
	virtual ~ITextComponent(){}
	virtual void ChangeText(const std::string& text) = 0;
};

class TextComponentMember;

class TextComponent : public ITextComponent{
public:
	TextComponent();
	~TextComponent();

	void Initialize() override;

	void Update() override;

	void DrawTextUI();

	void CreateInspector() override;

	void IO_Data(I_ioHelper* io) override;
	void ChangeText(const std::string& text) override;
private:

	TextComponentMember* impl;
};