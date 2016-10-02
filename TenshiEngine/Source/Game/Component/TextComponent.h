#pragma once

#include "IComponent.h"

class ITextComponent :public Component{
public:
	ITextComponent(){}
	virtual ~ITextComponent(){}
	virtual void ChangeText(const std::string& text) = 0;
	virtual void ChangeFontSize(float size) = 0;
	virtual void ChangeCenter(bool center) = 0;
	virtual void SetTextureCenter(const XMFLOAT2& center) = 0;
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
#ifdef _ENGINE_MODE
	void EngineUpdate() override;
#endif
	void Update() override;
	void Finish() override;

	void DrawTextUI();

#ifdef _ENGINE_MODE
	void CreateInspector() override;
#endif

	void IO_Data(I_ioHelper* io) override;
	void ChangeText(const std::string& text) override;
	void ChangeFontSize(float size) override;
	void ChangeCenter(bool center) override;

	void SetTextureCenter(const XMFLOAT2& center) override;

private:
	TextComponent(const TextComponent&);
	void operator =(const TextComponent&);

	float mFontSize;
	bool mCenter;

	XMVECTOR mBackScale;
	XMFLOAT2 m_Center;

	TextComponentMember* impl;
};