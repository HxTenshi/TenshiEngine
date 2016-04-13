#pragma once

#include <string>
#include <list>
namespace picojson{
	class value;
}
class Actor;

class Scene{
public:
	Scene();
	~Scene();

	void LoadScene(const std::string& fileName);
	void SaveScene(Actor* SceneRoot);

	void MemoryLoadScene();
	void MemorySaveScene();

private:

	std::string m_FilePath;
	std::string m_Name;

	std::list<picojson::value*> mMemorySave;
};