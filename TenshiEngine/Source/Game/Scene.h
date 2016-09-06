#pragma once

#include "Library/picojson.h"

class Actor;

class Scene{
public:
	Scene();
	~Scene();

	void LoadScene(const std::string& fileName);
	void SaveScene();

	void MemoryLoadScene();
	void MemorySaveScene();

private:

	std::string m_FilePath;
	std::string m_Name;

	std::string m_MemorySaveFilePath;
	std::string m_MemorySaveName;
	picojson::array m_MemorySave;
};