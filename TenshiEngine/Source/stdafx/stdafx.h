
//メモリーリーク検出
#if _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif


#ifndef NULL
#define NULL 0
#endif

#include <d3d11.h>
#define _XM_NO_INTRINSICS_
#include "XNAMath/XNAMath.h"

// デバッグ時のSTLのアロケーターをリリース仕様にする
//#define _NO_DEBUG_HEAP 1