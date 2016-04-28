
#include "Scene.h"
#include "MySTL/Utility.h"
#include "MySTL/File.h"
#include "Library/picojson.h"

#include "Game.h"

Scene::Scene(){

}
Scene::~Scene(){
	for (auto act : mMemorySave){
		delete act;
	}
	mMemorySave.clear();
}

void Scene::LoadScene(const std::string& fileName){

	m_FilePath = fileName;
	m_Name = behind_than_find_last_of(fileName, "/");
	if (m_Name == ""){
		m_Name = fileName;
	}
	File scenefile(m_FilePath);
	if (scenefile){
		UINT id;
		while (scenefile){
			if (!scenefile.In(&id))break;
			auto a = new Actor();
			a->ImportData("./Scenes/" + m_Name + "/Object_" + std::to_string(id) + ".json");
			Game::AddObject(a);
		}
	}
}
void Scene::SaveScene(Actor* SceneRoot){
	File scenefile;
	if (!scenefile.Open(m_FilePath)){
		scenefile.FileCreate();
	}
	scenefile.Clear();

	SceneRoot->ExportSceneDataStart("./Scenes/" + m_Name, scenefile);
}

void Scene::MemoryLoadScene(){
	m_FilePath = m_MemorySaveFilePath;
	m_Name = m_MemorySaveName;
	for (auto& act : mMemorySave){
		auto postactor = new Actor();
		postactor->ImportData(*act);
		Game::AddObject(postactor);
	}
}

void Scene::MemorySaveScene(){
	for (auto act : mMemorySave){
		delete act;
	}
	mMemorySave.clear();

	m_MemorySaveFilePath = m_FilePath;
	m_MemorySaveName = m_Name;
	Game::GetAllObject([&](Actor* act){
		auto data = new picojson::value();
		act->ExportData(*data);
		mMemorySave.push_back(data);
	});
}
