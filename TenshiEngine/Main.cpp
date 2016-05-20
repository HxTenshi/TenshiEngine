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

#include "Application/DefineDrawMultiThread.h"


#include "Engine/Profiling.h"

class InputManagerRapper{
public:

	void Initialize(HWND hWnd, HINSTANCE hInstance){
		mScreenFocus = false;
		mr = false;
		ml = false;
		mx = 0;
		my = 0;
		wx = 1;
		wy = 1;

		InputManager::InitDirectInput(hWnd, hInstance);
		Window::SetMouseEvents(&mScreenFocus,&ml, &mr, &mx, &my, &wx, &wy);
	}

	void Update(){

		InputManager::SetMouseL(ml);
		InputManager::SetMouseR(mr);
		int x = (int)((float)mx / wx * WindowState::mWidth);
		int y = (int)((float)my / wy * WindowState::mHeight);
		InputManager::SetMouseXY(x, y);
		InputManager::Update(mScreenFocus);
	}

	void Release(){
		InputManager::Release();
	}

private:
	bool mScreenFocus;
	bool mr, ml;
	int mx, my;
	int wx, wy;
};

#include <memory>


#pragma comment(lib,"lib/Debug/sqlite3.lib")
#include "Library\sqlite3.h"
class db{
	db(){

		// DBファイル名
		char *fileName = "DB/SQLiteDB";
		// DBオブジェクト
		sqlite3 *pDB = NULL;
		// DBのオープン
		int err = sqlite3_open(fileName, &pDB);
		if (err != SQLITE_OK){
			/* エラー処理 */
		}

		// テーブルの作成
		char *errMsg = NULL;

		err = sqlite3_exec(pDB,
			"CREATE TABLE IF NOT EXISTS myTable "
			"(id INTEGER PRIMARY KEY AUTOINCREMENT, "
			"name CHAR(32) NOT NULL)",
			NULL, NULL, &errMsg);

		if (err != SQLITE_OK){
			printf("%s\n", errMsg);
			sqlite3_free(errMsg);
			errMsg = NULL;
			/* TODO: エラー処理 */
		}

		// ステートメントオブジェクト
		sqlite3_stmt *pStmt = NULL;

		// データの追加
		// ステートメントの用意
		err = sqlite3_prepare_v2(pDB,
			"INSERT INTO myTable (name) VALUES (?)",
			128, &pStmt, NULL);

		if (err != SQLITE_OK){
			/* TODO:エラー処理 */
		}
		else{
			int i = 0;
			for (i = 0; i < 5; i++){
				std::string name = "test" + std::to_string(i);

				// ?の部分に値を代入
				sqlite3_bind_text(pStmt, 1, name.c_str(), name.length(), SQLITE_STATIC);
				while (SQLITE_DONE != sqlite3_step(pStmt)){}
			}
		}

		// ステートメントの解放
		sqlite3_finalize(pStmt);

		// データの抽出
		// ステートメントの用意
		err = sqlite3_prepare_v2(pDB,
			"SELECT * FROM myTable", 64,
			&pStmt, NULL);

		if (err != SQLITE_OK){
			/* TODO:エラー処理 */
		}
		else{
			// データの抽出
			while (SQLITE_ROW == (err = sqlite3_step(pStmt))){
				int id = sqlite3_column_int(pStmt, 0);
				const unsigned char *name = sqlite3_column_text(pStmt, 1);
				printf("id: %d, name: %s\n", id, name);
			}

			if (err != SQLITE_DONE){
				/* TODO: エラー処理 */
			}
		}

		// ステートメントの解放
		sqlite3_finalize(pStmt);

		// DBのクローズ
		err = sqlite3_close(pDB);
		if (err != SQLITE_OK){
			/* TODO:エラー処理 */
		}
	}
};


#include <thread>
#include <mutex>
#include <condition_variable>

#include "Application\DrawThreadQueue.h"

class Application{
public:
	Application()
		:mGame(NULL)
		, mDestory(false)
		, mCmdFlag(false)
		, mCmdList(NULL)
	{
	}

