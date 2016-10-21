#pragma once

#include "MySTL/File.h"
#include "IComponent.h"

namespace physx{
	class PxController;
	class PxShape;
}

class ICharacterControllerComponent :public Component{
public:
	ICharacterControllerComponent(){}
	virtual ~ICharacterControllerComponent(){}

	virtual void Move(const XMVECTOR& velocity) = 0;
	virtual void Teleport(const XMVECTOR& position) = 0;
	virtual bool IsGround() = 0;
	virtual void SetSlopeLimit(float degree) = 0;
	virtual float GetSlopeLimit() const = 0;
	virtual void SetStepOffset(float offset) = 0;
	virtual float GetStepOffset() const = 0;
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

#ifdef _ENGINE_MODE
	void EngineUpdate() override;
#endif
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

	float GetSlopeLimit() const override { return mSlopeLimitDigree; }
	float GetStepOffset() const override { return mStepOffset; }

	void SetSize(float size) override;
private:
	void SetPhysxLayer(int layer);

	void OnEnabled()override;
	void OnDisabled()override;

	physx::PxController* mController;
	float mSlopeLimitDigree;
	float mStepOffset;
	float mSize;
	physx::PxShape* mShape;
	//XMVECTOR mGravity;
	//XMVECTOR mJump;
	friend Actor;
};

