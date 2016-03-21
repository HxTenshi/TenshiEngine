#pragma once

#include <string>
class Actor;

class Scene{
public:
	Scene();
	~Scene();

	void LoadScene(const std::string& fileName);
	void SaveScene(Actor* SceneRoot);

private:

	std::string m_FilePath;
	std::string m_Name;
};