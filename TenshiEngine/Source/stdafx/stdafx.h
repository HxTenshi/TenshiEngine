
//メモリーリーク検出
#if _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#define   new                   new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define   malloc(s)             _malloc_dbg(s, _NORMAL_BLOCK, __FILE__, __LINE__)
#define   calloc(c, s)          _calloc_dbg(c, s, _NORMAL_BLOCK, __FILE__, __LINE__)
#define   realloc(p, s)         _realloc_dbg(p, s, _NORMAL_BLOCK, __FILE__, __LINE__)
#define   _recalloc(p, c, s)    _recalloc_dbg(p, c, s, _NORMAL_BLOCK, __FILE__, __LINE__)
#define   _expand(p, s)         _expand_dbg(p, s, _NORMAL_BLOCK, __FILE__, __LINE__)
#endif


#ifndef NULL
#define NULL 0
#endif

#include <d3d11.h>
#define _XM_NO_INTRINSICS_
#include "XNAMath/XNAMath.h"


#include "Engine/SystemLog.h"

// デバッグ時のSTLのアロケーターをリリース仕様にする
//#define _NO_DEBUG_HEAP 1