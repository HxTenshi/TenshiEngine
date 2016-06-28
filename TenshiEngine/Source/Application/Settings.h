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
};