
#include <list>
#include <iostream>
#include <algorithm>
#include <filesystem> // std::tr2::sys::path etc.

#include "ScriptComponent.h"
#include "MySTL/Reflection/Reflection.h"
#include "Window/window.h"

#include "../ScriptComponent/main.h"
typedef IDllScriptComponent* (__cdecl *CreateInstance_)(const char*);
typedef void(__cdecl *DeleteInstance_)(IDllScriptComponent*);

typedef decltype(Reflection::map) (__cdecl *GetReflectionData_)();

#include "Game/Script/SGame.h"
SGame gSGame;

void CreateScriptFileExtension(const std::string& classNmae, const std::string& extension){

	std::fstream file;

	auto srcname = "EngineResource/ScriptTemplate/ScriptTemplate" + extension;
	file.open(srcname, std::ios::in | std::ios::ate);

	std::fstream outFile;
	auto outfilename = "ScriptComponent/Scripts/"+ classNmae + extension;
	outFile.open(outfilename, std::ios::out);

	int length = file.tellg();
	file.seekg(0, file.beg);//ファイルポインタを最初に戻す
	char * buf = new char[length+1];

	memset(buf, 0, (length+1)*sizeof(char));
	file.read(buf, length);

	std::string buffer = buf;
	std::string from = "$temp$";
	std::string to = classNmae;

	auto pos = buffer.find(from);
	while (pos != std::string::npos){
		buffer.replace(pos, from.size(), to);
		pos = buffer.find(from, pos + to.size());
	}

	outFile.write(buffer.c_str(), buffer.size());

	file.close();
	outFile.close();

	delete[] buf;
}



std::string outConsole;
bool create_cmd_process(HWND hWnd){
	//	パイプの作成
	HANDLE readPipe;
	HANDLE writePipe;
	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof(sa);
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = NULL;
	if (CreatePipe(&readPipe, &writePipe, &sa, 0) == 0){
		MessageBox(0, "パイプが作成できませんでした", "エラー", MB_OK);
		return false;
	}
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	ZeroMemory(&pi, sizeof(pi));
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
	si.hStdOutput = writePipe;
	si.hStdError = writePipe;
	si.wShowWindow = SW_HIDE;
#ifdef _DEBUG
	char cmd[] = "cmd.exe /K \".\\ScriptComponent\\createdll_auto_d.bat\"";
#else
	char cmd[] = "cmd.exe /K \".\\ScriptComponent\\createdll_auto.bat\"";
#endif
	//	プロセスの起動(cmd.exe)
	if (CreateProcess(NULL, cmd, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi) == 0){
		MessageBox(0, "プロセスの作成に失敗しました", "エラー", MB_OK);
		return false;
	}
	HANDLE childProcess = pi.hProcess;
	CloseHandle(pi.hThread);

	outConsole.reserve(20000);
	char readBuf[1025];
	bool end = false;
	do{
		DWORD totalLen, len;
		if (WaitForSingleObject(childProcess, 100) == WAIT_OBJECT_0)
			end = true;
		if (PeekNamedPipe(readPipe, NULL, 0, NULL, &totalLen, NULL) == 0)
			break;
		if (0 < totalLen){
			if (ReadFile(readPipe, readBuf, sizeof(readBuf) - 1, &len, NULL) == 0)
				return false;
			readBuf[len] = 0;

			//if (sizeof(mem) - 1<memSz + len){	//メモリがあふれないようにクリアする
			//	mem[0] = 0;
			//	memSz = 0;
			//}
			//memSz += len;
			//strcat_s(mem, sizeof(mem), readBuf);
			outConsole += readBuf;
			//SendMessage(hWnd, WM_VSCROLL, SB_BOTTOM, 0);	//	スクロールバーを一番下へ移動させる
			if (totalLen>len)	//	プロセスは終了しているがまだデータがーが残っているので終了を保留
				end = false;
		}
	} while (end == false);

	if (CloseHandle(writePipe) == 0){
		MessageBox(0, "パイプを閉じることができませんでした。", "エラー", MB_OK);
		return false;
	}
	if (CloseHandle(readPipe) == 0){
		MessageBox(0, "パイプを閉じることができませんでした。", "エラー", MB_OK);
		return false;
	}
	CloseHandle(pi.hProcess);
	for (int i = 0; i < 100; i++){
		auto out = forward_than_find_first_of(outConsole, "\n");
		outConsole = behind_than_find_first_of(outConsole, "\n");
		if (out == "")break;
		out.pop_back();
		Window::AddLog(out);
	}
	return true;
}

