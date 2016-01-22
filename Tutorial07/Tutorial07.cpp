//--------------------------------------------------------------------------------------
// File: Tutorial07.cpp
//
// This application demonstrates texturing
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------


//#define _XM_NO_INTRINSICS_
// /D "_XM_NO_INTRINSICS_" 

#include <d3d11.h>
#include "XNAMath/xnamath.h"

#include "Window/Window.h"
#include "Device/DirectX11Device.h"

#include "MySTL/ptr.h"
#include "ConstantBuffer/ConstantBuffer.h"
#include "Graphic/Model/Model.h"
#include "Graphic/Material/Material.h"

#include "ConstantBuffer/ConstantBufferDif.h"

#include "Game/Game.h"

#include "Graphic/Font/Font.h"
//#include "Input\InputManager.h"

class InputManagerRapper{
public:

	void Initialize(HWND hWnd, HINSTANCE hInstance){
		mr = false;
		ml = false;
		mx = 0;
		my = 0;
		wx = 1;
		wy = 1;

		InputManager::InitDirectInput(hWnd, hInstance);
		Window::SetMouseEvents(&ml, &mr, &mx, &my, &wx, &wy);
	}

	void Update(){

		InputManager::SetMouseL(ml);
		InputManager::SetMouseR(mr);
		int x = (int)((float)mx / wx * WindowState::mWidth);
		int y = (int)((float)my / wy * WindowState::mHeight);
		InputManager::SetMouseXY(x, y);
		InputManager::Update();
	}

	void Release(){
		InputManager::Release();
	}

private:
	bool mr, ml;
	int mx, my;
	int wx, wy;
};

#include <memory>
void a(){
	std::make_shared<int>();
}

class Light{
public:
	Light()
	{

	}
	~Light()
	{

	}

	HRESULT Create()
	{

		mCBChangeLgiht = ConstantBuffer<cbChangesLight>::create(3);
		if (!mCBChangeLgiht.mBuffer)
			return S_FALSE;

		mCBChangeLgiht.mParam.LightVect = XMFLOAT4(-1.0f, -1.0f, 1.0f, 0.0f);
		mCBChangeLgiht.mParam.LightColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		mCBChangeLgiht.UpdateSubresource();

		mCBChangeLgihtMaterial = ConstantBuffer<cbChangesMaterial>::create(5);
		if (!mCBChangeLgihtMaterial.mBuffer)
			return S_FALSE;

		mCBChangeLgihtMaterial.mParam.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		mCBChangeLgihtMaterial.mParam.Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
		mCBChangeLgihtMaterial.mParam.Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

		mCBChangeLgihtMaterial.UpdateSubresource();

		return S_OK;
	}

	void VSSetConstantBuffers()
	{
		mCBChangeLgiht.VSSetConstantBuffers();
	}
	void PSSetConstantBuffers()
	{
		mCBChangeLgiht.PSSetConstantBuffers();
		mCBChangeLgihtMaterial.PSSetConstantBuffers();
	}

	void Release(){

	}
private:

	ConstantBuffer<cbChangesLight>		mCBChangeLgiht;
	ConstantBuffer<cbChangesMaterial>	mCBChangeLgihtMaterial;
};

#include "Library\sqlite3.h"


#include "Engine\AssetLoader.h"
class Application{
public:
	Application()
		:mGame(NULL)
	{
		AssetLoader as;
		as.LoadFile("");

		//// DBファイル名
		//char *fileName = "mySQLiteDB";
		//// DBオブジェクト
		//sqlite3 *pDB = NULL;
		//// DBのオープン
		//int err = sqlite3_open(fileName, &pDB);
		//if (err != SQLITE_OK){
		//	/* エラー処理 */
		//}
		//
		//// テーブルの作成
		//char *errMsg = NULL;
		//
		//err = sqlite3_exec(pDB,
		//	"CREATE TABLE IF NOT EXISTS myTable "
		//	"(id INTEGER PRIMARY KEY AUTOINCREMENT, "
		//	"name CHAR(32) NOT NULL)",
		//	NULL, NULL, &errMsg);
		//
		//if (err != SQLITE_OK){
		//	printf("%s\n", errMsg);
		//	sqlite3_free(errMsg);
		//	errMsg = NULL;
		//	/* TODO: エラー処理 */
		//}
		//
		//// ステートメントオブジェクト
		//sqlite3_stmt *pStmt = NULL;
		//
		//// データの追加
		//// ステートメントの用意
		//err = sqlite3_prepare_v2(pDB,
		//	"INSERT INTO myTable (name) VALUES (?)",
		//	128, &pStmt, NULL);
		//
		//if (err != SQLITE_OK){
		//	/* TODO:エラー処理 */
		//}
		//else{
		//	int i = 0;
		//	for (i = 0; i < 5; i++){
		//		std::string name = "test" + std::to_string(i);
		//
		//		// ?の部分に値を代入
		//		sqlite3_bind_text(pStmt, 1, name.c_str(), name.length(), SQLITE_STATIC);
		//		while (SQLITE_DONE != sqlite3_step(pStmt)){}
		//	}
		//}
		//
		//// ステートメントの解放
		//sqlite3_finalize(pStmt);
		//
		//// データの抽出
		//// ステートメントの用意
		//err = sqlite3_prepare_v2(pDB,
		//	"SELECT * FROM myTable", 64,
		//	&pStmt, NULL);
		//
		//if (err != SQLITE_OK){
		//	/* TODO:エラー処理 */
		//}
		//else{
		//	// データの抽出
		//	while (SQLITE_ROW == (err = sqlite3_step(pStmt))){
		//		int id = sqlite3_column_int(pStmt, 0);
		//		const unsigned char *name = sqlite3_column_text(pStmt, 1);
		//		printf("id: %d, name: %s\n", id, name);
		//	}
		//
		//	if (err != SQLITE_DONE){
		//		/* TODO: エラー処理 */
		//	}
		//}
		//
		//// ステートメントの解放
		//sqlite3_finalize(pStmt);
		//
		//// DBのクローズ
		//err = sqlite3_close(pDB);
		//if (err != SQLITE_OK){
		//	/* TODO:エラー処理 */
		//}
	}

