
#include "ProfileViewer.h"

#include "Engine/Profiling.h"

#include "Game/Actor.h"
#include "MySTL/ptr.h"

#include "Graphic/material/material.h"


#include "Window/Window.h"
#include "Game/Component/TransformComponent.h"
#include "Game/Component/MaterialComponent.h"
#include "Game/Component/TextureModelComponent.h"
class ProfileBar : public Actor{
public:
	ProfileBar(const std::string& name,float y)
	{

		mTransform = mComponents.AddComponent<TransformComponent>();
		mComponents.AddComponent<MaterialComponent>();
		mComponents.AddComponent<TextureModelComponent>();

		Name(name);


		mTransform->Position(XMVectorSet(0, y, 0, 1));
		mTransform->Scale(XMVectorSet(0, 10, 0, 1));

	}
	~ProfileBar(){
		Finish();
	}

	void Init(const XMFLOAT4& color){
		Initialize();
		Start();
		auto mate = GetComponent<MaterialComponent>();
		Material material;
		material.SetAlbedo(color);
		material.Create();
		material.SetTexture("EngineResource/null4.png");
		mate->SetMaterial(0, material);
	}

	float ProfileBarUpdate(float x){
		auto pos = mTransform->Position();
		auto scale = mTransform->Scale();

		double time = Profiling::GetTime(mName);

		double Width = 1200.0;//(float)WindowState::mWidth;
		float sx = (float)((time/(1.0 / 60.0)) * (Width) * 0.5);

		mTransform->Position(XMVectorSet(x + sx/2, pos.y, 0, 1));
		mTransform->Scale(XMVectorSet(sx,scale.y, 1, 1));

		UpdateComponent();

		return sx;
	}
};

ProfileViewer::ProfileViewer()
{

	AddBar("Main:Update", 0, XMFLOAT4(0.4, 0.4, 0.1, 1));
	AddBar("Main:Draw", 0, XMFLOAT4(0.4, 0.1, 0.1, 1));
	AddBar("Main:etc", 0, XMFLOAT4(0.1, 0.3, 0.1, 1));
	AddBar("Main:wait", 0, XMFLOAT4(0.1, 0.1, 0.1, 1));

	AddBar("Draw:etc", 1, XMFLOAT4(0.1, 0.3, 0.1, 1));
	AddBar("Draw:execute", 1, XMFLOAT4(0.4, 0.4, 0.1, 1));
	AddBar("Draw:swap", 1, XMFLOAT4(0.4, 0.1, 0.1, 1));
	AddBar("Draw:wait", 1, XMFLOAT4(0.1, 0.1, 0.1, 1));
}
ProfileViewer::~ProfileViewer(){
	//for (int i = 0; i < 2; i++){
	//	for (auto* bar : mProfileBars[i]){
	//		delete bar;
	//	}
	//}
}
void ProfileViewer::AddBar(const std::string& name, int CPU, const XMFLOAT4& color){
	auto obj = shared_ptr<ProfileBar>(new ProfileBar(name, 5 + CPU * 12.0f));
	obj->Init(color);
	mProfileBars[CPU].push_back(obj);
}
void ProfileViewer::Update(){

	for (int i = 0; i < 2; i++){
		float x = 0;
		for (auto bar : mProfileBars[i]){
			x += bar->ProfileBarUpdate(x);
		}
	}
}