#include "BoneMirrorComponent.h"
#include "Game/Actor.h"
#include "Game/Script/GameObject.h"
#include "ModelComponent.h"
#include "Graphic/Model/Model.h"
#include "Graphic/Model/BoneModel.h"
#include "TransformComponent.h"

BoneMirrorComponent::BoneMirrorComponent()
{
	m_TargetBone = NULL;
	m_ModelComponent = NULL;
	m_TargetBoneID = -1;
	m_LoadNamesInitialize = false;
}

BoneMirrorComponent::~BoneMirrorComponent()
{
}

void BoneMirrorComponent::Initialize()
{
	m_ModelComponent = NULL;
	m_LoadNamesInitialize = false;
	ChangeTargetBone(m_TargetBone);
	SetTargetBoneID(m_TargetBoneID);
}

void BoneMirrorComponent::Start()
{
}

void BoneMirrorComponent::Finish()
{
	m_TargetBone = NULL;
	m_ModelComponent = NULL;
	m_TargetBoneID = -1;
	m_BoneNames.clear();
	m_LoadNamesInitialize = false;
}

#ifdef _ENGINE_MODE
void BoneMirrorComponent::EngineUpdate()
{
}
#endif

void BoneMirrorComponent::Update()
{

	if (!m_TargetBone)return;
	if (!m_ModelComponent) {
		m_ModelComponent = m_TargetBone->GetComponent<ModelComponent>();
	}
	if (m_ModelComponent && m_ModelComponent->mModel && m_ModelComponent->mModel->mBoneModel) {

		auto bone = m_ModelComponent->mModel->mBoneModel;
		transform(bone);
	}
}

#include "Engine/Inspector.h"
#include "Window/Window.h"
#ifdef _ENGINE_MODE
void BoneMirrorComponent::CreateInspector()
{

	if (!m_LoadNamesInitialize) {
		if (m_ModelComponent && m_ModelComponent->mModel && m_ModelComponent->mModel->mBoneModel) {

			auto bone = m_ModelComponent->mModel->mBoneModel;
			m_BoneNames.resize(bone->GetBones().size());
			int i = 0;
			for (auto bone : bone->GetBones()) {
				m_BoneNames[i] = bone.mStrName;
				i++;
			}

			m_LoadNamesInitialize = true;
		}
	}
	Inspector ins("BoneMirror",this);
	ins.AddEnableButton(this);
	ins.Add("TargetBone", &m_TargetBone, [&]() {
		ChangeTargetBone(m_TargetBone);
		Window::ClearInspector();

	});
	ins.AddSelect("BoneID", &m_TargetBoneID, m_BoneNames, [&](int value) {
		SetTargetBoneID(value);
	});
	ins.Complete();
}
#endif

void BoneMirrorComponent::IO_Data(I_ioHelper * io)
{
	Enabled::IO_Data(io);

#define _KEY(x) io->func( x , #x)
#define _KEY_OBJ(x) ioGameObjectHelper::func( &x , #x, io, &gameObject)
	_KEY_OBJ(m_TargetBone);
	_KEY(m_TargetBoneID);

#undef _KEY
}

void BoneMirrorComponent::ChangeTargetBone(GameObject target)
{
	m_TargetBone = target;
	m_BoneNames.clear();
	m_LoadNamesInitialize = false;
	m_ModelComponent = NULL;

	if (m_TargetBone) {
		if (!m_ModelComponent) {
			m_ModelComponent = m_TargetBone->GetComponent<ModelComponent>();
		}
		if (m_ModelComponent && m_ModelComponent->mModel && m_ModelComponent->mModel->mBoneModel) {

			auto bone = m_ModelComponent->mModel->mBoneModel;
			m_BoneNames.resize(bone->GetBones().size());
			int i = 0;
			for (auto bone : bone->GetBones()) {
				m_BoneNames[i] = bone.mStrName;
				i++;
			}
			m_LoadNamesInitialize = true;

			if (bone->GetBones().size() <= m_TargetBoneID) {
				m_TargetBoneID = -1;
			}
			transform(bone);
		}
	}
}

std::vector<std::string>& BoneMirrorComponent::GetBoneNames()
{
	if (!m_LoadNamesInitialize) {
		if (m_ModelComponent && m_ModelComponent->mModel && m_ModelComponent->mModel->mBoneModel) {

			auto bone = m_ModelComponent->mModel->mBoneModel;
			m_BoneNames.resize(bone->GetBones().size());
			int i = 0;
			for (auto bone : bone->GetBones()) {
				m_BoneNames[i] = bone.mStrName;
				i++;
			}

			m_LoadNamesInitialize = true;
		}
	}
	return m_BoneNames;
}

int BoneMirrorComponent::GetTargetBoneID()
{
	return m_TargetBoneID;
}

void BoneMirrorComponent::SetTargetBoneID(int id)
{
	m_TargetBoneID = id;

	if (m_ModelComponent && m_ModelComponent->mModel && m_ModelComponent->mModel->mBoneModel) {

		auto bone = m_ModelComponent->mModel->mBoneModel;

		if (bone->GetBones().size() <= m_TargetBoneID) {
			m_TargetBoneID = -1;
		}
		transform(bone);

	}
}

void BoneMirrorComponent::transform(BoneModel * bone)
{
	if (m_TargetBoneID != -1 && bone->GetBones().size() > m_TargetBoneID && m_TargetBone) {
		auto s = gameObject->mTransform->WorldScale();
		auto m = XMMatrixMultiply(bone->GetBones()[m_TargetBoneID].mMtxPose, m_TargetBone->mTransform->GetMatrix());
		float w[] = { m.r[0].w,m.r[1].w, m.r[2].w };
		m.r[0] = XMVector3Normalize(m.r[0]) * s.x;
		m.r[1] = XMVector3Normalize(m.r[1]) * s.y;
		m.r[2] = XMVector3Normalize(m.r[2]) * s.z;
		m.r[0].w = w[0];
		m.r[1].w = w[1];
		m.r[2].w = w[2];
		gameObject->mTransform->WorldMatrix(m);
	}
}
