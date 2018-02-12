
#include "ScriptCompiler.h"
#include <list>
#include <iostream>
#include <algorithm>
#include <filesystem> // std::tr2::sys::path etc.

#include "Application/BuildVersion.h"

#include "Game/Component/ScriptComponent.h"
#include "MySTL/Reflection/Reflection.h"
#include "Window/window.h"
#include "Engine/Inspector.h"
#include "Engine/AssetLoad.h"

#include "../ScriptComponent/main.h"
typedef IDllScriptComponent* (__cdecl *CreateInstance_)(const char*);
typedef void(__cdecl *DeleteInstance_)(IDllScriptComponent*);
typedef decltype(Reflection::map) (__cdecl *GetReflectionData_)();

typedef bool(__cdecl *IDllFunction0)(IDllScriptComponent*, IDllScriptComponent::Func0);
typedef bool(__cdecl *IDllFunction1)(IDllScriptComponent*, IDllScriptComponent::Func1, GameObject);
typedef void(__cdecl *InitIGame)(IGame*);

#include "Game/Script/SGame.h"
SGame gSGame;


#define _SKIP_COMPILE 0

#ifdef _ENGINE_MODE
// �v���W�F�N�g�t�@�C���̐���
void IncludeScriptFileProject()
{
	// �x�[�X�ɂ���v���W�F�N�g�t�@�C���i��j
	std::fstream headFile;
	auto headFilePath = "EngineResource/ScriptTemplate/ScriptTemplate.vcxproj";
	headFile.open(headFilePath, std::ios::in);
	// �J�����t�@�C����ǂݍ���
	const std::istreambuf_iterator<char> it(headFile);
	const std::istreambuf_iterator<char> last;
	const std::string head(it, last);

	// �x�[�X�ɂ���v���W�F�N�g�t�@�C���i���j
	const std::string bottom = "    <None Include=\"dll.def\" />\n  </ItemGroup>\n  <Import Project = \"$(VCTargetsPath)\Microsoft.Cpp.targets\" />\n  <ImportGroup Label = \"ExtensionTargets\">\n</ImportGroup>\n</Project>";

	// �w��p�X����t�@�C�����ċA�I�Ɍ���
	namespace sys = std::tr2::sys;
	std::string addScript = "\n";
	const sys::path scriptPath("./ScriptComponent/Scripts/");
	std::for_each(sys::recursive_directory_iterator(scriptPath), sys::recursive_directory_iterator(),
		[&](const sys::path& path)
		{
			// �t�@�C���Ȃ�
			if( sys::is_regular_file(path) ) {
				if( path.extension() == ".h" )			addScript += "    <ClInclude Include=\"Scripts\\" + path.stem().string() + ".h\" />\n";
				else if( path.extension() == ".cpp" )	addScript += "    <ClCompile Include=\"Scripts\\" + path.stem().string() + ".cpp\" />\n";
			}
		}
	);

	// �o�͐�v���W�F�N�g�t�@�C��
	std::fstream destFile;
	auto destFilePath = "ScriptComponent/ScriptComponent.vcxproj";
	destFile.open(destFilePath, std::ios::out);

	// ��������
	destFile << head << addScript << bottom << std::flush;

	// �㏈��
	headFile.close();
	destFile.close();
}


