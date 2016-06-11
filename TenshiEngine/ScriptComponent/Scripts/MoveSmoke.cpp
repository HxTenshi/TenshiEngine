#include "MoveSmoke.h"
#include "../h_standard.h"
#include "../h_component.h"
#include <cmath>
#include <algorithm>
#include <math.h>

//生成時に呼ばれます（エディター中も呼ばれます）
void MoveSmoke::Initialize(){

}

//initializeとupdateの前に呼ばれます（エディター中も呼ばれます）
void MoveSmoke::Start(){

}

//毎フレーム呼ばれます
void MoveSmoke::Update(){

	auto com = gameObject->GetComponent<ParticleComponent>();
	if (!com)return;
	if (mMaxSpeed == 0.0)return;
	//auto mStartParam = com->mCParticleBuffer.mParam;
	com->mParticleParam .Vector.y = 0.2f * (mSpeed / mMaxSpeed);
	com->mParticleParam.Param.x = 128 * (mSpeed / mMaxSpeed);
	com->mParticleParam.MinMaxScale.x = 0.5 *(mSpeed / mMaxSpeed) + 0.5;
	com->mParticleParam.MinMaxScale.y = 1.5 *(mSpeed / mMaxSpeed) + 0.5;

	
}

//開放時に呼ばれます（Initialize１回に対してFinish１回呼ばれます）（エディター中も呼ばれます）
void MoveSmoke::Finish(){

}

//コライダーとのヒット時に呼ばれます
void MoveSmoke::OnCollideBegin(Actor* target){
	(void)target;
}

//コライダーとのヒット中に呼ばれます
void MoveSmoke::OnCollideEnter(Actor* target){
	(void)target;
}

//コライダーとのロスト時に呼ばれます
void MoveSmoke::OnCollideExit(Actor* target){
	(void)target;
}