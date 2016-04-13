// stdafx.h : 標準のシステム インクルード ファイルのインクルード ファイル、または
// 参照回数が多く、かつあまり変更されない、プロジェクト専用のインクルード ファイル
// を記述します。

#pragma once

#include "../Source/Application/Define.h"
#define NULL 0
static class Data{
public:
	static HWND GetHWND(){
		if (mhWnd == NULL){
			mhWnd = FindWindow(L"TenshiEngineDummyWindowClass", NULL);
		}
		return mhWnd;
	}
	static void MyPostMessage(MyWindowMessage wm, void* p = NULL);
	static void _SendMessage(UINT wm, WPARAM p1 = NULL, LPARAM p2 = NULL);

private:
	static HWND mhWnd;
};