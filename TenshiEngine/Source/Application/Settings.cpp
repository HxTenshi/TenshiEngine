
#include "Settings.h"
#include "Library/picojson.h"
#include <fstream>
#include "Window/Window.h"

Settings gSettings;

InitParams Settings::mParams;


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
	
	WindowState::mWidth = (UINT)mParams.WindowSizeX;
	WindowState::mHeight = (UINT)mParams.WindowSizeY;

}