


#ifdef _ENGINE_MODE

#include "SelectActor.h"

#include <set>
#include <map>

#include "Device/DirectX11Device.h"

#include "Game/Actor.h"
#include "Game/Component/TransformComponent.h"
#include "Game/Component/MaterialComponent.h"
#include "Game/Component/ModelComponent.h"
#include "Game/Component/MeshDrawComponent.h"
#include "Game/Component/PhysXColliderComponent.h"
#include "Game/Component/CameraComponent.h"

#include "Window/Window.h"

#include "Input/Input.h"


#include "Graphic/Model/Model.h"

#include "Game/EngineObject/EditorCamera.h"
#include "PhysX/PhysX3.h"


#include "Game/Game.h"

#include "Game/RenderingSystem.h"

//選択時の移動オブジェクト
class Arrow : public Actor{
public:

	Arrow(){
		Name("arrow");
		mTransform = make_shared<TransformComponent>();
		mComponents.AddComponent<TransformComponent>(mTransform);
		auto model = make_shared<ModelComponent>();
		model->mFileName = "EngineResource/Arrow.pmx.tesmesh";
		mComponents.AddComponent<ModelComponent>(model);
		auto material = make_shared<MaterialComponent>();
		mComponents.AddComponent<MaterialComponent>(material);

		//mComponents.AddComponent<MeshDrawComponent>();

		mComponents.AddComponent<PhysXComponent>();
		auto phy = mComponents.GetComponent<PhysXComponent>();
		phy->mIsEngineMode = true;
		mComponents.AddComponent<PhysXColliderComponent>();
		auto col = mComponents.GetComponent<PhysXColliderComponent>();
		col->CreateMesh(std::string("EngineResource/Arrow.pmx.tesmesh"));
		col->SetScale(XMVectorSet(0.1, 0.1, 0.1, 1));

		phy->SetKinematic(true);
	}
	~Arrow(){
	}

	void Update(float deltaTime)override{
		(void)deltaTime;
	}
private:
};

Selects::Selects(){
	mLastVect = XMVectorZero();
}
Selects::~Selects(){
	CopyDelete();
}
void Selects::OneSelect(Actor* act){
	mSelects.clear();
	if (act){
		mSelects.push_back(act);
	}
}
bool Selects::TriggerSelect(Actor* act){
	//ヌルは無視
	if (!act)return false;
	int num = (int)mSelects.size();
	mSelects.remove(act);
	//削除されていれば
	if (num != (int)mSelects.size()){
		return false;
	}
	//削除されなければ追加
	mSelects.push_back(act);
	return true;
}
void Selects::MoveStart(XMVECTOR& vect){
	mLastVect = vect;
}
void Selects::MovePos(XMVECTOR& vect){

	auto move = vect - mLastVect;
	mLastVect = vect;

	for (auto& act : mSelects){
		auto pos = act->mTransform->Position();
		act->mTransform->Position(pos + move);
	}

}
void Selects::MoveEnd(XMVECTOR& vect){
	auto move = vect - mLastVect;
	mLastVect = vect;
	for (auto& act : mSelects){
		auto pos = act->mTransform->Position();
		act->mTransform->Position(pos + move);
		Game::SetUndo(act);
	}

}

void Selects::Copy(){
	CopyDelete();
	for (auto& act : mSelects){
		auto data = new picojson::value();
		act->ExportData(*data, true);

		mCopy.push_back(data);
	}
}
void Selects::CopyDelete(){
	for (auto act : mCopy){
		delete act;
	}
	mCopy.clear();
}
void Selects::Paste(){
	mSelects.clear();
	for (auto& act : mCopy){
		auto postactor = new Actor();
		postactor->ImportDataAndNewID(*act);
		Game::AddObject(postactor);
		TriggerSelect(postactor);
	}
}

int Selects::SelectNum(){
	return (int)mSelects.size();
}
XMVECTOR Selects::GetPosition(){
	XMVECTOR pos = XMVectorZero();
	for (auto& act : mSelects){
		pos += act->mTransform->WorldPosition();
	}
	int num = (int)mSelects.size();
	if (num){
		pos = pos / (FLOAT)num;
	}
	return pos;
}

Actor* Selects::GetSelectOne(){
	int num = (int)mSelects.size();
	if (num){
		return *mSelects.begin();
	}
	return NULL;
}
std::list<Actor*>& Selects::GetSelects(){
	return mSelects;
}