	HRESULT InitDevice(const Window& window)
	{
		HRESULT hr = S_OK;

		hr = Device::Init(window);
		if (FAILED(hr))
			return hr;

		FontManager::Init();

		// Set primitive topology
		//インデックスの並び　Z字など
		Device::mpImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		mInputManagerRapper.Initialize(window.GetGameScreenHWND(), window.mhInstance);


		hr = mLight.Create();
		if (FAILED(hr))
			return hr;

		mGame = new Game();

		return S_OK;
	}

	void Render()
	{

		static unsigned long time_start = timeGetTime();
		static int count_frames = 0;

		if (count_frames > 60){
			time_start = timeGetTime();
			count_frames = 1;
		}
		{

			mInputManagerRapper.Update();

			mGame->Update();

			mLight.PSSetConstantBuffers();

			mGame->Draw();


			ID3D11ShaderResourceView *const pNULL[4] = { NULL, NULL, NULL, NULL };
			Device::mpImmediateContext->PSSetShaderResources(0, 4, pNULL);
			ID3D11SamplerState *const pSNULL[4] = { NULL, NULL, NULL, NULL };
			Device::mpImmediateContext->PSSetSamplers(0, 4, pSNULL);

			Device::mpSwapChain->Present(1, 0);

		}

		unsigned long current_time = timeGetTime();
		double fps = double(count_frames) / (current_time - time_start) * 1000;
		std::string title = "fps: "+std::to_string(fps);

		Window::SetWindowTitle(title);
		count_frames++;
	}


	void CleanupDevice()
	{

		if (mGame)delete mGame;

		FontManager::Release();

		mInputManagerRapper.Release();

		Device::CleanupDevice();

		mLight.Release();

		Window::Release();
	}
private:

	Light mLight;
	InputManagerRapper mInputManagerRapper;
	Game* mGame;
};


#include <thread>

/* スレッドプロシージャ */
void ThreadProc(int nThreadNo)
{
	MessageBox(NULL, "thread", "error", MB_OK);
	//Test::NativeFraction f();

	//ポインタにしないとデストラクタが呼ばれない？
	//Test::NativeFraction* f = new Test::NativeFraction(1.0f, 4.0f);
	//printf("%.3f", f->GetRatio());
	//delete f;
	ExitThread(0);
}
//--------------------------------------------------------------------------------------
// Entry point to the program. Initializes everything an4d goes into a message processing 
// loop. Idle time is used to render the scene.
//--------------------------------------------------------------------------------------

inline
bool WhileColl(){
	if (Input::Down(KeyCoord::Key_ESCAPE)){
		throw(0);
	}
	return true;
}

struct throwNull{};
inline
throwNull ForColl(){
	if (Input::Down(KeyCoord::Key_ESCAPE)){
		throw(0);
	}
	return throwNull();
}

template<class T>
T& operator & (T&& t, throwNull& n){
	return t;
}

//無限ループ回避
//インプットアップデートに依存しないデバッグインプットを作成する必要あり
//#define while(x) while( (x) && WhileColl() )
//#define for(x) for( x & ForColl() )
//
//#undef while
//#undef for

int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
{

	//HANDLE hThread;
	//DWORD dwThreadID;
	//hThread = CreateThread(NULL, 0,
	//	(LPTHREAD_START_ROUTINE)ThreadProc,
	//	(LPVOID)0, 0, &dwThreadID);

	//メモリリーク検出
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	UNREFERENCED_PARAMETER( hPrevInstance );
	UNREFERENCED_PARAMETER( lpCmdLine );

	Window mWindow(hInstance, nCmdShow);
	if (FAILED(mWindow.Init()))
		return 0;

	// Main message loop
	MSG msg = { 0 };
	{
		Application App;
		if (FAILED(App.InitDevice(mWindow)))
		{
			App.CleanupDevice();
			return 0;
		}


		while (WM_QUIT != msg.message)
		//for (int i=0; WM_QUIT != msg.message;i++)
		{
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				App.Render();
			}
		}
		App.CleanupDevice();
	}



	//CloseHandle(hThread);

	return ( int )msg.wParam;
}