//同じDLLを持ってるコンポーネントを管理
class UseScriptActors{
public:
	UseScriptActors(){
		hModule = NULL;
		mCreate = NULL;
		mDelete = NULL;
		mGetReflect = NULL;

	}
	~UseScriptActors(){
		UnLoad();
	}
	void ReCompile(){
		for (auto& p : mList){
			p->Unload();
		}

		UnLoad();

		CreateIncludeClassFile();

		char cdir[255];
		GetCurrentDirectory(255, cdir);
		std::string  pass = cdir;
#ifdef _DEBUG
		pass += "/ScriptComponent/createdll_auto_d.bat";
#else
		pass += "/ScriptComponent/createdll_auto.bat";
#endif

		//SHELLEXECUTEINFO	sei = { 0 };
		////構造体のサイズ
		//sei.cbSize = sizeof(SHELLEXECUTEINFO);
		////起動側のウインドウハンドル
		//sei.hwnd = Window::GetMainHWND();
		////起動後の表示状態
		//sei.nShow = SW_SHOWNORMAL;
		////このパラメータが重要で、セットしないとSHELLEXECUTEINFO構造体のhProcessメンバがセットされない。
		//sei.fMask = SEE_MASK_NOCLOSEPROCESS;
		////起動プログラム
		//sei.lpFile = pass.c_str();
		////プロセス起動
		//if (!ShellExecuteEx(&sei) || (const int)sei.hInstApp <= 32){
		//	MessageBox(Window::GetMainHWND(), "ファイルを開けませんでした", "失敗", MB_OK);
		//	return;
		//}
		//
		//
		////終了を待つ
		//WaitForSingleObject(sei.hProcess, INFINITE);
		//
		//CloseHandle(sei.hProcess);


		create_cmd_process(Window::GetMainHWND());

		DllLoad();

		for (auto& p : mList){
			p->Load();
		}
	}

	void CreateIncludeClassFile(){

		File includesFile("./ScriptComponent/System/include");
		includesFile.FileCreate();
		File factorysFile("./ScriptComponent/System/factory");
		factorysFile.FileCreate();

		namespace sys = std::tr2::sys;
		sys::path p("./ScriptComponent/Scripts/"); // 列挙の起点
		//std::for_each(sys::directory_iterator(p), sys::directory_iterator(),
			//  再帰的に走査するならコチラ↓
		std::for_each(sys::recursive_directory_iterator(p), sys::recursive_directory_iterator(),
			[&](const sys::path& p) {
			if (sys::is_regular_file(p)) { // ファイルなら...
				if (p.extension() == ".h"){
					auto out = "\n#include\"../" + p.directory_string() + "\"";
					includesFile.Out(out);

					auto outf = "\n_ADD(" + p.stem() + ");";
					factorysFile.Out(outf);
				}
			}
			else if (sys::is_directory(p)) { // ディレクトリなら...
				//std::cout << "dir.: " << p.string() << std::endl;
			}
		});
	}

	void UnLoad(){
		FreeLibrary(hModule);
		hModule = NULL;
		mCreate = NULL;
		mDelete = NULL;
		mGetReflect = NULL;
		Reflection::map = NULL;
	}
	void DllLoad(){

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
			UnLoad();
			return;
		}
		mDelete = (DeleteInstance_)GetProcAddress(hModule, "ReleseInstance");
		if (mDelete == NULL)
		{
			UnLoad();
			return;
		}
		mGetReflect = (GetReflectionData_)GetProcAddress(hModule, "GetReflectionData");
		if (mGetReflect == NULL)
		{
			UnLoad();
			return;
		}