SelectUndo::SelectUndo()
{
}
SelectUndo::~SelectUndo(){
	for (auto& com : mCommands){
		delete com.second;
	}
}
void SelectUndo::Clear(){
	for (auto& com : mCommands){
		delete com.second;
	}
	mCommands.clear();
}
void SelectUndo::Set(Actor* act){
	if (!act)return;
	auto temp = mCommands.find((int)act);
	if (temp != mCommands.end()){
		delete temp->second;
		temp->second = new ActorUndoCommand(act);
	}
	else{
		mCommands.insert(std::make_pair((int)act, new ActorUndoCommand(act)));
	}
}
void SelectUndo::Remove(Actor* act){
	if (!act)return;
	auto temp = mCommands.find((int)act);
	if (temp != mCommands.end()){
		delete temp->second;
		mCommands.erase(temp);
	}
}
void SelectUndo::PushUndo(){
	if (mCommands.size() == 0)return;

	auto Link = new std::list<ICommand*>();

	for (auto& com : mCommands){
		Link->push_back(com.second);
	}
	Game::SetUndo(new LinkUndoCommand(Link));
	mCommands.clear();
}

SelectActor::SelectActor()
	:mCreateInspector(false)
	, mDontTreeViewSelect(false)
{
	mSelectAsset = false;
	mDragBox = -1;

	mVectorBox = NULL;

	mSelectWireMaterial.mDiffuse.x = 0.95f;
	mSelectWireMaterial.mDiffuse.y = 0.6f;
	mSelectWireMaterial.mDiffuse.z = 0.1f;
	mSelectWireMaterial.Create();
	
	mSelectPhysxWireMaterial.mDiffuse.x = 0.1f;
	mSelectPhysxWireMaterial.mDiffuse.y = 0.95f;
	mSelectPhysxWireMaterial.mDiffuse.z = 0.6f;
	mSelectPhysxWireMaterial.Create();
}

void SelectActor::Finish(){
	if (mVectorBox){
		for (int i = 0; i < 3; i++){

			mVectorBox[i].Finish();
		}
		delete[] mVectorBox;
		mVectorBox = NULL;
	}
}

void SelectActor::Initialize(){
	if (mVectorBox == NULL){
		mVectorBox = new Arrow[3];

		float s = 0.1f;

		for (int i = 0; i < 3; i++){
			Game::AddEngineObject(&mVectorBox[i]);

			mVectorBox[i].mTransform->Scale(XMVectorSet(s,s,s, 1));

		}

		mVectorBox[0].mTransform->DegreeRotate(XMVectorSet(0, 0, -90, 1));
		mVectorBox[1].mTransform->DegreeRotate(XMVectorSet(0, 0, 0, 1));
		mVectorBox[2].mTransform->DegreeRotate(XMVectorSet(90, 0, 0, 1));

		auto mate = mVectorBox[0].GetComponent<MaterialComponent>();
		if (mate){
			auto m = mate->GetMaterial(0);

			m.Create("EngineResource/NoLighting.fx");
			auto& d = m.mCBMaterial.mParam.Diffuse;
			d.x = 1;
			d.y = 0;
			d.z = 0;
			d.w = 1;

			mate->SetMaterial(0, m);
		}
		mate = mVectorBox[1].GetComponent<MaterialComponent>();
		if (mate){
			auto m = mate->GetMaterial(0);
			m.Create("EngineResource/NoLighting.fx");
			auto& d = m.mCBMaterial.mParam.Diffuse;
			d.x = 0;
			d.y = 1;
			d.z = 0;
			d.w = 1;
			mate->SetMaterial(0, m);
		}
		mate = mVectorBox[2].GetComponent<MaterialComponent>();
		if (mate){
			auto m = mate->GetMaterial(0);
			m.Create("EngineResource/NoLighting.fx");
			auto& d = m.mCBMaterial.mParam.Diffuse;
			d.x = 0;
			d.y = 0;
			d.z = 1;
			d.w = 1;
			mate->SetMaterial(0, m);
		}
	}
}

Actor* SelectActor::GetSelectOne(){
	return mSelects.SelectNum()==1?mSelects.GetSelectOne():NULL;
}

