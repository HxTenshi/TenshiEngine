#pragma once

class ScriptManager {
public:

#ifdef _ENGINE_MODE
	static void ReCompile();
	static void CreateScriptFile(const std::string& className);

	static void OpenScriptFile(const std::string& fileName, int line = 0);
#endif
};