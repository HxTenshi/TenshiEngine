#include "AssetsWindow.h"

#ifdef _ENGINE_MODE

#include "Application/Define.h"
#include "Window/Window.h"
#include "Engine/AssetDataBase.h"

#include <filesystem>

namespace MoveFileCheck{
	std::tr2::sys::path stack = "";
	void Set(const std::string& s){
		stack = s;
	}
	bool CheckMove(std::string* s){

		std::tr2::sys::path temp = *s;
		if (temp.filename() == stack.filename()){
			return true;
		}
		return false;
	}
	std::string Get(){
		return stack.string();
	}
}

#include "Game/Component/ScriptComponent.h"

namespace AssetsWindow{

	//  文字列を置換する
	void Replace(std::string& Source, std::string String2, std::string String3)
	{
		std::string::size_type  Pos(Source.find(String2));

		while (Pos != std::string::npos)
		{
			Source.replace(Pos, String2.length(), String3);
			Pos = Source.find(String2, Pos + String3.length());
		}
	}

	void Initialize(){

		static 	std::string m_RenameStack;
		static AssetDataTemplatePtr m_Back;
		Window::SetWPFCollBack(MyWindowMessage::AssetFileDeleted, [&](void* p)
		{
			if (!p)return;
			std::string* s = (std::string*)p;
			Replace(*s, "\\", "/");

			Window::AddLog("DeleteFile:" + *s);

			//テンプファイル上書き保存処理？で誤認される
			//std::tr2::sys::path path(*s);
			//if (path.extension() == ".cpp" || path.extension() == ".h") {
			//	IncludeScriptFileProject();
			//}

			MoveFileCheck::Set(*s);
			m_Back = AssetDataBase::GetAssetFile(*s);
			AssetDataBase::Remove(s->c_str());
			Window::Deleter(s);
		});
		Window::SetWPFCollBack(MyWindowMessage::AssetFileCreated, [&](void* p)
		{
			if (!p)return;
			std::string* s = (std::string*)p;
			Replace(*s, "\\", "/");

			Window::AddLog("CreateFile:"+*s);

			std::tr2::sys::path path(*s);
			if (path.extension() == ".meta"){
				Window::Deleter(s);
				return;
			}
			if (path.extension() == ".TMP") {
				Window::Deleter(s);
				return;
			}
			if (path.extension().string().find('~') != std::string::npos) {
				Window::Deleter(s);
				return;
			}

			if (path.extension() == ".cpp" || path.extension() == ".h") {
				GenerateScriptProjectFile();
				Window::Deleter(s);
				return;
			}


			if (MoveFileCheck::CheckMove(s)){
				auto old = MoveFileCheck::Get() + ".meta";
				auto New = *s + ".meta";
				MoveFile(old.c_str(), New.c_str());

				AssetDataBase::MoveAssetFile(*s, m_Back);
			}
			else{
				auto old = MoveFileCheck::Get() + ".meta";
				DeleteFile(old.c_str());
				AssetDataBase::InitializeMetaData(s->c_str());
			}
			MoveFileCheck::Set("");
			Window::Deleter(s);
		});
		Window::SetWPFCollBack(MyWindowMessage::AssetFileChanged, [&](void* p)
		{
			if (!p)return;
			std::string* s = (std::string*)p;
			Replace(*s, "\\", "/");

			Window::AddLog("ChangeFile:" + *s);
			//AssetDataTemplatePtr temp;
			//temp = AssetDataBase::GetAssetFile(*s);
			AssetDataBase::FileUpdate(s->c_str());
			//if (temp)temp->FileUpdate(*s);
			Window::Deleter(s);
		});
		Window::SetWPFCollBack(MyWindowMessage::AssetFileRenamed_OldName, [&](void* p)
		{
			if (!p)return;
			std::string* s = (std::string*)p;
			Replace(*s, "\\", "/");

			Window::AddLog("ReneameFile:" + *s);
			m_RenameStack = *s;
			Window::Deleter(s);
		});
		Window::SetWPFCollBack(MyWindowMessage::AssetFileRenamed_NewName, [&](void* p)
		{
			if (!p)return;
			std::string* s = (std::string*)p;
			Replace(*s, "\\", "/");

			Window::AddLog("RenameToFile:" + *s);

			std::tr2::sys::path path(m_RenameStack);
			//ビジュアルスタジオでファイルを上書き保存？した場合
			//一時ファイルからのリネームになる
			//一時ファイルに保存 -> 元ファイル削除 -> 元ファイルにリネームの処理が走る

			//一時ファイルからのリネーム
			if (path.extension().string().find('~') != std::string::npos) {
				AssetDataBase::MoveAssetFile(*s, m_Back);
				AssetDataBase::FileUpdate(s->c_str());
				MoveFileCheck::Set("");
			}
			//普通のリネーム
			else {
				AssetDataBase::AssetFileRename(m_RenameStack, *s);

				auto old = m_RenameStack + ".meta";
				auto New = *s + ".meta";
				MoveFile(old.c_str(), New.c_str());
			}

			Window::Deleter(s);
			m_RenameStack = "";
		});
}

	void Finish(){
		auto old = MoveFileCheck::Get() + ".meta";
		DeleteFile(old.c_str());
	}
}
#endif