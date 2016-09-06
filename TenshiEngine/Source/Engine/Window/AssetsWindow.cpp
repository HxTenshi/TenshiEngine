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

namespace AssetsWindow{


	void Initialize(){


		Window::SetWPFCollBack(MyWindowMessage::AssetFileDeleted, [&](void* p)
		{
			if (!p)return;
			std::string* s = (std::string*)p;
			MoveFileCheck::Set(*s);
			AssetDataBase::Remove(s->c_str());
			Window::Deleter(s);
		});
		Window::SetWPFCollBack(MyWindowMessage::AssetFileCreated, [&](void* p)
		{
			if (!p)return;
			std::string* s = (std::string*)p;
			std::tr2::sys::path path(*s);
			if (path.extension() == "" || path.extension() == ".meta"){
				Window::Deleter(s);
				return;
			}

			if (MoveFileCheck::CheckMove(s)){
				auto old = MoveFileCheck::Get() + ".meta";
				auto New = *s + ".meta";
				MoveFile(old.c_str(), New.c_str());
			}
			else{
				auto old = MoveFileCheck::Get() + ".meta";
				DeleteFile(old.c_str());
			}
			MoveFileCheck::Set("");

			AssetDataBase::InitializeMetaData(s->c_str());
			Window::Deleter(s);
		});
		Window::SetWPFCollBack(MyWindowMessage::AssetFileChanged, [&](void* p)
		{
			if (!p)return;
			std::string* s = (std::string*)p;
			AssetDataTemplatePtr temp;
			AssetDataBase::Instance(s->c_str(), temp);
			if (temp)temp->FileUpdate();
			Window::Deleter(s);
		});
		Window::SetWPFCollBack(MyWindowMessage::AssetFileRenamed, [&](void* p)
		{
			if (!p)return;
			std::string* s = (std::string*)p;
			Window::Deleter(s);
		});
}

	void Finish(){
		auto old = MoveFileCheck::Get() + ".meta";
		DeleteFile(old.c_str());
	}
}
#endif