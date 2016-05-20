#pragma once

#include "../Material/Texture.h"

struct ID3D11Texture2D;

class FontFileData{
public:
	FontFileData();
	~FontFileData();

	void Create(const char* fileName);
	void SetText(const std::string& text);
	Texture GetTexture();

private:
	void Release();

	HFONT hFont;
	DESIGNVECTOR mDesign;
	std::string mFileName;
	std::string mFontName;
	Texture mTexture;
	ID3D11Texture2D* mTexture2D;
};