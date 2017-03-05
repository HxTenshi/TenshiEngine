
#include "MeshDrawComponent.h"

#include "Window/Window.h"
#include "Game/Actor.h"
#include "ModelComponent.h"
#include "TextureModelComponent.h"
#include "MaterialComponent.h"

#include "Graphic/Model/Model.h"

#include "Game/Game.h"

#include "Game/RenderingSystem.h"
#include "Engine/Inspector.h"
#include "Game/Component/CameraComponent.h"

MeshDrawComponent::MeshDrawComponent(){
}

#ifdef _ENGINE_MODE
void MeshDrawComponent::EngineUpdate(){
	Update();
}
#endif
void MeshDrawComponent::Update(){

	if (!mModel){
		mModel = gameObject->GetComponent<ModelComponent>();
		//if (!mModel)
		//	mModel = gameObject->GetComponent<TextureModelComponent>();
		if (!mModel)return;
	}
	if (!mModel->mModel)return;

	if (!mMaterial){
		mMaterial = gameObject->GetComponent<MaterialComponent>();
	}
	if (!mMaterial)return;
	Game::AddDrawList(DrawStage::Init, std::function<void()>([&](){
		mModel->SetMatrix();
	}));

	auto drawfanc = std::function<void()>([&]() {
		Model& model = *mModel->mModel;

		//if (!Input::Down(KeyCode::Key_M)) {
		//	if (auto cam = Game::GetMainCamera()) {
		//		if (!cam->Looking(model.GetBoundingSphere(), gameObject)) {
		//			//Window::AddLog("Œ©‚¦‚Ä‚È‚¢‚æ:"+gameObject->Name());
		//			return;
		//		}
		//	}
		//}

		auto& meshvec = mModel->GetMeshComVector();

		auto render = RenderingEngine::GetEngine(ContextType::MainDeferrd);

		if (meshvec.size() > 0) {
			model.Draw(render->m_Context, meshvec);
		}
		else {
			model.Draw(render->m_Context, mMaterial);
		}


	});

	bool alf = mMaterial->GetUseAlpha();
	if (alf) {
		Game::AddDrawListZ(DrawStage::Forward, gameObject, drawfanc);
	}
	else {
		Game::AddDrawList(DrawStage::Diffuse, drawfanc);
	}

}
#ifdef _ENGINE_MODE
void MeshDrawComponent::CreateInspector(){
	
	Inspector ins("MeshRender", this);
	ins.AddEnableButton(this);
	ins.Complete();
}
#endif

void MeshDrawComponent::IO_Data(I_ioHelper* io){
	
	Enabled::IO_Data(io);

#define _KEY(x) io->func( x , #x)

#undef _KEY
}