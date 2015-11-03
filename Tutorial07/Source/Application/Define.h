#pragma once
#include <Windows.h>


#define WM_MYWMCOLLBACK		(WM_APP+0x0001)
//WPFウィンドウから送られるメッセージを定義
enum class MyWindowMessage{
	StackIntPtr,			//IntPtrをスタックする
	ReturnTreeViewIntPtr,	//ツリービューのIntPtrを渡す

	CreatePrefabToActor,

	SelectActor,
	ActorDestroy,
	ChangeParamComponent,
	AddComponent,
	RemoveComponent,
	Count,
};