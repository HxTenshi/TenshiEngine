#pragma once

#include "IComponent.h"

class ITextComponent :public Component{
public:
	ITextComponent(){}
	virtual ~ITextComponent(){}
	virtual void ChangeText(const std::string& text) = 0;
private:
	ITextComponent(const ITextComponent&);
	void operator =(const ITextComponent&);
};

class TextComponentMember;

class TextComponent : public ITextComponent{
public:
	TextComponent();
	~TextComponent();

	void Initialize() override;

	void EngineUpdate() override;
	void Update() override;
	void Finish() override;

	void DrawTextUI();

#ifdef _ENGINE_MODE
	void CreateInspector() override;
#endif

	void IO_Data(I_ioHelper* io) override;
	void ChangeText(const std::string& text) override;
	void ChangeFontSize(float size);
	void ChangeCenter(bool center);

private:
	TextComponent(const TextComponent&);
	void operator =(const TextComponent&);

	float mFontSize;
	bool mCenter;

	XMVECTOR mBackScale;

	TextComponentMember* impl;
};