	HRESULT InitDevice(const Window& window)
	{
		HRESULT hr = S_OK;

		hr = Device::Init(window);
		if (FAILED(hr))
			return hr;

		mInputManagerRapper.Initialize(window.GetMainHWND(), window.mhInstance);

		mGame = new Game();

#if _DRAW_MULTI_THREAD
		mUpdateEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		mDrawEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		ResetEvent(mDrawEvent);
		ResetEvent(mUpdateEvent);

		mDrawThread = std::thread(std::bind(std::mem_fn(&Application::DrawThread), this));
#else
#endif

		return S_OK;
	}

	void Render()
	{

		{
			auto tick = Profiling::Start("Main:Update");
			mInputManagerRapper.Update();
			mGame->Update();
		}
		{
			auto tick = Profiling::Start("Main:Draw");
			mGame->Draw();
		}


#if _DRAW_MULTI_THREAD
		ID3D11CommandList* cmdList;
		{
			auto tick = Profiling::Start("Main:etc");
			auto render = RenderingEngine::GetEngine(ContextType::MainDeferrd);
			render->m_Context->FinishCommandList(false, &cmdList);
			SetEvent(mUpdateEvent);
		}

		{
			auto tick = Profiling::Start("Main:wait");
			WaitForSingleObject(mDrawEvent, INFINITE);
		}
		ResetEvent(mDrawEvent);
		mCmdList = cmdList;
		mCmdFlag = true;
#else
		auto size = DrawThreadQueue::size();
		for (int i = 0; i < size; i++){
			auto func = DrawThreadQueue::dequeue();
			func();
		}
		Device::mpSwapChain->Present(1, 0);
#endif
	}


	void CleanupDevice()
	{

#if _DRAW_MULTI_THREAD

		mDestory = true;
		SetEvent(mUpdateEvent);
		mDrawThread.join();

		//スレッド終了
		CloseHandle(mUpdateEvent);
		CloseHandle(mDrawEvent);
#else
#endif


		if (mGame)delete mGame;

		mInputManagerRapper.Release();

		Device::CleanupDevice();

		Window::Release();
	}
private:

	void DrawThread(){
		while (!mDestory)
		{

			{
				auto tick = Profiling::Start("Draw:wait");
				WaitForSingleObject(mUpdateEvent, INFINITE);
			}
			ID3D11CommandList* cmdList;
			{
				auto tick = Profiling::Start("Draw:etc");
				ResetEvent(mUpdateEvent);
				SetEvent(mDrawEvent);

				while (!mDestory)
				{
					if (mCmdFlag || mDestory)break;
				}
				if (mDestory)break;
				cmdList = (ID3D11CommandList*)mCmdList;
				mCmdFlag = false;

				auto size = DrawThreadQueue::size();
				for (int i = 0; i < size; i++){
					auto func = DrawThreadQueue::dequeue();
					func();
				}
			}
			{
				auto tick = Profiling::Start("Draw:execute");
				Device::mpImmediateContext->ExecuteCommandList(cmdList, false);
				cmdList->Release();
			}
			{
				auto tick = Profiling::Start("Draw:swap");
				Device::mpSwapChain->Present(1, 0);
			}
		}

		SetEvent(mDrawEvent);
	}

	InputManagerRapper mInputManagerRapper;
	Game* mGame;

public:
	volatile bool mDestory;
	volatile bool mCmdFlag;
	volatile ID3D11CommandList* mCmdList;
	HANDLE mUpdateEvent;
	HANDLE mDrawEvent;
	std::thread mDrawThread;

};


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


#include "MySTL\Coroutine.h"


int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
{

	//メモリリーク検出
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	UNREFERENCED_PARAMETER( hPrevInstance );
	UNREFERENCED_PARAMETER( lpCmdLine );

	//メモリーリークにブレークポイント
	//_CrtSetBreakAlloc(8359);
	

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

	return ( int )msg.wParam;
}
