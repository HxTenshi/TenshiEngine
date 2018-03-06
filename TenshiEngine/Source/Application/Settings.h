#pragma once

struct InitParams{
	int WindowSizeX = 1200;
	int WindowSizeY = 800;
};

class Settings{
public:
	Settings();
public:
	static InitParams mParams;
	static const char* EngineDirectory;		// エンジンのディレクトリ
	static const char* ProjectDirectory;	// プロジェクトのディレクトリ
};