
#include <Shlwapi.h>

#include "Font.h"
#include "Window/Window.h"
#include "Device/DirectX11Device.h"
#include "Game/RenderingSystem.h"


template <class T> void SafeRelease(T **ppT)
{
	if (*ppT)
	{
		(*ppT)->Release();
		*ppT = NULL;
	}
}


struct BitmapState{
	int x = 0;//書き出し位置(左上)
	int y = 0;//書き出し位置(左上)
	int w = 0;//フォントビットマップの幅
	int h = 0;//フォントビットマップの高
	int Level = 17;//α値の段階 (GGO_GRAY4_BITMAPなので17段階)
};
struct TextureBitmapState{
	int x = 0;//書き出し位置(左上)
	int y = 0;//書き出し位置(左上)
	int w = 0;//フォントビットマップの幅
	int h = 0;//フォントビットマップの高
};

class Character{
public:
	Character(){
	}
	~Character(){

	}

	void Weight(BYTE* pOutBits, const TextureBitmapState& mTextureBitmapState) const{
		for (int y = mBitmapState.y; y < mBitmapState.y + mBitmapState.h; y++)
		{
			for (int x = mBitmapState.x; x < mBitmapState.x + mBitmapState.w; x++)
			{

				int _x = x + mTextureBitmapState.x;
				int _y = y + mTextureBitmapState.y;
				//テクスチャー外の書き込み
				if (_x >= mTextureBitmapState.w ||
					_y >= mTextureBitmapState.h){
					continue;
				}


				_x = x - mBitmapState.x;
				_y = y - mBitmapState.y;
				int _pos = _x + _y * mBitmapState.w;
				int Alpha =
					(255 * mBitmapByte[_pos]) / (mBitmapState.Level - 1);
				int Color = 0x00ffffff | (Alpha << 24);





				_x = (x + mTextureBitmapState.x) * 4;
				_y = (y + mTextureBitmapState.y) * mTextureBitmapState.w * 4;



				_pos = _x + _y;

				memcpy(
					pOutBits + _pos,
					&Color,
					sizeof(DWORD));
			}
		}
	}

	void Create(UINT Code, HDC hdc){


		UINT code = Code;     // テクスチャに書き込む文字

		// フォントビットマップ取得
		TEXTMETRIC TM;
		GetTextMetrics(hdc, &TM);
		GLYPHMETRICS GM;
		CONST MAT2 Mat = { { 0, 1 }, { 0, 0 }, { 0, 0 }, { 0, 1 } };
		DWORD size = GetGlyphOutline(
			hdc,
			code,
			GGO_GRAY4_BITMAP,
			&GM,
			0,
			NULL,
			&Mat);
		mBitmapByte.resize(size);
		GetGlyphOutline(
			hdc, code, GGO_GRAY4_BITMAP, &GM, size, mBitmapByte.data(), &Mat);

		mBitmapState.x = GM.gmptGlyphOrigin.x;
		mBitmapState.y = TM.tmAscent - GM.gmptGlyphOrigin.y;
		mBitmapState.w = GM.gmBlackBoxX + (4 - (GM.gmBlackBoxX % 4)) % 4;
		mBitmapState.h = GM.gmBlackBoxY;
	}

	const BitmapState& GetState() const{
		return mBitmapState;
	}

private:

	std::vector<BYTE> mBitmapByte;
	BitmapState mBitmapState;

};

class Text{
public:
	Text(const std::string& text, HDC hdc){

		if (!hdc)return;

		auto size = (int)text.length();
		int ci = 0;
		for (int i = 0; i < size; ++i){

			//void* c = &wtext[i];
			//BYTE* byte = (BYTE*)c;
			UINT code = 0;

			auto c = text[i];

			if (IsDBCSLeadByte(c) == 0)
			{
				code = (UINT)c;
			}
			else
			{

				auto c1 = (BYTE)text[i];
				auto c2 = (BYTE)text[i + 1];
				code = (UINT)((c1) << 8 | (c2));
				i++;
			}



			mCharacters.push_back(Character());
			mCharacters[ci].Create(code, hdc);
			ci++;
		}
	}
	~Text(){

	}