void SelectActor::UpdateInspector(){
	if (mSelects.SelectNum() != 1 && !mSelectAsset)return;

	if (!mCreateInspector){
		if (mSelectAsset){
			if (mSelects.SelectNum() <= 1){
				AssetDataBase::CreateInspector(mAssetFileName.c_str());
			}
		}
		else{
			if (mSelects.SelectNum() == 1){
				mSelects.GetSelectOne()->CreateInspector();
				if (!mDontTreeViewSelect){
					Window::SelectTreeViewItem(mSelects.GetSelectOne()->mTreeViewPtr);
				}
				mDontTreeViewSelect = false;
			}
		}

		mCreateInspector = true;
	}
	else{

		static unsigned long time_start = timeGetTime();
		unsigned long current_time = timeGetTime();
		unsigned long b = (current_time - time_start);
		if (b >= 16){
			time_start = timeGetTime();
			//処理が追いつかない場合がある
			Window::UpdateInspector();
		}
	}
}
void SelectActor::Update(float deltaTime){
	//(void)deltaTime;
	//if (Window::GetTreeViewWindow()->IsActive()){
	//	void* s = (Actor*)Window::GetTreeViewWindow()->GetSelectItem();
	//	
	//	if (s){
	//		if (s != mTreeSelect){
	//			mTreeSelect = (Actor*)s;
	//			mSelect = mTreeSelect;
	//		}
	//	}
	//}

	if (!mSelects.SelectNum() || mSelectAsset)return;

	if (Input::Down(KeyCoord::Key_LCONTROL) && Input::Trigger(KeyCoord::Key_C)){
		mSelects.Copy();
	}
	if (Input::Down(KeyCoord::Key_LCONTROL) && Input::Trigger(KeyCoord::Key_V)){
		mSelects.Paste();
	}


	auto tar = mSelects.GetPosition();
	mVectorBox[0].mTransform->Position(tar);
	mVectorBox[1].mTransform->Position(tar);
	mVectorBox[2].mTransform->Position(tar);

	auto cam = Game::GetMainCamera();
	if (cam){
		auto pos = cam->gameObject->mTransform->WorldPosition();
		auto l = XMVector3Length(tar - pos).x/100.0f;
		mVectorBox[0].mTransform->Scale(XMVectorSet(l, l, l, 1));
		mVectorBox[1].mTransform->Scale(XMVectorSet(l, l, l, 1));
		mVectorBox[2].mTransform->Scale(XMVectorSet(l, l, l, 1));


		auto col = mVectorBox[0].GetComponent<PhysXColliderComponent>();
		col->SetScale(XMVectorSet(l, l, l, 1));
		col = mVectorBox[1].GetComponent<PhysXColliderComponent>();
		col->SetScale(XMVectorSet(l, l, l, 1));
		col = mVectorBox[2].GetComponent<PhysXColliderComponent>();
		col->SetScale(XMVectorSet(l, l, l, 1));

	}

	mVectorBox[0].UpdateComponent(deltaTime);
	mVectorBox[1].UpdateComponent(deltaTime);
	mVectorBox[2].UpdateComponent(deltaTime);

	static XMVECTOR vect = XMVectorZero();
	if (Input::Down(MouseCoord::Left)){
		int x, y;
		Input::MouseLeftDragVector(&x, &y);
		//float l = XMVector2Length(XMVectorSet(x,-y,0,1)).x;
		float p = -y *0.05f;
		vect = mDragPos;
		if (Input::Down(KeyCoord::Key_LSHIFT)){
			p += 0.5f;
			p = (float)(int)p;
		}

		if (mDragBox == 0){
			vect += XMVectorSet(p, 0.0f, 0.0f, 0.0f);
		}
		if (mDragBox == 1){
			vect += XMVectorSet(0.0f, p, 0.0f, 0.0f);
		}
		if (mDragBox == 2){
			vect += XMVectorSet(0.0f, 0.0f, p, 0.0f);
		}
	}

	if (Input::Up(MouseCoord::Left)){
		if (mDragBox != -1){
			mSelects.MoveEnd(vect);
		}
		mDragBox = -1;
	}
	if (mDragBox != -1){
		mSelects.MovePos(vect);
	}
	SelectActorDraw();
}
void SelectActor::SelectActorDraw(){

	if (mSelectAsset)return;

	//メッシュ描画
	for (auto select : mSelects.GetSelects()){
		Game::AddDrawList(DrawStage::Engine, std::function<void()>([&, select](){
			if (!select)return;
			auto mModel = select->GetComponent<ModelComponent>();
			if (!mModel)return;
			Model& model = *mModel->mModel;

			auto render = RenderingEngine::GetEngine(ContextType::MainDeferrd);

			render->PushSet(DepthStencil::Preset::DS_Zero_Alawys);
			render->PushSet(Rasterizer::Preset::RS_Back_Wireframe);

			mModel->SetMatrix();
			model.Draw(render->m_Context, mSelectWireMaterial);


			render->PopRS();
			render->PopDS();
		}));
	}
	//Physxメッシュ描画
	for (auto select : mSelects.GetSelects()){
		Game::AddDrawList(DrawStage::Engine, std::function<void()>([&, select](){
			if (!select)return;
			auto com = select->GetComponent<PhysXColliderComponent>();
			if (!com)return;

			auto render = RenderingEngine::GetEngine(ContextType::MainDeferrd);

			render->PushSet(DepthStencil::Preset::DS_Zero_Alawys);
			render->PushSet(Rasterizer::Preset::RS_Back_Wireframe);

			
			com->DrawMesh(render->m_Context, mSelectPhysxWireMaterial);


			render->PopRS();
			render->PopDS();
		}));
	}

	//アロー描画
	Game::AddDrawList(DrawStage::Engine, std::function<void()>([&](){
	
		auto render = RenderingEngine::GetEngine(ContextType::MainDeferrd);

		render->PushSet(DepthStencil::Preset::DS_All_Alawys);
		render->PushSet(Rasterizer::Preset::RS_Back_Solid);
	

		auto f = Game::GetMainCamera()->gameObject->mTransform->Forward();

		auto func = [](const std::pair<float, std::function<void()>>& p1, const std::pair<float, std::function<void()>>& p2)
		{
			return p1.first > p2.first;
		};
		std::set<std::pair<float, std::function<void()>>, std::function<bool(const std::pair<float, std::function<void()>>&, const std::pair<float, std::function<void()>>&)> > draws(func);


		for (int _i = 0; _i < 3; _i++){
			int i = _i;

			auto mModel = mVectorBox[i].GetComponent<ModelComponent>();
			Model& model = *mModel->mModel;

			auto au = mVectorBox[i].mTransform->Up();
			auto dot = XMVector3Dot(au, f);

			auto result = draws.insert(std::pair<float, std::function<void()>>((float)dot.x, [=](){
				mModel->SetMatrix();
				model.Draw(render->m_Context, mVectorBox[i].GetComponent<MaterialComponent>());
			}));
			if (!result.second){
				dot.x -= 0.01f;
				draws.insert(std::pair<float, std::function<void()>>((float)dot.x, [=](){
					mModel->SetMatrix();
					model.Draw(render->m_Context, mVectorBox[i].GetComponent<MaterialComponent>());
				}));
			}
		}
		//Game::GetMainCamera()->SetOrthographic();

		for (auto & d : draws){
			d.second();
		}

		render->PopRS();
		render->PopDS();
	}));
}

