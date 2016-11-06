#include "ScriptManager.h"
#include "ScriptCompiler.h"

#ifdef _ENGINE_MODE


void CreateScriptFileExtension(const std::string& classNmae, const std::string& extension) {

	std::fstream file;

	auto srcname = "EngineResource/ScriptTemplate/ScriptTemplate" + extension;
	file.open(srcname, std::ios::in | std::ios::ate);

	std::fstream outFile;
	auto outfilename = "ScriptComponent/Scripts/" + classNmae + extension;
	outFile.open(outfilename, std::ios::out);

	int length = (int)file.tellg();
	file.seekg(0, file.beg);//ファイルポインタを最初に戻す
	char * buf = new char[length + 1];

	memset(buf, 0, (length + 1) * sizeof(char));
	file.read(buf, length);

	std::string buffer = buf;
	std::string from = "$temp$";
	std::string to = classNmae;

	auto pos = buffer.find(from);
	while (pos != std::string::npos) {
		buffer.replace(pos, from.size(), to);
		pos = buffer.find(from, pos + to.size());
	}

	outFile.write(buffer.c_str(), buffer.size());

	file.close();
	outFile.close();

	delete[] buf;
}


//static
void ScriptManager::ReCompile() {
	UseScriptActors::Get()->ReCompile();
}
void ScriptManager::CreateScriptFile(const std::string& className) {
	CreateScriptFileExtension(className, ".h");
	CreateScriptFileExtension(className, ".cpp");
}

#include <thread>
#include "Window/Window.h"

class VSThread {
public:
	VSThread() {

	}
	~VSThread() {
		close();
	}
	bool isOpenVS() {
		return hProcess != NULL;
	}

	void openVS() {

		if (threadVS && threadVS->joinable() && hProcess == NULL) {
			threadVS->detach();
			delete threadVS;
			threadVS = NULL;
		}

		if (threadVS)return;
		threadVS = new std::thread([&]() {
			char cdir[255];
			GetCurrentDirectory(255, cdir);
			std::string  pass = cdir;
			pass += "/ScriptComponent/ScriptComponent.sln";

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
			if (!ShellExecuteEx(&sei) || (const int)sei.hInstApp <= 32) {
				return;
			}

			hProcess = sei.hProcess;
			//終了を待つ
			WaitForSingleObject(sei.hProcess, INFINITE);

			CloseHandle(sei.hProcess);
			hProcess = NULL;
		});
	}
	void openFile(const std::string& filename ,int line) {

		if (threadFile && threadFile->joinable()) {
			threadFile->detach();
			delete threadFile;
			threadFile = NULL;
		}

		if (threadFile)return;
		threadFile = new std::thread([&, filename, line]() {
			char cdir[255];
			GetCurrentDirectory(255, cdir);
			std::string  pass = cdir;
			pass += "/tool/VisualStudioFileOpenTool.exe";

			SHELLEXECUTEINFO	sei = { 0 };
			//構造体のサイズ
			sei.cbSize = sizeof(SHELLEXECUTEINFO);
			//起動側のウインドウハンドル
			sei.hwnd = Window::GetMainHWND();
			//起動後の表示状態
			sei.nShow = SW_SHOWNORMAL;
			//このパラメータが重要で、セットしないとSHELLEXECUTEINFO構造体のhProcessメンバがセットされない。
			sei.fMask = NULL;
			//起動プログラム
			sei.lpFile = pass.c_str();

			auto param = "15 " + filename + " " + std::to_string(line);

			sei.lpParameters = param.c_str();

			for (;;) {
				if (hProcess != NULL)break;
				Sleep(100);
			}

			//プロセス起動
			if (!ShellExecuteEx(&sei) || (const int)sei.hInstApp <= 32) {
				return;
			}
		});
	}
	void close() {
		if (threadVS) {
			if (threadVS->joinable()) {
				threadVS->detach();
			}			
			delete threadVS;
			threadVS = NULL;
		}
		if (threadFile) {
			if (threadFile->joinable()) {
				threadFile->detach();
			}
			delete threadFile;
			threadFile = NULL;
		}
		if (hProcess) {

			CloseHandle(hProcess);
			hProcess = NULL;
		}
	}
	std::thread* threadVS = NULL;
	std::thread* threadFile = NULL;
	HANDLE hProcess = NULL;
};
void ScriptManager::OpenScriptFile(const std::string & fileName, int line)
{
	static VSThread vs;

	if (!vs.isOpenVS()) {
		vs.openVS();
	}

	vs.openFile(fileName,line);
}
#endif