#pragma once

#include "MySTL/File.h"
#include "IComponent.h"

namespace physx{
	class PxController;
}

class ICharacterControllerComponent :public Component{
public:
	ICharacterControllerComponent(){}
	virtual ~ICharacterControllerComponent(){}

	virtual void Move(const XMVECTOR& velocity) = 0;
	virtual void Teleport(const XMVECTOR& position) = 0;
	virtual bool IsGround() = 0;
	virtual void SetSlopeLimit(float degree) = 0;
	virtual void SetStepOffset(float offset) = 0;
	virtual void SetSize(float size) = 0;
private:
};


class CharacterControllerComponent :public ICharacterControllerComponent{
public:
	CharacterControllerComponent();
	~CharacterControllerComponent();
	void Initialize() override;
	void Start() override;
	void Finish() override;

	void EngineUpdate() override;
	void Update() override;

#ifdef _ENGINE_MODE
	void CreateInspector() override;
#endif
	void IO_Data(I_ioHelper* io) override;

	void Move(const XMVECTOR& velocity) override;
	void Teleport(const XMVECTOR& position) override;
	bool IsGround() override;
	void SetSlopeLimit(float degree) override;
	void SetStepOffset(float offset) override;
	void SetSize(float size) override;
private:
	physx::PxController* mController;
	float mSlopeLimitDigree;
	float mStepOffset;
	float mSize;
	//XMVECTOR mGravity;
	//XMVECTOR mJump;
};

