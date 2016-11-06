#pragma once

#include <vector>
#include <list>
#include "../ScriptComponent/main.h"

class File;
class IDllScriptComponent;
class ScriptComponent;
class IGame;
class ScriptManager;
class UseScriptActors {
public:
	static UseScriptActors* Get();
	IDllScriptComponent* Create(const std::string& ClassName, ScriptComponent* com);
	void Deleter(IDllScriptComponent* script, ScriptComponent* com);
	bool Function(IDllScriptComponent* com, IDllScriptComponent::Func0 func);
	bool Function(IDllScriptComponent* com, IDllScriptComponent::Func1 func, GameObject tar);
private:
	UseScriptActors();
	~UseScriptActors();

#ifdef _ENGINE_MODE
	void ReCompile();

	void CreateIncludeClassFile();
	void findSerialize(File* file, const std::string& classname);

	void findMember(std::string&& pickstr, File* file, const std::string& classname);
	void reflect(const std::string& member, File* file, const std::string& classname);
	void filter(std::string& str);

	void split(std::vector<std::string> &v, const std::string &input_string, const std::string &delimiter);
#endif

	void UnLoad();
	void DllLoad();
	std::list<ScriptComponent*> mList;
	void* mCreate;
	void* mDelete;
	void* mGetReflect;
	void* mFunction0;
	void* mFunction1;
	HMODULE hModule;

	friend ScriptManager;
};