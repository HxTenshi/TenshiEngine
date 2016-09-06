#pragma once
#include <Windows.h>

#define WM_MYWMCOLLBACK		(WM_APP+0x0001)
//WPFウィンドウから送られるメッセージを定義
enum class MyWindowMessage{
	StackIntPtr,			//IntPtrをスタックする
	ReturnTreeViewIntPtr,	//ツリービューのIntPtrを渡す
	ScriptCompile,
	SaveScene,

	SelectAsset,
	OpenAsset,

	CreateScriptFile,
	CreateAssetFile,

	//ファイル操作系
	AssetFileChanged,
	AssetFileCreated,
	AssetFileDeleted,
	AssetFileRenamed,

	PlayGame,
	StopGame,
	CreateModelConvert,
	CreatePrefabToActor,
	CreateActorToPrefab,

	SelectActor,
	SetActorParent,
	ActorDoubleClick,
	ActorDestroy,
	ChangeParamComponent,
	AddComponent,
	RemoveComponent,
	Count,
};