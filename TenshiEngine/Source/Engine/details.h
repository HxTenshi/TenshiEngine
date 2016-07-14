#pragma once


#include "AssetFile\AssetFileData.h"

template <class T>
AssetDataTemplatePtr AssetDataTemplate<T>::Create(const char* filename){

	struct PrivateFactory : public  AssetDataTemplate<T>{
		PrivateFactory() : AssetDataTemplate()
		{
			m_FileData = new T();
		}
		virtual ~PrivateFactory(){
			delete m_FileData;
		}

		bool Create(const char* filename){
			return ((AssetFileData*)m_FileData)->Create(filename);
		}
	};
	auto temp = make_shared<PrivateFactory>();

	if (!temp->Create(filename)){
		return NULL;
	}

	return temp;
}

template <class T>
AssetDataTemplatePtr AssetDataTemplate<T>::Create(T* fileData){

	struct PrivateFactory : public  AssetDataTemplate<T>{
		PrivateFactory(T* fileData) : AssetDataTemplate()
		{
			m_FileData = fileData;
		}
		virtual ~PrivateFactory(){
			delete m_FileData;
		}
	};
	auto temp = make_shared<PrivateFactory>(fileData);
	return temp;
}


template <class T>
void AssetDataTemplate<T>::FileUpdate(){
	((AssetFileData*)m_FileData)->FileUpdate();
}

template <class T>
const T* AssetDataTemplate<T>::GetFileData(){
	return m_FileData;
}