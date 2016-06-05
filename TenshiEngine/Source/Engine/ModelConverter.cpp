#include "ModelConverter.h"

#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

void ModelConverter::Comvert(const std::string& file){
	//	パイプの作成
	HANDLE readPipe;
	HANDLE writePipe;
	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof(sa);
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = NULL;
	if (CreatePipe(&readPipe, &writePipe, &sa, 0) == 0){
		return;
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
	std::string cmd = ".\\tool\\meshconv.exe \"";
	cmd += file + "\" ";

	{
		char drive_[_MAX_DRIVE];
		char dir_[_MAX_DIR];
		char fname_[_MAX_FNAME];
		char ext_[_MAX_EXT];
		_splitpath_s(file.c_str(), drive_, _MAX_DRIVE, dir_, _MAX_DIR, fname_, _MAX_FNAME, ext_, _MAX_EXT);

		cmd += " .\\assets\\" + std::string(fname_)+".tedmesh2 -fz -tan";
	}



	if (!PathFileExists(".\\tool\\meshconv.exe")){
		return;
	}

	//	プロセスの起動
	if (CreateProcess(NULL, (LPSTR)cmd.c_str(), NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi) == 0){
		return;
	}
	HANDLE childProcess = pi.hProcess;
	CloseHandle(pi.hThread);

	bool end = false;
	do{
		DWORD totalLen, len;
		if (WaitForSingleObject(childProcess, 100) == WAIT_OBJECT_0)
			end = true;
		if (PeekNamedPipe(readPipe, NULL, 0, NULL, &totalLen, NULL) == 0)
			break;
	} while (end == false);

	if (CloseHandle(writePipe) == 0){
		return;
	}
	if (CloseHandle(readPipe) == 0){
		return;
	}
	CloseHandle(pi.hProcess);
	return;
}