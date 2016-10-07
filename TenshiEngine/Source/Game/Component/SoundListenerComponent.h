#pragma once

#include "IComponent.h"
#include "MySTL/ptr.h"

class IDirectSound3DListener;

class SoundListenerComponent :public Component {
public:
	SoundListenerComponent();
	~SoundListenerComponent();

	void Initialize() override;
	void Start() override;
#ifdef _ENGINE_MODE
	void EngineUpdate() override;
#endif
	void Update() override;
	void Finish() override;

#ifdef _ENGINE_MODE
	void CreateInspector() override;
#endif

	void IO_Data(I_ioHelper* io) override;
private:

	IDirectSound3DListener *mListener;
};