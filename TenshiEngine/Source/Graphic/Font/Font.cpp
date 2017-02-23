
#include <Shlwapi.h>

#include "Font.h"
#include "Window/Window.h"
#include "Device/DirectX11Device.h"
#include "Game/RenderingSystem.h"

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
				int Alpha = 0;
				if (mBitmapByte.size() > _pos) {
					Alpha =
						(255 * mBitmapByte[_pos]) / (mBitmapState.Level - 1);
				}
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
		bool kuuhaku = false;
		//空白処理
		if (code == 32) {
			kuuhaku = true;
			code = (UINT)'t';
		}
		if (code == 33088) {
			kuuhaku = true;
			code = (UINT)'あ';
		}

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
		if(size == GDI_ERROR) {
			//エラー処理。
			//スペースの場合もここが呼ばれるらしいのでその処理
			//スペースの代わり
			//code = (UINT)'t';
			//DWORD size = GetGlyphOutline(
			//	hdc,
			//	code,
			//	GGO_GRAY4_BITMAP,
			//	&GM,
			//	0,
			//	NULL,
			//	&Mat);
			//mBitmapByte.resize(size);
			return;
		}
		else {
			mBitmapByte.resize(size);

		}
		if (!kuuhaku) {
			GetGlyphOutline(
				hdc, code, GGO_GRAY4_BITMAP, &GM, size, mBitmapByte.data(), &Mat);
		}

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
				//32空白
				code = (UINT)c;
			}
			else
			{
				//33088空白
				auto c1 = (BYTE)text[i];
				auto c2 = (BYTE)text[i + 1];
				code = (UINT)(((UINT)c1) << 8 | ((UINT)c2));
				i++;
			}



			mCharacters.push_back(Character());
			mCharacters[ci].Create(code, hdc);
			ci++;
		}
	}
	~Text(){

	}

	void Weight(BYTE* pOutBits, TextureBitmapState mTextureBitmapState, bool Center){

		if (Center){
			float textLenght = 0.0f;
			for (const auto& c : mCharacters){
				textLenght += c.GetState().x + c.GetState().w;
			}
			mTextureBitmapState.x = (mTextureBitmapState.w - textLenght) / 2.0f;
		}

		for (const auto& c : mCharacters){
			c.Weight(pOutBits, mTextureBitmapState);
			mTextureBitmapState.x += c.GetState().x + c.GetState().w;
		}
	}

private:
	std::vector<Character> mCharacters;
};


//static 
std::vector<std::string> FontFileData::m_Fonts;

