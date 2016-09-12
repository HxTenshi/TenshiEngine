
#include "Scene.h"
#include "MySTL/Utility.h"
#include "MySTL/File.h"
#include "Game.h"

Scene::Scene(){
}
Scene::~Scene(){
}

void Scene::MemoryLoadScene(){
	m_FilePath = m_MemorySaveFilePath;
	m_Name = m_MemorySaveName;
	for (auto& act : m_MemorySave){
		auto postactor = make_shared<Actor>();
		postactor->ImportData(act);
		Game::AddObject(postactor);
	}

}

void Scene::MemorySaveScene(){
	m_MemorySave.clear();

	m_MemorySaveFilePath = m_FilePath;
	m_MemorySaveName = m_Name;
	Game::GetAllObject([&](GameObject act){
		picojson::value data;
		act->ExportData(data);
		m_MemorySave.push_back(data);
	});
}


void Scene::LoadScene(const std::string& fileName){
	m_FilePath = fileName;
	picojson::value out;
	std::ifstream scene(fileName);
	scene >> out;

	auto arr = out.get<picojson::array>();
	for (auto& val : arr){
		auto postactor = make_shared<Actor>();
		postactor->ImportData(val);
		Game::AddObject(postactor);
	}

}

void Scene::SaveScene(){
	picojson::array arr;
	Game::GetAllObject([&](GameObject act){
		picojson::value data;
		act->ExportData(data);
		arr.push_back(data);
	});
	picojson::value out(arr);

	if (m_FilePath == ""){
		m_FilePath = "Assets/new.scene";
	}
	{
		File scenefile;
		if (!scenefile.Open(m_FilePath)){
			scenefile.FileCreate();
		}
		scenefile.Clear();
	}

	std::ofstream scene(m_FilePath);
	scene << out;
}
