

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

#include "Game/Component/MovieComponent.h"

#include "Engine/Profiling.h"

#include <memory>

#include "Application\Shutdown.h"


//#pragma comment(lib,"lib/Debug/sqlite3.lib")
//#include "Library\sqlite3.h"
//class db{
//	db(){
//
//		// DBファイル名
//		char *fileName = "DB/SQLiteDB";
//		// DBオブジェクト
//		sqlite3 *pDB = NULL;
//		// DBのオープン
//		int err = sqlite3_open(fileName, &pDB);
//		if (err != SQLITE_OK){
//			/* エラー処理 */
//		}
//
//		// テーブルの作成
//		char *errMsg = NULL;
//
//		err = sqlite3_exec(pDB,
//			"CREATE TABLE IF NOT EXISTS myTable "
//			"(id INTEGER PRIMARY KEY AUTOINCREMENT, "
//			"name CHAR(32) NOT NULL)",
//			NULL, NULL, &errMsg);
//
//		if (err != SQLITE_OK){
//			printf("%s\n", errMsg);
//			sqlite3_free(errMsg);
//			errMsg = NULL;
//			/* TODO: エラー処理 */
//		}
//
//		// ステートメントオブジェクト
//		sqlite3_stmt *pStmt = NULL;
//
//		// データの追加
//		// ステートメントの用意
//		err = sqlite3_prepare_v2(pDB,
//			"INSERT INTO myTable (name) VALUES (?)",
//			128, &pStmt, NULL);
//
//		if (err != SQLITE_OK){
//			/* TODO:エラー処理 */
//		}
//		else{
//			int i = 0;
//			for (i = 0; i < 5; i++){
//				std::string name = "test" + std::to_string(i);
//
//				// ?の部分に値を代入
//				sqlite3_bind_text(pStmt, 1, name.c_str(), name.length(), SQLITE_STATIC);
//				while (SQLITE_DONE != sqlite3_step(pStmt)){}
//			}
//		}
//
//		// ステートメントの解放
//		sqlite3_finalize(pStmt);
//
//		// データの抽出
//		// ステートメントの用意
//		err = sqlite3_prepare_v2(pDB,
//			"SELECT * FROM myTable", 64,
//			&pStmt, NULL);
//
//		if (err != SQLITE_OK){
//			/* TODO:エラー処理 */
//		}
//		else{
//			// データの抽出
//			while (SQLITE_ROW == (err = sqlite3_step(pStmt))){
//				int id = sqlite3_column_int(pStmt, 0);
//				const unsigned char *name = sqlite3_column_text(pStmt, 1);
//				printf("id: %d, name: %s\n", id, name);
//			}
//
//			if (err != SQLITE_DONE){
//				/* TODO: エラー処理 */
//			}
//		}
//
//		// ステートメントの解放
//		sqlite3_finalize(pStmt);
//
//		// DBのクローズ
//		err = sqlite3_close(pDB);
//		if (err != SQLITE_OK){
//			/* TODO:エラー処理 */
//		}
//	}
//};


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

	HRESULT InitDevice(Window* window)
	{

		_SYSTEM_LOG_H("アプリケーションの初期化");
		HRESULT hr = S_OK;
		mWindow = window;

		hr = Device::Init(*window);
		if (FAILED(hr))
			return hr;

		//mInputManagerRapper.Initialize(window.GetMainHWND(), window.mhInstance);

		mGame = new Game();

#if _DRAW_MULTI_THREAD
		mUpdateEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		mDrawEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		//mMessageEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		ResetEvent(mDrawEvent);
		ResetEvent(mUpdateEvent);
		//ResetEvent(mMessageEvent);

		mDrawThread = std::thread(std::bind(std::mem_fn(&Application::UpdateThread), this));
#else
#endif

		return S_OK;
	}


