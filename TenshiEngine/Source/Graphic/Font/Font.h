#pragma once

#include "../Material/Texture.h"

struct ID3D11Texture2D;

class FontFileData{
public:
	FontFileData();
	~FontFileData();

	void CreateFont_(const char* fileName, float FontSize);
	void CreateTexture_(UINT width, UINT height);
	void SetText(const std::string& text, bool center);
	Texture GetTexture();

private:
	void Release();

	HFONT hFont;
	DESIGNVECTOR mDesign;
	std::string mFileName;
	std::string mFontName;
	Texture mTexture;
	ID3D11Texture2D* mTexture2D;
	UINT mWidth;
	UINT mHeight;
};