	void Weight(BYTE* pOutBits, TextureBitmapState mTextureBitmapState){

		for (const auto& c : mCharacters){
			c.Weight(pOutBits, mTextureBitmapState);
			mTextureBitmapState.x += c.GetState().x + c.GetState().w;

		}
	}

private:
	std::vector<Character> mCharacters;
};



FontFileData::FontFileData()
	:hFont(NULL)
	, mTexture2D(NULL){
	Create("");

}
FontFileData::~FontFileData(){
	Release();
}


void FontFileData::Release(){
	if (mFileName != ""){
		// リソース削除
		RemoveFontResourceEx(
			mFileName.c_str(), //ttfファイルへのパス
			FR_PRIVATE,
			&mDesign
			);
		mFileName = "";
	}


	if (hFont){
		DeleteObject(hFont);
		hFont = NULL;
		mFontName = "";
	}

	if (mTexture2D){
		mTexture2D->Release();
		mTexture2D = NULL;
	}
}

void FontFileData::Create(const char* fileName){
	Release();
	mFileName = "EngineResource/meiryo.ttc";
	//mFileName = "APJapanesefontF.ttf";
	// フォントを使えるようにする
	AddFontResourceEx(
		mFileName.c_str(), //ttfファイルへのパス
		FR_PRIVATE,
		&mDesign
		);
	mFontName = "メイリオ";
	//mFontName = "あんずもじ湛";
	// フォントの生成
	LOGFONT lf = {
		48.0f, 0, 0, 0, 0, 0, 0, 0,
		SHIFTJIS_CHARSET,
		OUT_TT_ONLY_PRECIS,
		CLIP_DEFAULT_PRECIS,
		PROOF_QUALITY,
		FIXED_PITCH | FF_MODERN,
		""
	};

	const char* text = mFontName.c_str();
	auto size = mFontName.size();
	if (size >= 32)return;
	//32文字まで
	memcpy(lf.lfFaceName, text, sizeof(CHAR)*size);
	lf.lfFaceName[size] = '\0';

	hFont = CreateFontIndirect(&lf);



	// CPUで書き込みができるテクスチャを作成
	D3D11_TEXTURE2D_DESC desc;
	memset(&desc, 0, sizeof(desc));
	//desc.Width = GM.gmCellIncX;
	//desc.Height = TM.tmHeight;


	desc.Width = 400;
	desc.Height = 400;

	desc.MipLevels = 1;
	desc.ArraySize = 1;
	// RGBA(255,255,255,255)タイプ
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	// 動的（書き込みするための必須条件）
	desc.Usage = D3D11_USAGE_DYNAMIC;
	// シェーダリソースとして使う	
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	// CPUからアクセスして書き込みOK
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	Device::mpd3dDevice->CreateTexture2D(&desc, 0, &mTexture2D);

	ID3D11ShaderResourceView* pShaderResourceView;
	Device::mpd3dDevice->CreateShaderResourceView(mTexture2D, nullptr, &pShaderResourceView);
	mTexture.Create(pShaderResourceView);


}


void FontFileData::SetText(const std::string& text){
	if (!hFont)return;

	auto hwnd = Window::GetGameScreenHWND();
	HDC hdc = GetDC(hwnd);
	HFONT oldFont = (HFONT)SelectObject(hdc, hFont);

	auto textData = Text(text,hdc);

	// デバイスコンテキストとフォントハンドルの開放
	SelectObject(hdc, oldFont);
	ReleaseDC(hwnd, hdc);


	auto render = RenderingEngine::GetEngine(ContextType::MainDeferrd);

	// フォント情報をテクスチャに書き込む部分
	D3D11_MAPPED_SUBRESOURCE hMappedResource;
	render->m_Context->Map(
		mTexture2D,
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&hMappedResource);

	// ここで書き込む
	BYTE* pBits = (BYTE*)hMappedResource.pData;

	TextureBitmapState state;
	state.w = 400;
	state.h = 400;

	//テクスチャーをクリア
	int size = state.w*state.h * 4;
	memset(pBits, 0, size);

	textData.Weight(pBits, state);

	render->m_Context->Unmap(mTexture2D, 0);



}

Texture FontFileData::GetTexture(){
	return mTexture;
}