bool removeDirectory(std::string fileName)
{
	bool retVal = true;
	std::string nextFileName;

	WIN32_FIND_DATA foundFile;

	HANDLE hFile = FindFirstFile((fileName + "\\*.*").c_str(), &foundFile);

	if (hFile != INVALID_HANDLE_VALUE)
	{
		do
		{
			//If a found file is . or .. then skip
			if (strcmp(foundFile.cFileName, ".") != 0 && strcmp(foundFile.cFileName, "..") != 0)
			{
				//The path should be absolute path
				nextFileName = fileName + "\\" + foundFile.cFileName;

				//If the file is a directory
				if ((foundFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
				{
					removeDirectory(nextFileName.c_str());
					RemoveDirectory(nextFileName.c_str());
				}
				//If the file is a file
				else
				{
					DeleteFile(nextFileName.c_str());
				}
			}
		} while (FindNextFile(hFile, &foundFile) != 0);
	}

	FindClose(hFile);

	//Delete starting point itseft
	if (RemoveDirectory(fileName.c_str()) == 0)retVal = false;

	return retVal;
}
#include <shlwapi.h>

#pragma comment(lib, "shlwapi.lib")

std::string outConsole;
bool create_cmd_process() {
	//	�p�C�v�̍쐬
	HANDLE readPipe;
	HANDLE writePipe;
	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof(sa);
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = NULL;
	if (CreatePipe(&readPipe, &writePipe, &sa, 0) == 0) {
		Window::AddLog("�p�C�v���쐬�ł��܂���ł���");
		return false;
	}

	Window::AddLog("�R���p�C���J�n...");

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

	if (!PathFileExists("ScriptComponent\\createdll_auto.bat")) {
		Window::AddLog("�����R���p�C�����s");
		return false;
	}

	//	�v���Z�X�̋N��(cmd.exe)
	if (CreateProcess(NULL, cmd, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi) == 0) {
		Window::AddLog("�v���Z�X�̍쐬�Ɏ��s���܂���");
		return false;
	}
	HANDLE childProcess = pi.hProcess;
	CloseHandle(pi.hThread);

	outConsole.reserve(20000);
	char readBuf[1025];
	bool end = false;
	do {
		DWORD totalLen, len;
		if (WaitForSingleObject(childProcess, 100) == WAIT_OBJECT_0)
			end = true;
		if (PeekNamedPipe(readPipe, NULL, 0, NULL, &totalLen, NULL) == 0)
			break;
		if (0 < totalLen) {
			if (ReadFile(readPipe, readBuf, sizeof(readBuf) - 1, &len, NULL) == 0) {
				Window::AddLog("�G���[");
				return false;
			}
			readBuf[len] = 0;

			//if (sizeof(mem) - 1<memSz + len){	//�����������ӂ�Ȃ��悤�ɃN���A����
			//	mem[0] = 0;
			//	memSz = 0;
			//}
			//memSz += len;
			//strcat_s(mem, sizeof(mem), readBuf);
			outConsole += readBuf;
			//SendMessage(hWnd, WM_VSCROLL, SB_BOTTOM, 0);	//	�X�N���[���o�[����ԉ��ֈړ�������
			if (totalLen>len)	//	�v���Z�X�͏I�����Ă��邪�܂��f�[�^���[���c���Ă���̂ŏI����ۗ�
				end = false;

			{
				auto out = forward_than_find_first_of(outConsole, "\n");
				if (out == "")continue;
				outConsole = behind_than_find_first_of(outConsole, "\n");
				out.pop_back();
				Window::AddLog(out);
			}
		}
	} while (end == false);

	if (CloseHandle(writePipe) == 0) {
		Window::AddLog("�p�C�v����邱�Ƃ��ł��܂���ł����B");
		return false;
	}
	if (CloseHandle(readPipe) == 0) {
		Window::AddLog("�p�C�v����邱�Ƃ��ł��܂���ł����B");
		return false;
	}
	CloseHandle(pi.hProcess);
	for (int i = 0; i < 100; i++) {
		auto out = forward_than_find_first_of(outConsole, "\n");
		outConsole = behind_than_find_first_of(outConsole, "\n");
		if (out == "")break;
		out.pop_back();
		Window::AddLog(out);
	}
	Window::AddLog("�R���p�C���I��");
	return true;
}

#endif

//����DLL�������Ă�R���|�[�l���g���Ǘ�
	UseScriptActors::UseScriptActors() {
		hModule = NULL;
		mCreate = NULL;
		mDelete = NULL;
		mGetReflect = NULL;
		mFunction0 = NULL;
		mFunction1 = NULL;


#ifdef _ENGINE_MODE
#else
		DllLoad();
#endif

	}
	UseScriptActors::~UseScriptActors() {
		UnLoad();
	}

#ifdef _ENGINE_MODE
	void UseScriptActors::ReCompile() {
		for (auto& p : mList) {
			p->SaveParam();
			p->Unload();
		}

		UnLoad();

#if _SKIP_COMPILE
#else

		CreateIncludeClassFile();


		//�Ō�̃R���p�C���o�[�W�����m�F
		const std::string LogFilePath = "Temp/buildver.txt";
		int ver = _BUILD_VERSION;
		int buildver = 0;
		{
			std::ifstream in(LogFilePath, std::ios::in);
			in >> buildver;
		}
		//�o�[�W�������Ⴄ
		if (ver != buildver) {
			std::ofstream out(LogFilePath, std::ios::out);
			out << ver;
			//�����[�X�t�H���_�폜
			removeDirectory(".\\ScriptComponent\\Release");
		}
		else {
			//����������Ă����Α�̕��C�H
			DeleteFile(".\\ScriptComponent\\Release\\Script.obj");
		}
		if (!create_cmd_process()) {
			//MessageBox(Window::GetMainHWND(), "�r���h���蓮�ōs���ĉ������B", "DLL�ǂݍ���", MB_OK);
			Window::AddLog("�R�}���h�v�����v�g����̃R���p�C���J�n");

			char cdir[255];
			GetCurrentDirectory(255, cdir);
			std::string  pass = cdir;
#ifdef _DEBUG
			pass += "/ScriptComponent/createdll_auto_d.bat";
#else
			pass += "/ScriptComponent/createdll_auto_.bat";
#endif

			SHELLEXECUTEINFO	sei = { 0 };
			//�\���̂̃T�C�Y
			sei.cbSize = sizeof(SHELLEXECUTEINFO);
			//�N�����̃E�C���h�E�n���h��
			sei.hwnd = Window::GetMainHWND();
			//�N����̕\�����
			sei.nShow = SW_SHOWNORMAL;
			//���̃p�����[�^���d�v�ŁA�Z�b�g���Ȃ���SHELLEXECUTEINFO�\���̂�hProcess�����o���Z�b�g����Ȃ��B
			sei.fMask = SEE_MASK_NOCLOSEPROCESS;
			//�N���v���O����
			sei.lpFile = pass.c_str();
			//�v���Z�X�N��
			if (!ShellExecuteEx(&sei) || (const int)sei.hInstApp <= 32) {
				Window::AddLog("�t�@�C�����J���܂���ł���");
				return;
			}


			//�I����҂�
			WaitForSingleObject(sei.hProcess, INFINITE);

			CloseHandle(sei.hProcess);

			Window::AddLog("�R���p�C���I��");
		}

#endif

		DllLoad();

		for (auto& p : mList) {
			p->Load();
			p->LoadParam();
		}

	}

	void UseScriptActors::CreateIncludeClassFile() {
		//�t�@�C�����X�V���ăR���p�C��������
		{
			std::ofstream h1("./ScriptComponent/Script.cpp", std::ios::app | std::ios::out);
			h1 << "";
			h1.close();
		}
		{

			std::ofstream h1("./ScriptComponent/Reflection/ReflectionSetter.cpp", std::ios::app | std::ios::out);
			h1 << "";
			h1.close();
		}
		//�C���N���[�h�̍\�z
		File includesFile("./ScriptComponent/System/include.h");
		includesFile.FileCreate();
		File factorysFile("./ScriptComponent/System/factory.h");
		factorysFile.FileCreate();
		File reflectionsFile("./ScriptComponent/System/reflection.h");
		reflectionsFile.FileCreate();

		namespace sys = std::tr2::sys;
		sys::path p("./ScriptComponent/Scripts/"); // �񋓂̋N�_
												   //std::for_each(sys::directory_iterator(p), sys::directory_iterator(),
												   //  �ċA�I�ɑ�������Ȃ�R�`����
		std::for_each(sys::recursive_directory_iterator(p), sys::recursive_directory_iterator(),
			[&](const sys::path& p) {
			if (sys::is_regular_file(p)) { // �t�@�C���Ȃ�...
				if (p.extension() == ".h") {
					auto out = "\n#include\"../" + p.generic_string() + "\"";
					includesFile.Out(out);

					auto outf = "\n_ADD(" + p.stem().string() + ");";
					factorysFile.Out(outf);

					findSerialize(&reflectionsFile, p.stem().string());
				}
			}
			else if (sys::is_directory(p)) { // �f�B���N�g���Ȃ�...
											 //std::cout << "dir.: " << p.string() << std::endl;
			}
		});

		IncludeScriptFileProject();
	}
	//void ReplaceEnter(std::string& targetStr) {

	//	const std::string CRLF = "\r\n";
	//	const std::string CR = "\r";
	//	const std::string LF = "\n";
	//	std::string::size_type pos = 0;
	//	while (pos = targetStr.find(CR, pos), pos != std::string::npos) {
	//		targetStr.replace(pos, CR.length(), LF);
	//	}
	//	pos = 0;
	//	while (pos = targetStr.find(CRLF, pos), pos != std::string::npos) {
	//		targetStr.replace(pos, CRLF.length(), LF);
	//	}
	//}
	//bool GetLine(std::string& targetStr, std::string& out) {

	//	const std::string LF = "\n";
	//	std::string::size_type pos = 0;
	//	pos = 0;
	//	pos = targetStr.find(LF);
	//	if (pos != std::string::npos) {
	//		out = targetStr.substr(0, pos);
	//		targetStr.replace(0, pos, "");
	//		return true;
	//	}

	//	return false;
	//}
	//void ReplaceComment(std::string& targetStr) {



	//	for (;;) {
	//		auto start = targetStr.find("//");
	//		if (start == std::string::npos) {
	//			break;
	//		}
	//		targetStr.erase(start);
	//	}
	//	for (;;) {
	//		auto start = targetStr.find("\n");
	//		if (start == std::string::npos) {
	//			break;
	//		}
	//		targetStr.erase(start);
	//	}

	//	for (;;) {
	//		auto start = targetStr.find("/*");
	//		if (start == std::string::npos) {
	//			start = 0;
	//		}
	//		auto end = targetStr.find("*/", start);
	//		if (end == std::string::npos) {
	//			targetStr.erase(start);
	//		}
	//		else {
	//			targetStr.erase(start, end);
	//		}
	//	}
	//}

	void UseScriptActors::findSerialize(File* file, const std::string& classname) {

		auto filepath = "./ScriptComponent/Scripts/" + classname + ".h";
		std::ifstream classfile(filepath, std::ios::in);

		if (classfile.fail())
		{
			return;
		}
		//�S���ǂݍ���
		std::istreambuf_iterator<char> it(classfile);
		std::istreambuf_iterator<char> last;
		std::string str(it, last);

		unsigned int findpoint = 0;
		for (;;) {

			//�V���A���C�Y����
			auto serialize = str.find("SERIALIZE", findpoint);

			if (std::string::npos == serialize) {
				break;
			}

			auto serializeend = str.find(";", serialize);
			if (std::string::npos == serializeend) {
				break;

			}
			findpoint = serializeend;

			//�V���A���C�Y�͈͂𔲂��o��
			auto sub = serializeend - serialize;
			auto serializestr = str.substr(serialize, sub);

			findMember(std::move(serializestr), file, classname);
		}
	}

	void UseScriptActors::findMember(std::string&& pickstr, File* file, const std::string& classname) {

		//�����ɕ���킵������ȕ������폜
		filter(pickstr);


		//�X�y�[�X��؂�ɂ���
		std::vector<std::string> v;
		split(v, pickstr, " ");
		auto num = (int)v.size();

		//������̌���Member�ϐ�������
		std::string member;
		for (int i = num - 1; i >= 0; i--) {
			auto l = (int)v[i].size();
			if (l == 0)continue;
			member = v[i];
			break;
		}

		if (member != "") {
			reflect(member, file, classname);
		}
	}

	void UseScriptActors::reflect(const std::string& member, File* file, const std::string& classname) {
		auto outf = "\n_REF(" + classname + "," + member + ");";
		file->Out(outf);
	}

	void UseScriptActors::filter(std::string& str) {
		//while(1){
		//	auto temp = str.find("\\\\");
		//	if (temp == std::string::npos){
		//		break;
		//	}
		//	str.replace(temp,2,"  ");
		//}
		//while (1){
		//	auto temp = str.find("\\\"");
		//	if (temp == std::string::npos){
		//		break;
		//	}
		//	str.replace(temp, 2, "  ");
		//}
		//������Ă��邩

		for (;;) {
			auto start = str.find('\"');
			if (start == std::string::npos) {
				break;
			}
			auto end = str.find('\"', start);
			if (start == std::string::npos) {
				str.erase(start);
			}
			else {
				str.erase(start, end);
			}
		}


		for (;;) {
			auto start = str.find("/*");
			if (start == std::string::npos) {
				break;
			}
			auto end = str.find("*/", start);
			if (end == std::string::npos) {
				str.erase(start);
			}
			else {
				str.erase(start, end);
			}
		}
		for (;;) {
			auto start = str.find("//");
			if (start == std::string::npos) {
				break;
			}
			auto end = str.find('\n', start);
			if (start == std::string::npos) {
				str.erase(start);
			}
			else {
				str.erase(start, end);
			}
		}

		for (;;) {
			auto temp = str.find('\n');
			if (temp == std::string::npos) {
				break;
			}
			str.replace(temp, 1, " ");
		}
		for (;;) {
			auto temp = str.find('\t');
			if (temp == std::string::npos) {
				break;
			}
			str.replace(temp, 1, " ");
		}
		for (;;) {
			auto start = str.find('\'');
			if (start == std::string::npos) {
				break;
			}
			auto end = str.find('\'', start);
			if (start == std::string::npos) {
				str.erase(start);
			}
			else {
				str.erase(start, end);
			}
		}


		//������Ă��邩
		auto equal = str.find('=');
		if (std::string::npos != equal) {
			str.erase(equal);
		}

	}

	void UseScriptActors::split(std::vector<std::string> &v, const std::string &input_string, const std::string &delimiter)
	{
		std::string::size_type index = input_string.find_first_of(delimiter);

		if (index != std::string::npos) {
			v.push_back(input_string.substr(0, index));
			split(v, input_string.substr(index + 1), delimiter);
		}
		else {
			v.push_back(input_string);
		}
	}
	void UseScriptActors::Push(ScriptComponent * com)
	{
		mList.push_back(com);
	}
	void UseScriptActors::Pop(ScriptComponent * com)
	{
		mList.remove(com);
	}
#endif

	void UseScriptActors::UnLoad() {
		FreeLibrary(hModule);
		hModule = NULL;
		mCreate = NULL;
		mDelete = NULL;
		mGetReflect = NULL;
		mFunction0 = NULL;
		mFunction1 = NULL;
		Reflection::map = NULL;
	}
	void UseScriptActors::DllLoad() {

#ifdef _ENGINE_MODE
		// DLL�̃��[�h
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


#else
		hModule = LoadLibrary("ScriptComponent/ExeCompile/ScriptComponent.dll");
#endif
		if (!hModule) {
			_SYSTEM_LOG_ERROR("�X�N���v�gDLL�̓ǂݍ���");
			return;
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

		mFunction0 = (IDllFunction0)GetProcAddress(hModule, "Function0");
		if (mFunction0 == NULL)
		{
			UnLoad();
			return;
		}
		mFunction1 = (IDllFunction0)GetProcAddress(hModule, "Function1");
		if (mFunction1 == NULL)
		{
			UnLoad();
			return;
		}
		InitIGame initIGame = (InitIGame)GetProcAddress(hModule, "InitializeIGame");
		if (initIGame == NULL)
		{
			UnLoad();
			return;
		}
		initIGame(&gSGame);
		Reflection::map = ((GetReflectionData_)mGetReflect)();
	}

	UseScriptActors * UseScriptActors::Get()
	{
		static UseScriptActors insctance;
		return &insctance;
	}

	IDllScriptComponent* UseScriptActors::Create(const std::string& ClassName) {
		if (!mCreate) {
			_SYSTEM_LOG_ERROR("�X�N���v�g[" + ClassName + "]�̍쐬");
			return NULL;
		}

		//dll�ō쐬�����N���X�C���X�^���X���쐬����
		return ((CreateInstance_)mCreate)(ClassName.c_str());
	}

	void UseScriptActors::Deleter(IDllScriptComponent* script) {


		((DeleteInstance_)mDelete)(script);
	}
	bool UseScriptActors::Function(IDllScriptComponent* com, IDllScriptComponent::Func0 func) {
		return ((IDllFunction0)mFunction0)(com, func);
	}
	bool UseScriptActors::Function(IDllScriptComponent* com, IDllScriptComponent::Func1 func, GameObject tar) {
		return ((IDllFunction1)mFunction1)(com, func, tar);
	}
	std::list<ScriptComponent*> mList;