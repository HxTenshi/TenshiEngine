#pragma once


class InputManagerRapper{
public:

	void Initialize(HWND hWnd, HINSTANCE hInstance);

	void Update();

	void Release();

#ifdef _ENGINE_MODE
	bool GetScreenFocus();
#endif

private:
	bool mScreenFocus;
	bool mr, ml;
	int mx, my;
	int wx, wy;
};