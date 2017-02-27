#include "SoundListenerComponent.h"
#include "Game/Actor.h"
#include "Game/Component/TransformComponent.h"
#include "Sound/Sound.h"
#include "Engine/Inspector.h"

#include <dsound.h>

bool mIsCreate = false;

SoundListenerComponent::SoundListenerComponent()
{
	mListener = NULL;
	m_Rolloff = 1.0f;
}

SoundListenerComponent::~SoundListenerComponent()
{
}

void SoundListenerComponent::Initialize()
{
}

void SoundListenerComponent::Start()
{

	if (mIsCreate) {
		Window::AddLog(gameObject->Name()+"> 複数のサウンドリスナーが使用されています。");
		return;
	}


	//WAVEFORMATEX	wfex;
	//::ZeroMemory(&wfex, sizeof(WAVEFORMATEX));
	//wfex.nChannels = 1;
	//wfex.wFormatTag = WAVE_FORMAT_PCM;
	//wfex.wBitsPerSample = 16;
	//wfex.nBlockAlign = wfex.nChannels * wfex.wBitsPerSample / 8;
	//wfex.nSamplesPerSec = 44100;
	//wfex.nAvgBytesPerSec = wfex.nSamplesPerSec * wfex.nBlockAlign;
	//wfex.cbSize = 0;

	DSBUFFERDESC DSBufferDesc;
	DSBufferDesc.dwSize = sizeof(DSBUFFERDESC);

	DSBufferDesc.dwFlags = DSBCAPS_CTRL3D | DSBCAPS_PRIMARYBUFFER;
	DSBufferDesc.dwBufferBytes = 0;
	DSBufferDesc.dwReserved = 0;
	DSBufferDesc.lpwfxFormat = 0;
	DSBufferDesc.guid3DAlgorithm = GUID_NULL;

	IDirectSoundBuffer *ptmpBuf = 0;
	SoundDevice::GetDevice()->CreateSoundBuffer(&DSBufferDesc, &ptmpBuf, NULL);
	ptmpBuf->QueryInterface(IID_IDirectSound3DListener8, (void**)&mListener);
	ptmpBuf->Release();
	if (mListener == 0) {
		return;
	}


	auto pos = gameObject->mTransform->WorldPosition();
	auto up = XMVector3Normalize(gameObject->mTransform->Up());
	auto forward = XMVector3Normalize(gameObject->mTransform->Forward());
	mListener->SetPosition(pos.x, pos.y, pos.z, DS3D_DEFERRED);
	mListener->SetOrientation(forward.x, forward.y, forward.z, up.x, up.y, up.z, DS3D_DEFERRED);
	mListener->SetRolloffFactor(m_Rolloff, DS3D_DEFERRED);
	mListener->CommitDeferredSettings();

	//DS3DLISTENER lis;
	////memset(&lis, 0, sizeof(DS3DLISTENER));
	//lis.dwSize = sizeof(DS3DLISTENER);
	//lis.vPosition.x = 0.0f;
	//lis.vPosition.y = 0.0f;
	//lis.vPosition.z = 0.0f;
	//lis.vOrientFront.x = 0.0f;
	//lis.vOrientFront.y = 0.0f;
	//lis.vOrientFront.z = 1.0f;
	//lis.vOrientTop.x = 0.0f;
	//lis.vOrientTop.y = 1.0f;
	//lis.vOrientTop.z = 0.0f;
	//lis.flDistanceFactor = 1.0f;
	//lis.flRolloffFactor = DS3D_DEFAULTROLLOFFFACTOR;
	//lis.flDopplerFactor = DS3D_DEFAULTDOPPLERFACTOR;
	//auto hr = mListener->SetAllParameters(&lis, DS3D_IMMEDIATE);


	mIsCreate = true;

}

#ifdef _ENGINE_MODE
void SoundListenerComponent::EngineUpdate()
{
	Update();
}
#endif

void SoundListenerComponent::Update()
{

	if (!mListener)return;

	auto pos = gameObject->mTransform->WorldPosition();
	auto up = XMVector3Normalize(gameObject->mTransform->Up());
	auto forward = XMVector3Normalize(gameObject->mTransform->Forward());

	mListener->SetPosition(pos.x, pos.y, pos.z,DS3D_DEFERRED);
	mListener->SetOrientation(forward.x, forward.y, forward.z, up.x, up.y, up.z, DS3D_DEFERRED);



	mListener->CommitDeferredSettings();
}

void SoundListenerComponent::Finish()
{
	if (mListener) {
		mListener->Release();
		mListener = NULL;
		mIsCreate = false;
	}
}

#ifdef _ENGINE_MODE
void SoundListenerComponent::CreateInspector()
{
	Inspector ins("SoundListener", this);
	ins.AddEnableButton(this);

	static float dis = 1.0f;
	ins.Add("Rolloff", &m_Rolloff, [&](float value) {
		m_Rolloff = value;
		if (!mListener)return;
		//mListener->SetDistanceFactor(dis, DS3D_DEFERRED);
		mListener->SetRolloffFactor(m_Rolloff, DS3D_DEFERRED);
	});


	ins.Complete();
}
#endif

void SoundListenerComponent::IO_Data(I_ioHelper * io)
{
	//Enabled::IO_Data(io);

#define _KEY(x) io->func( x , #x)

	_KEY(m_Rolloff);

#undef _KEY
}
