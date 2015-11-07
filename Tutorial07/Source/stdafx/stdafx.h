
//メモリーリーク検出
#if _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

// デバッグ時のSTLのアロケーターをリリース仕様にする
//#define _NO_DEBUG_HEAP 1