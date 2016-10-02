#pragma once

#include "Game/Actor.h"


class Canvas :public Actor {
public:
	Canvas();
	~Canvas();

#ifdef _ENGINE_MODE
	void CreateInspector() override;
#endif

	void _ExportData(I_ioHelper* io, bool childExport = false) override;
	void _ImportData(I_ioHelper* io) override;

	static float GetWidth() { return m_Width; }
	static float GetHeight() { return m_Height; }
private:
	static float m_Width;
	static float m_Height;
};