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
	static const char* EngineDirectory;		// �G���W���̃f�B���N�g��
	static const char* ProjectDirectory;	// �v���W�F�N�g�̃f�B���N�g��
};