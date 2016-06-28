#include "DebugNaviMesh.h"

#include "Window/Window.h"

#include "Game/Game.h"

DebugNaviMesh::DebugNaviMesh(){
	mStart = NULL;
	mEnd = NULL;
	mGameObject = NULL;
}
DebugNaviMesh::~DebugNaviMesh(){

}
void DebugNaviMesh::Initialize(Actor* gameObject){
	mGameObject = gameObject;
}
void DebugNaviMesh::EngineUpdate(){

}
void DebugNaviMesh::Update(){

	if (mNavigatePtr && mGameObject){
		mNavigatePtr->Move(mSpeed);
		auto pos = mNavigatePtr->GetPosition();
		mGameObject->mTransform->Position(pos);
	}

}
void DebugNaviMesh::Finish(){

}
void DebugNaviMesh::CreateInspector(std::vector<InspectorDataSet>& data){

	Window::AddInspector(new TemplateInspectorDataSet<std::string>("Class", &mFileName, [&](std::string f){mFileName = f;
		mNaviMesh.Create(mFileName);
	}), data);

	Window::AddInspector(new TemplateInspectorDataSet<float>("speed", &mSpeed, [&](float f){mSpeed = f;
	}), data);
	Window::AddInspector(new InspectorButtonDataSet("Find", [&](){
		Find();
	}), data);

	
}



void DebugNaviMesh::Find(){
	auto start = Game::FindNameActor("NaviStart");
	auto end = Game::FindNameActor("NaviEnd");

	mStart = mNaviMesh.FindNaviPosition(start->mTransform->WorldPosition());
	mEnd = mNaviMesh.FindNaviPosition(end->mTransform->WorldPosition());

	mNaviMeshCreatorPtr = make_shared<NavigateCreator>(mStart, mEnd, &mNaviMesh);

	mNavigatePtr = mNaviMeshCreatorPtr->Result();
}