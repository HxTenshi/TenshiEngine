#pragma once

#include "../Material/Texture.h"
class Font;
struct ID3D11Texture2D;
struct IDXGIKeyedMutex;
struct IDXGIKeyedMutex;
struct ID2D1RenderTarget;
struct ID2D1SolidColorBrush;
struct IDXGISurface1;
struct IDWriteTextFormat;
struct ID3D10Device1;
struct ID2D1Factory;

class FontManager{
public:

static
HRESULT Init();
static
void Release();

private:
	static ID3D10Device1*		device101;
	static ID2D1Factory*		d2dfactory;

	friend Font;
};

class TextFormat{
public:
	TextFormat();
	~TextFormat();

	IDWriteTextFormat* GetTextFormat();
private:

	IDWriteTextFormat* mWriteTextFormat;
};


class Font{
public:
	Font();
	~Font();
	HRESULT SetText(const std::string& text);
	Texture GetTexture();
	void DrawThreadUpdateText(const std::wstring& wtext);
private:
	void Initialize();

	TextFormat mTextFormat;
	Texture	mTexture;

	ID3D11Texture2D*		texture11;
	IDXGIKeyedMutex*		keyedmutex11;
	IDXGIKeyedMutex*		keyedmutex10;
	ID2D1RenderTarget*		rendertarget;
	ID2D1SolidColorBrush*	brush;
	IDXGISurface1*			surface10;
	bool mInitializeComplete;
};