void SelectActor::ReleaseSelect(Actor* actor){

	mSelects.GetSelects().remove(actor);
}

void SelectActor::SetSelect(Actor* select, bool dontTreeViewSelect){
	mDragBox = -1;

	if (mSelectAsset){
		mSelects.OneSelect(NULL);
	}
	mSelectAsset = false;

	//存在しなければNULL
	if (!Game::FindActor(select)){
		select = NULL;
	}
	if (EngineInput::Down(KeyCoord::Key_LCONTROL)){
		if (mSelects.TriggerSelect(select)){
			mSelectUndo.Set(select);
		}
		else{
			mSelectUndo.Remove(select);

		}
	}
	else{
		mSelects.OneSelect(select);
		mSelectUndo.Clear();
		mSelectUndo.Set(select);
	}

	Window::ClearInspector();
	mCreateInspector = false;
	mDontTreeViewSelect = dontTreeViewSelect;
}
void SelectActor::SetSelectAsset(Actor* select,const char* filename){
	mDragBox = -1;
	if (!mSelectAsset){
		mSelects.OneSelect(NULL);
	}
	mSelectAsset = true;
	

	if (EngineInput::Down(KeyCoord::Key_LCONTROL)){
		mSelects.TriggerSelect(select);
	}
	else{
		mSelects.OneSelect(select);
	}

	Window::ClearInspector();
	mCreateInspector = false;

	mAssetFileName = filename;

}

void SelectActor::ReCreateInspector(){
	Window::ClearInspector();
	mCreateInspector = false;

}


bool SelectActor::ChackHitRay(PhysX3Main* physx, EditorCamera* camera){

	if (!mSelects.SelectNum() || mSelectAsset)return false;

	int x, y;
	Input::MousePosition(&x, &y);
	XMVECTOR point = XMVectorSet((FLOAT)x, (FLOAT)y, 0.0f, 1.0f);
	XMVECTOR vect = camera->PointRayVector(point);
	XMVECTOR pos = camera->GetPosition();

	auto act = physx->EngineSceneRaycast(pos, vect);
	if (act){
		if (&mVectorBox[0] == act){
			mDragPos = mSelects.GetPosition();
			mSelects.MoveStart(mDragPos);
			mDragBox = 0;
			return true;
		}
		if (&mVectorBox[1] == act){
			mDragPos = mSelects.GetPosition();
			mSelects.MoveStart(mDragPos);
			mDragBox = 1;
			return true;
		}
		if (&mVectorBox[2] == act){
			mDragPos = mSelects.GetPosition();
			mSelects.MoveStart(mDragPos);
			mDragBox = 2;
			return true;
		}
	}
	return false;
}

void SelectActor::PushUndo(){
	mSelectUndo.PushUndo();
}


#endif