#pragma once

#include "../Material/Texture.h"
class Font;
class ID3D11Texture2D;
class IDXGIKeyedMutex;
class IDXGIKeyedMutex;
class ID2D1RenderTarget;
class ID2D1SolidColorBrush;
class IDXGISurface1;
class IDWriteTextFormat;
class ID3D10Device1;
class ID2D1Factory;

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