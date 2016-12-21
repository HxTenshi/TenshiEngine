#pragma once


class InputManagerRapper{
public:

	void Initialize(HWND hWnd, HINSTANCE hInstance);

	void Update();

	void Release();

	void SetMouseP(const LPARAM& lParam);

#ifdef _ENGINE_MODE
	bool GetScreenFocus();
#endif

private:
	bool mScreenFocus;

#ifdef _ENGINE_MODE
	bool mEngineFocus;
#endif
	bool mr, ml;
	int mx, my;
	int wx, wy;
};