#include "Component.h"

#include "../ScriptComponent/main.h"
typedef IDllScriptComponent* (__cdecl *CreateInstance_)();
typedef void(__cdecl *DeleteInstance_)(IDllScriptComponent*);

std::map<std::string, std::function<Component*()>> ComponentFactory::mFactoryComponents;
bool ComponentFactory::mIsInit = false;

//同じDLLを持ってるコンポーネントを管理
class UseScriptActors{
public:
	UseScriptActors(){
		mEndReloadDLL = false;
	}
	void ReLoad(){
		if (mEndReloadDLL)return;
		mEndReloadDLL = true;
		for (auto& p : mList){
			p->Unload();
		}

		char cdir[255];
		GetCurrentDirectory(255, cdir);
		std::string  pass = cdir;
#ifdef _DEBUG
		pass += "/ScriptComponent/createdll_auto_d.bat";
#else
		pass += "/ScriptComponent/createdll_auto.bat";
#endif

		SHELLEXECUTEINFO	sei = { 0 };
		//構造体のサイズ
		sei.cbSize = sizeof(SHELLEXECUTEINFO);
		//起動側のウインドウハンドル
		sei.hwnd = Window::GetMainHWND();
		//起動後の表示状態
		sei.nShow = SW_SHOWNORMAL;
		//このパラメータが重要で、セットしないとSHELLEXECUTEINFO構造体のhProcessメンバがセットされない。
		sei.fMask = SEE_MASK_NOCLOSEPROCESS;
		//起動プログラム
		sei.lpFile = pass.c_str();
		//プロセス起動
		if (!ShellExecuteEx(&sei) || (const int)sei.hInstApp <= 32){
			MessageBox(Window::GetMainHWND(), "ファイルを開けませんでした", "失敗", MB_OK);
			return;
		}
		//終了を待つ
		WaitForSingleObject(sei.hProcess, INFINITE);


		for (auto& p : mList){
			p->Load();
		}
	}
	std::list<ScriptComponent*> mList;

	bool mEndReloadDLL;
};
static UseScriptActors actors;

ScriptComponent::ScriptComponent(){
	pDllClass = NULL;
	hModule = NULL;
	mCreate = NULL;
	mDelete = NULL;

	Load();

	actors.mList.push_back(this);

}
ScriptComponent::~ScriptComponent(){
	Unload();
	actors.mList.remove(this);
}

void ScriptComponent::Load(){
	if (pDllClass)return;
		// DLLのロード
#ifdef _DEBUG
	hModule = LoadLibrary("../ScriptComponent/Debug/ScriptComponent.dll");
#else
	hModule = LoadLibrary("../ScriptComponent/Release/ScriptComponent.dll");
#endif
		if (hModule == NULL)
		{
#ifdef _DEBUG
			hModule = LoadLibrary("ScriptComponent/Debug/ScriptComponent.dll");
#else
			hModule = LoadLibrary("ScriptComponent/Release/ScriptComponent.dll");
#endif
		}


		mCreate = (CreateInstance_)GetProcAddress(hModule, "CreateInstance");
		if (mCreate == NULL)
		{
			FreeLibrary(hModule);
			hModule = NULL;
			return;
		}
		mDelete = (DeleteInstance_)GetProcAddress(hModule, "ReleseInstance");
		if (mDelete == NULL)
		{
			mCreate = NULL;
			FreeLibrary(hModule);
			hModule = NULL;
			return;
		}

		//dllで作成したクラスインスタンスを作成する
		pDllClass = ((CreateInstance_)mCreate)();
	}

void ScriptComponent::Unload(){

		if (pDllClass)
			((DeleteInstance_)mDelete)(pDllClass);

		FreeLibrary(hModule);
		hModule = NULL;
		mCreate = NULL;
		mDelete = NULL;
		pDllClass = NULL;
	}
void ScriptComponent::ReCompile(){
		Unload();
		Load();

	}
void ScriptComponent::Update(){

		if (pDllClass){
			pDllClass->Update(gameObject);
		}

		if (Input::Trigger(KeyCoord::Key_R)){
			actors.ReLoad();
			return;
			Unload();

			//STARTUPINFO si;// スタートアップ情報
			//PROCESS_INFORMATION pi;// プロセス情報
			//
			////STARTUPINFO 構造体の内容を取得 
			//GetStartupInfo(&si);
			//
			//CreateProcess(
			//	NULL,					// 実行可能モジュールの名前
			//	"cmd.exe cd ./../ScriptComponent/ cureatedll.bat",			// コマンドラインの文字列
			//	NULL,					// セキュリティ記述子
			//	NULL,					// セキュリティ記述子
			//	FALSE,					// ハンドルの継承オプション
			//	CREATE_NO_WINDOW,				// 作成のフラグ 
			//	//CREATE_NEW_PROCESS_GROUP	: 新たなプロセス
			//	//CREATE_NEW_CONSOLE		: 新しいコンソールで実行
			//	//CREATE_NO_WINDOW			: コンソールウィンドウなしでアプリケーションを実行
			//	NULL,					// 新しい環境ブロック
			//	NULL,					// カレントディレクトリの名前
			//	&si,					// スタートアップ情報
			//	&pi					// プロセス情報
			//	);

			//ShellExecute(Window::GetHWND(), "open", "c:Program Files\\Wireshark\\test.bat", SW_SHOW);
			char cdir[255];
			GetCurrentDirectory(255, cdir);
			std::string  pass = cdir;
#ifdef _DEBUG
			pass += "/ScriptComponent/createdll_auto_d.bat";
#else
			pass += "/ScriptComponent/createdll_auto.bat";
#endif
			//if (ShellExecute(Window::GetHWND(), NULL, pass.c_str(), NULL, NULL, SW_SHOWNORMAL) <= (HINSTANCE)32)
			//	MessageBox(Window::GetHWND(), "ファイルを開けませんでした", "失敗", MB_OK);

			SHELLEXECUTEINFO	sei = { 0 };
			//構造体のサイズ
			sei.cbSize = sizeof(SHELLEXECUTEINFO);
			//起動側のウインドウハンドル
			sei.hwnd = Window::GetMainHWND();
			//起動後の表示状態
			sei.nShow = SW_SHOWNORMAL;
			//このパラメータが重要で、セットしないとSHELLEXECUTEINFO構造体のhProcessメンバがセットされない。
			sei.fMask = SEE_MASK_NOCLOSEPROCESS;
			//起動プログラム
			sei.lpFile = pass.c_str();
			//プロセス起動
			if (!ShellExecuteEx(&sei) || (const int)sei.hInstApp <= 32){
				MessageBox(Window::GetMainHWND(), "ファイルを開けませんでした", "失敗", MB_OK);
				return;
			}
			//終了を待つ
			WaitForSingleObject(sei.hProcess, INFINITE);

			Load();

		}
		else{
			actors.mEndReloadDLL = false;
		}
	}


#include "Game.h"
void MeshDrawComponent::Update(){

	if (!mModel){
		mModel = gameObject->GetComponent<ModelComponent>();
		if (!mModel)
			mModel = gameObject->GetComponent<TextureModelComponent>();
		if (!mModel)return;
	}
	if (!mModel->mModel)return;

	if (!mMaterial){
		mMaterial = gameObject->GetComponent<MaterialComponent>();
	}
	if (!mMaterial)return;

	Game::AddDrawList(DrawStage::Diffuse, std::function<void()>([&](){
		mModel->SetMatrix();
		Model& model = *mModel->mModel;

		if (mWriteDepth){
			model.Draw(mMaterial);
		}
		else{
			NoWriteDepth(model);
		}
	}));

}