		Reflection::map = ((GetReflectionData_)mGetReflect)();
	}
	IDllScriptComponent* Create(const std::string& ClassName){
		if (!mCreate)return NULL;
		//dllで作成したクラスインスタンスを作成する
		return ((CreateInstance_)mCreate)(ClassName.c_str());
	}

	void Deleter(IDllScriptComponent* script){
		((DeleteInstance_)mDelete)(script);
	}
	std::list<ScriptComponent*> mList;
	void* mCreate;
	void* mDelete;
	void* mGetReflect;
	HMODULE hModule;

};

UseScriptActors actors;


//static
void ScriptManager::ReCompile(){
	actors.ReCompile();
}
void ScriptManager::CreateScriptFile(const std::string& className){
	CreateScriptFileExtension(className, ".h");
	CreateScriptFileExtension(className, ".cpp");
}

ScriptComponent::ScriptComponent(){
	mEndInitialize = false;
	mEndStart = false;
	pDllClass = NULL;
}
ScriptComponent::~ScriptComponent(){
}
void ScriptComponent::Initialize(){
	Load();
	actors.mList.push_back(this);
	mEndInitialize = true;
	if (pDllClass){
		pDllClass->Initialize();
	}
}
void ScriptComponent::Load(){
	if (pDllClass)return;

	//dllで作成したクラスインスタンスを作成する
	pDllClass = actors.Create(mClassName);

	if (pDllClass){
		pDllClass->game = &gSGame;
		pDllClass->gameObject = gameObject;

		if (mEndInitialize)pDllClass->Initialize();
		if (mEndStart)pDllClass->Start();
	}
}
void ScriptComponent::Unload(){

	if (pDllClass){
		pDllClass->Finish();
		actors.Deleter(pDllClass);
	}

	pDllClass = NULL;

}
void ScriptComponent::ReCompile(){
	Unload();
	Load();

}
void ScriptComponent::Start(){
	mEndStart = true;
	if (pDllClass){
		pDllClass->Start();
	}
}
void ScriptComponent::Update(){

	if (pDllClass){
		pDllClass->Update();
	}
}
void ScriptComponent::Finish(){

	Unload();
	actors.mList.remove(this);
	mEndInitialize = false;
	mEndStart = false;
}

void ScriptComponent::OnCollide(Actor* target){
	if (pDllClass){
		pDllClass->OnCollide(target);
	}
}


void ScriptComponent::CreateInspector(){

	auto data = Window::CreateInspector();
	std::function<void(std::string)> collback = [&](std::string name){
		mClassName = name;
		ReCompile();
	};
	Window::AddInspector(new TemplateInspectorDataSet<std::string>("Class", &mClassName, collback), data);

	if (Reflection::map){
		auto infos = Reflection::GetMemberInfos(pDllClass, mClassName);

		for (auto i : infos){
			auto info = Reflection::GetMemberInfo(infos,i);

			if (info.GetTypeName() == "float"){
				float* paramdata = Reflection::Get<float>(info);

				std::function<void(float)> collbackfloat = [=](float f){
					*paramdata = f;
				};
				Window::AddInspector(new TemplateInspectorDataSet<float>(info.GetName(), paramdata, collbackfloat), data);
			}
			if (info.GetTypeName() == "bool"){
				bool* paramdata = Reflection::Get<bool>(info);

				std::function<void(bool)> collbackfloat = [=](bool f){
					*paramdata = f;
				};
				Window::AddInspector(new TemplateInspectorDataSet<bool>(info.GetName(), paramdata, collbackfloat), data);
			}
			if (info.GetTypeName() == "int"){
				int* paramdata = Reflection::Get<int>(info);

				std::function<void(int)> collbackfloat = [=](int f){
					*paramdata = f;
				};
				Window::AddInspector(new TemplateInspectorDataSet<int>(info.GetName(), paramdata, collbackfloat), data);
			}
		}

	}

	Window::ViewInspector("Script", this, data);
}

void ScriptComponent::IO_Data(I_ioHelper* io){
#define _KEY(x) io->func( x , #x)
	_KEY(mClassName);

#undef _KEY
}