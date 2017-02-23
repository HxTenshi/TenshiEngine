#pragma once

#include "../Material/Texture.h"

struct ID3D11Texture2D;

class FontFileData{
public:
	FontFileData();
	~FontFileData();

	void AddFont(const char* fileName);
	void CreateFont_(const char* fontName, float FontSize);
	void CreateTexture_(UINT width, UINT height);
	void SetText(const std::string& text, bool center);
	Texture GetTexture();

	static std::vector<std::string>& GetFonts();
	static void ClearFonts();

private:
	void Release();

	static std::vector<std::string> m_Fonts;

	HFONT hFont;
	DESIGNVECTOR mDesign;
	std::string mFileName;
	std::string mFontName;
	Texture mTexture;
	ID3D11Texture2D* mTexture2D;
	UINT mWidth;
	UINT mHeight;
};