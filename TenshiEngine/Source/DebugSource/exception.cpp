#include"exception.h"

#include <bitset>
#include <sstream>

#pragma comment(lib,"dbghelp.lib")
#include <DbgHelp.h>

#include "SystemLog.h"

int Debug::filter(unsigned int code, struct _EXCEPTION_POINTERS *ep) {


	//HMODULE hm;
	//::GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, static_cast<LPCTSTR>(ep->ExceptionRecord->ExceptionAddress), &hm);
	//MODULEINFO mi;
	//::GetModuleInformation(::GetCurrentProcess(), hm, &mi, sizeof(mi));
	//char fn[MAX_PATH];
	//::GetModuleFileNameExA(::GetCurrentProcess(), hm, fn, MAX_PATH);
	//
	//std::ostringstream oss;
	//oss << "SE " << (seDescription(code)) << " at address 0x" << std::hex << ep->ExceptionRecord->ExceptionAddress << std::dec
	//	<< " inside " << fn << " loaded at base address 0x" << std::hex << mi.lpBaseOfDll << "\n";
	//
	//
	//Hx::Debug()->Log(" Exceptエラー Address[" + oss.str() + "]");

	std::ostringstream oss;

	{
		STACKFRAME sf;
		BOOL bResult;
		PIMAGEHLP_SYMBOL pSym;
		DWORD Disp;

		printf("例外発生。\n");

		/* シンボル情報格納用バッファの初期化 */
		pSym = (PIMAGEHLP_SYMBOL)GlobalAlloc(GMEM_FIXED, 10000);
		pSym->SizeOfStruct = 10000;
		pSym->MaxNameLength = 10000 - sizeof(IMAGEHLP_SYMBOL);


		IMAGEHLP_LINE line;
		line.SizeOfStruct = sizeof(IMAGEHLP_LINE);

		/* スタックフレームの初期化 */
		ZeroMemory(&sf, sizeof(sf));
		sf.AddrPC.Offset = ep->ContextRecord->Eip;
		sf.AddrStack.Offset = ep->ContextRecord->Esp;
		sf.AddrFrame.Offset = ep->ContextRecord->Ebp;
		sf.AddrPC.Mode = AddrModeFlat;
		sf.AddrStack.Mode = AddrModeFlat;
		sf.AddrFrame.Mode = AddrModeFlat;

		/* シンボルハンドラの初期化 */
		SymInitialize(GetCurrentProcess(), NULL, TRUE);

		/* スタックフレームを順に表示していく */
		for (;;) {
			/* 次のスタックフレームの取得 */
			bResult = StackWalk(
				IMAGE_FILE_MACHINE_I386,
				GetCurrentProcess(),
				GetCurrentThread(),
				&sf,
				NULL,
				NULL,
				SymFunctionTableAccess,
				SymGetModuleBase,
				NULL);

			/* 失敗ならば、ループを抜ける */
			if (!bResult || sf.AddrFrame.Offset == 0) break;

			/* プログラムカウンタから関数名を取得 */
			bResult = SymGetSymFromAddr(GetCurrentProcess(), sf.AddrPC.Offset, &Disp, pSym);
			bResult = bResult && SymGetLineFromAddr(GetCurrentProcess(), sf.AddrPC.Offset, &Disp, &line);

			/* 取得結果を表示 */
			if (bResult) {
				std::string c = pSym->Name;
				{
					auto i = c.find(":");
					if (i != std::string::npos) {
						c = c.erase(i);
						c += ".cpp";
					}
				}

				oss << c << ":line(" << line.LineNumber << "):" << pSym->Name << "()";
				_SYSTEM_LOG(oss.str());
				oss.clear();
				//printf("0x%08x %s() + 0x%x\n", sf.AddrPC.Offset, pSym->Name, Disp);
				//Hx::Debug()->Log(oss.str());
			}
			else {
				//std::ostringstream oss;
				//oss << " address 0x" << std::hex << sf.AddrPC.Offset << std::dec << " ---";
				//printf("%08x, ---", sf.AddrPC.Offset);
				//Hx::Debug()->Log(oss.str());
			}
		}

		/* 後処理 */
		SymCleanup(GetCurrentProcess());
		GlobalFree(pSym);

	}


	//auto address = (unsigned long)ep->ExceptionRecord->ExceptionAddress;
	//auto ecode = ep->ExceptionRecord->ExceptionCode;
	//
	//{
	//	std::ostringstream os;
	//	os << std::hex << address;
	//	Hx::Debug()->Log(" Exceptエラー Address[" + os.str() + "]");
	//}
	{
		//std::ostringstream os;
		//os << std::hex << code;
		//Hx::Debug()->Log(" +-- Code[" + os.str() + "]");
	}
	if (code == EXCEPTION_FLT_DIVIDE_BY_ZERO) {
		oss << "[ゼロ除算]";
	}
	else if (code == EXCEPTION_INT_DIVIDE_BY_ZERO) {
		oss << "[ゼロ除算]";
	}
	else if (code == EXCEPTION_ACCESS_VIOLATION) {
		oss << "[アクセス違反]";
	}

	return EXCEPTION_EXECUTE_HANDLER;
}