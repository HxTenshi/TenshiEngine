
#include "Settings.h"
#include "Library/picojson.h"
#include <fstream>
#include "Window/Window.h"

Settings gSettings;

InitParams Settings::mParams;
const char* Settings::EngineDirectory;
const char* Settings::ProjectDirectory;


//template <class T>
//void get(picojson::object& obj, T& out ,const std::string& key){
//	auto v = obj.find(key);
//	if (v == obj.end())return;
//
//	out = v->second.get<T>();
//}

Settings::Settings(){


	//picojson::value value;
	//std::ifstream jsonfile();
	//if (jsonfile.fail()){
	//	return;
	//}
	//jsonfile >> value;

	//auto obj = value.get<picojson::object>();
	//
	//get(obj, mParams.WindowSizeX, "WindowSizeX");
	//get(obj, mParams.WindowSizeY, "WindowSizeY");

	auto inifile = "Settings/setting.ini";

	mParams.WindowSizeX = GetPrivateProfileInt(
		"Window", "SizeX", 1200, inifile);
	mParams.WindowSizeY = GetPrivateProfileInt(
		"Window", "SizeY", 800, inifile);
	int showcursor = GetPrivateProfileInt(
		"Window", "ShowCursor", 1, inifile);

	{// エンジンのディレクトリを取得
		static char dir[512];
		GetCurrentDirectory(512, dir);
		Settings::EngineDirectory = dir;
	}

	{// プロジェクトのディレクトリを取得
		static char dir[512];
		if( GetPrivateProfileString(
			"Project", "ProjectPath", "", dir, 512, inifile) ) {
			if( SetCurrentDirectory(dir) ) {
				Settings::ProjectDirectory = dir;
			}
			else {
				//error
			}
		}
		else {
			Settings::ProjectDirectory = Settings::EngineDirectory;
		}
	}
	
	WindowState::mWidth = (UINT)mParams.WindowSizeX;
	WindowState::mHeight = (UINT)mParams.WindowSizeY;
	ShowCursor((BOOL)showcursor);
}