#if _DRAW_MULTI_THREAD

	void UpdateThread(){
		while (!mDestory)
		{
			{
				auto tick = Profiling::Start("Main:Update");
				//mInputManagerRapper.Update();
				mWindow->Update();
				mGame->Update();
			}
			{
				auto tick = Profiling::Start("Main:Draw");
				mGame->Draw();
			}

			ID3D11CommandList* cmdList = NULL;
			{
				auto tick = Profiling::Start("Main:etc");
				auto render = RenderingEngine::GetEngine(ContextType::MainDeferrd);
				if (FAILED(render->m_Context->FinishCommandList(false, &cmdList))){
					cmdList = NULL;
				}
				SetEvent(mUpdateEvent);
			}
			{
				auto tick = Profiling::Start("Main:wait");
				WaitForSingleObject(mDrawEvent, INFINITE);
			}
				ResetEvent(mDrawEvent);
				if (cmdList){
					mCmdList = cmdList;
					mCmdFlag = true;
				}
		}

		SetEvent(mDrawEvent);
	}


	void Render()
	{

		while (!App::IsShutdown()){

			{
				auto tick = Profiling::Start("Draw:wait");
				WaitForSingleObject(mUpdateEvent, INFINITE);
				ResetEvent(mUpdateEvent);
			}

			{
				// Main message loop
				MSG msg = { 0 };
				while (WM_QUIT != msg.message)
				{
					if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
					{
						TranslateMessage(&msg);
						DispatchMessage(&msg);
					}
					else
					{
						break;
					}
				}
				if (WM_QUIT == msg.message){
					mDestory = true;
					SetEvent(mDrawEvent);
					return;
				}
			}

			ID3D11CommandList* cmdList;
			{
				auto tick = Profiling::Start("Draw:etc");
				SetEvent(mDrawEvent);

				while (!mCmdFlag)
				{
					Sleep(1);
				}

				cmdList = (ID3D11CommandList*)mCmdList;
				mCmdFlag = false;

				//auto size = DrawThreadQueue::size();
				//for (int i = 0; i < size; i++){
				//	auto func = DrawThreadQueue::dequeue();
				//	func();
				//}
			}
			{
				auto tick = Profiling::Start("Draw:execute");
				Device::mpImmediateContext->ExecuteCommandList(cmdList, false);
				cmdList->Release();
			}
			{
				auto tick = Profiling::Start("Draw:swap");

				Device::mpSwapChain->Present(0, 0);

			}
		}
	}
#else
	void Render()
	{

		// Main message loop
		MSG msg = { 0 };
		while (WM_QUIT != msg.message)
		{
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				{
					auto tick = Profiling::Start("Main:Update");
					mWindow->Update();
					//mInputManagerRapper.Update();
					mGame->Update();
				}
				{
					auto tick = Profiling::Start("Main:Draw");
					mGame->Draw();
				}
	
				//auto size = DrawThreadQueue::size();
				//for (int i = 0; i < size; i++){
				//	auto func = DrawThreadQueue::dequeue();
				//	func();
				//}
				//if (!MoviePlayFlag::IsMoviePlay()){
					Device::mpSwapChain->Present(0, 0);
				//}

					Device::mpImmediateContext->ClearState();

					// Setup the viewport
					D3D11_VIEWPORT vp;
					vp.Width = (FLOAT)WindowState::mWidth;
					vp.Height = (FLOAT)WindowState::mHeight;
					vp.MinDepth = 0.0f;
					vp.MaxDepth = 1.0f;
					vp.TopLeftX = 0;
					vp.TopLeftY = 0;
					Device::mpImmediateContext->RSSetViewports(1, &vp);

					Device::mpImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			}
		}
	}
#endif