FontFileData::FontFileData()
	:hFont(NULL)
	, mTexture2D(NULL){
	mWidth = 128;
	mHeight = 128;

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

// 日本語フォントの列挙
int CALLBACK EnumJFontProc(
	ENUMLOGFONTEX *lpelfe, // 論理的なフォントデータ
	NEWTEXTMETRICEX *lpntme, // 物理的なフォントデータ
	DWORD dwFontType, // フォントの種類
	LPARAM lParam // アプリケーション定義のデータ
)
{
	// truetype 以外は除外
	if (dwFontType != TRUETYPE_FONTTYPE)return TRUE; // 続けるときは 0 以外を返す
													 // 先頭に @ が付くものは除外
	if (lpelfe->elfLogFont.lfFaceName[0] == '@')return TRUE;
	// 先頭に $ が付くものは除外
	if (lpelfe->elfLogFont.lfFaceName[0] == '$')return TRUE;
	// 日本語以外は除外
	if (lstrcmp((LPCSTR)lpelfe->elfScript, TEXT("日本語")) != 0)return TRUE;
	// リストに登録
	//SendMessage((HWND)lParam, LB_ADDSTRING, 0, (LPARAM)lpelfe->elfLogFont.lfFaceName);

	auto v = (std::vector<std::string>*)lParam;
	v->push_back(lpelfe->elfLogFont.lfFaceName);

	return TRUE;

}

// 英語フォントの列挙
int CALLBACK EnumEFontProc(ENUMLOGFONTEX *lpelfe, NEWTEXTMETRICEX *lpntme, DWORD dwFontType, LPARAM lParam)
{
	// truetype 以外は除外
	if (dwFontType != TRUETYPE_FONTTYPE)return TRUE; // 続けるときは 0 以外を返す
													 // 先頭に @ が付くものは除外
	if (lpelfe->elfLogFont.lfFaceName[0] == '@')return TRUE;
	// 先頭に $ が付くものは除外
	if (lpelfe->elfLogFont.lfFaceName[0] == '$')return TRUE;
	// 欧文以外は除外
	if (lstrcmp((LPCSTR)lpelfe->elfScript, TEXT("欧文")) != 0)return TRUE;
	// スタイルに「標準」があるのは日本語フォントなので除外。これはかなりの決め手になった
	if (lstrcmp((LPCSTR)lpelfe->elfStyle, TEXT("標準")) == 0)return TRUE;
	// メイリオフォントのスタイルに「レギュラー」があるので除外。
	if (lstrcmp((LPCSTR)lpelfe->elfStyle, TEXT("レギュラー")) == 0)return TRUE;
	// リストに登録
	//SendMessage((HWND)lParam, LB_ADDSTRING, 0, (LPARAM)lpelfe->elfLogFont.lfFaceName);

	auto v = (std::vector<std::string>*)lParam;
	v->push_back(lpelfe->elfLogFont.lfFaceName);

	return TRUE;
}


void FontFileData::AddFont(const char * fileName)
{
	if (mFileName != "") {
		// リソース削除
		RemoveFontResourceEx(
			mFileName.c_str(), //ttfファイルへのパス
			FR_PRIVATE,
			&mDesign
		);
		mFileName = "";
	}

	AddFontResourceEx(
		mFileName.c_str(), //ttfファイルへのパス
		FR_PRIVATE,
		&mDesign
	);
}

void FontFileData::CreateFont_(const char* fontName, float FontSize){
	{
		if (hFont){
			DeleteObject(hFont);
			hFont = NULL;
			mFontName = "";
		}
	}
	mFontName = fontName;// "メイリオ";
	//mFontName = "あんずもじ湛";
	// フォントの生成
	LOGFONT lf = {
		FontSize, 0, 0, 0, 0, 0, 0, 0,
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
}
void FontFileData::CreateTexture_(UINT width, UINT height){

	{
		if (mTexture2D){
			mTexture2D->Release();
			mTexture2D = NULL;
		}
	}


	// CPUで書き込みができるテクスチャを作成
	D3D11_TEXTURE2D_DESC desc;
	memset(&desc, 0, sizeof(desc));
	//desc.Width = GM.gmCellIncX;
	//desc.Height = TM.tmHeight;

	mWidth = width;
	mHeight = height;
	desc.Width = mWidth;
	desc.Height = mHeight;

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


void FontFileData::SetText(const std::string& text, bool center){
	if (!hFont)return;
	if (!mTexture2D)return;

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
	state.w = mWidth;
	state.h = mHeight;

	//テクスチャーをクリア
	int size = state.w*state.h * 4;
	memset(pBits, 0, size);

	textData.Weight(pBits, state,center);

	render->m_Context->Unmap(mTexture2D, 0);



}

Texture FontFileData::GetTexture(){
	return mTexture;
}

std::vector<std::string>& FontFileData::GetFonts()
{
	if (m_Fonts.size() == 0) {
		// switch 文に入る前に変数宣言
		LOGFONT lf; // LOGFONT 構造体
		HDC hDC;
		auto hwnd = Window::GetMainHWND();

		hDC = GetDC(hwnd);
		lf.lfFaceName[0] = '\0'; // 全てのフォント名
		lf.lfPitchAndFamily = DEFAULT_PITCH; // 必ずこれ（= 0）を選ばなければならない

		// 日本語選択リストにフォントを表示するための処理
		lf.lfCharSet = SHIFTJIS_CHARSET /*DEFAULT_CHARSET*/; //DEFAULT_CHARSETでも挙動は一緒
															 //列挙プロシージャ関数はフォントを 1 つ列挙するたびに 1 度ずつ呼び出されます
		EnumFontFamiliesEx(hDC, &lf, (FONTENUMPROC)EnumJFontProc, (LPARAM)&m_Fonts, 0);

		// 英語選択リストにフォントを表示するための処理
		lf.lfCharSet = ANSI_CHARSET/*DEFAULT_CHARSET*/;
		//列挙プロシージャ関数はフォントを 1 つ列挙するたびに 1 度ずつ呼び出されます
		EnumFontFamiliesEx(hDC, &lf, (FONTENUMPROC)EnumEFontProc, (LPARAM)&m_Fonts, 0);

		ReleaseDC(hwnd, hDC);
	}
	return m_Fonts;
}

void FontFileData::ClearFonts()
{
	m_Fonts.clear();
}
