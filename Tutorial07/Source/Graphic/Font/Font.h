#pragma once


#include "../Material/Texture.h"
#include <string>
#include <DWrite.h>
#include <d2d1.h>
class Font;

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

	IDWriteTextFormat* GetTextFormat(){
		return mWriteTextFormat;
	}
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
	TextFormat mTextFormat;
	Texture	mTexture;

	ID3D11Texture2D*		texture11;
	IDXGIKeyedMutex*		keyedmutex11;
	IDXGIKeyedMutex*		keyedmutex10;
	ID2D1RenderTarget*		rendertarget;
	ID2D1SolidColorBrush*	brush;
	IDXGISurface1*			surface10;
};