//	void Render()
//	{
//
//		{
//			auto tick = Profiling::Start("Main:Update");
//			mInputManagerRapper.Update();
//			mGame->Update();
//		}
//		{
//			auto tick = Profiling::Start("Main:Draw");
//			mGame->Draw();
//		}
//
//
//#if _DRAW_MULTI_THREAD
//		ID3D11CommandList* cmdList;
//		{
//			auto tick = Profiling::Start("Main:etc");
//			auto render = RenderingEngine::GetEngine(ContextType::MainDeferrd);
//			render->m_Context->FinishCommandList(false, &cmdList);
//			SetEvent(mUpdateEvent);
//		}
//
//		{
//			auto tick = Profiling::Start("Main:wait");
//			WaitForSingleObject(mDrawEvent, INFINITE);
//		}
//		ResetEvent(mDrawEvent);
//		mCmdList = cmdList;
//		mCmdFlag = true;
//#else
//		auto size = DrawThreadQueue::size();
//		for (int i = 0; i < size; i++){
//			auto func = DrawThreadQueue::dequeue();
//			func();
//		}
//		//if (!MoviePlayFlag::IsMoviePlay()){
//			Device::mpSwapChain->Present(1, 0);
//		//}
//#endif
//	}


	void CleanupDevice()
	{

#if _DRAW_MULTI_THREAD

		mDestory = true;
		SetEvent(mDrawEvent);
		mDrawThread.join();

		//スレッド終了
		CloseHandle(mUpdateEvent);
		CloseHandle(mDrawEvent);
#else
#endif


		if (mGame)delete mGame;

		Device::CleanupDevice();

	}
private:

	//void DrawThread(){
	//	while (!mDestory)
	//	{
	//
	//		{
	//			auto tick = Profiling::Start("Draw:wait");
	//			WaitForSingleObject(mUpdateEvent, INFINITE);
	//		}
	//		ID3D11CommandList* cmdList;
	//		{
	//			auto tick = Profiling::Start("Draw:etc");
	//			ResetEvent(mUpdateEvent);
	//			SetEvent(mDrawEvent);
	//
	//			while (!mDestory)
	//			{
	//				if (mCmdFlag || mDestory)break;
	//			}
	//			if (mDestory)break;
	//			cmdList = (ID3D11CommandList*)mCmdList;
	//			mCmdFlag = false;
	//
	//			auto size = DrawThreadQueue::size();
	//			for (int i = 0; i < size; i++){
	//				auto func = DrawThreadQueue::dequeue();
	//				func();
	//			}
	//		}
	//		{
	//			auto tick = Profiling::Start("Draw:execute");
	//			Device::mpImmediateContext->ExecuteCommandList(cmdList, false);
	//			cmdList->Release();
	//		}
	//		{
	//			auto tick = Profiling::Start("Draw:swap");
	//
	//			if (!MoviePlayFlag::IsMoviePlay()){
	//				Device::mpSwapChain->Present(0, 0);
	//			}
	//		}
	//	}
	//
	//	SetEvent(mDrawEvent);
	//}

	Game* mGame;

public:
	volatile bool mDestory;
	volatile bool mCmdFlag;
	volatile ID3D11CommandList* mCmdList;
	HANDLE mUpdateEvent;
	//HANDLE mMessageEvent;
	HANDLE mDrawEvent;
	std::thread mDrawThread;

	Window* mWindow;

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

#ifdef _ENGINE_MODE
	if (FindWindow("TenshiEngineDummyWindowClass", NULL) != NULL){ /* 二重起動防止 */
		return FALSE;
	}
#else
	if (FindWindow("GameWindowClass", NULL) != NULL){ /* 二重起動防止 */
		return FALSE;
	}
#endif

	//メモリリーク検出
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	UNREFERENCED_PARAMETER( hPrevInstance );
	UNREFERENCED_PARAMETER( lpCmdLine );

	//メモリーリークにブレークポイント
	//_CrtSetBreakAlloc(669);

	SystemLog::Create();

	_SYSTEM_LOG_H("メイン");
	
	Window mWindow(hInstance, nCmdShow);
	if (FAILED(mWindow.Init()))
		return 0;

	{
		Application App;
		if (FAILED(App.InitDevice(&mWindow)))
		{
			App.CleanupDevice();
			mWindow.Release();
			return 0;
		}

		
		App.Render();

		App.CleanupDevice();
		mWindow.Release();
	}

	return 0;// (int)msg.wParam;
}
