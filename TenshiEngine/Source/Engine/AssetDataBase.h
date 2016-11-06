#pragma once


#include "MySTL/ptr.h"
#include <functional>
#include <unordered_map>
#include <map>
#include <string>
#include <cctype>
#include <algorithm>

#include "AssetFileTypes.h"
#include "AssetTypes.h"

class AssetFactory{
public:
	static AssetDataTemplatePtr Create(const char* filename);
private:
	static std::unordered_map<std::string, std::function<AssetDataTemplatePtr(const char*)>> m_Factory;
protected:
	AssetFactory();
	~AssetFactory();

	template<class T>
	void factory(std::string ex) {
		m_Factory.insert(std::make_pair<std::string, std::function<AssetDataTemplatePtr(const char*)>>(std::move(ex), [](const char* filename) { return AssetDataTemplate<T>::Create(filename); }));
	}
};
#include "Library/MD5.h"
class AssetDataBase{
public:
	
	template <class T>
	static void Instance(const char* filename, shared_ptr<T>& out){
		std::string x(filename);
		if (x.empty())return;
		std::transform(x.begin(), x.end(), x.begin(), [](int x) { return (char)std::tolower(x); });
		auto _filename = x.c_str();
		auto file = m_AssetCache.find(_filename);



		AssetDataTemplatePtr data;

		//メタファイルが存在しない
		if (file == m_AssetCache.end()){
			InitializeMetaData(_filename);
			data = AssetFactory::Create(_filename);
			if (data){
				MD5::MD5HashCode hash;
				memset(hash.key_c, NULL, sizeof(MD5::MD5HashCode));
				m_AssetCache.insert(std::make_pair(_filename, std::make_pair(hash, data)));
			}
		}
		//未ロード
		else if (file->second.second == NULL){
			data = AssetFactory::Create(_filename);
			if (data){
				file->second.second = data;
				auto temp = m_AssetMetaCache.find(file->second.first);
				if (temp != m_AssetMetaCache.end()){
					temp->second.second = data;
				}
			}
		}
		//ロード済み
		else{
			data = file->second.second;
		}
		
		if (data && 
			(T::_AssetFileType == data->m_AssetFileType) ||
			T::_AssetFileType == AssetFileType::None){
			out = data;
		}
	}

	template <class T>
	static void Instance(const MD5::MD5HashCode& hash, shared_ptr<T>& out){
		auto file = m_AssetMetaCache.find(hash);

		AssetDataTemplatePtr data;

		if (file == m_AssetMetaCache.end()){

		}
		else{
			if (file->second.second){
				data = file->second.second;
			}
			else{
				//未ロード
				data = AssetFactory::Create(file->second.first.c_str());
				file->second.second = data;
				auto temp = m_AssetCache.find(file->second.first);
				if (temp != m_AssetCache.end()){
					temp->second.second = data;
				}
			}
		}

		if (data &&
			(T::_AssetFileType == data->m_AssetFileType) ||
			T::_AssetFileType == AssetFileType::None){
			out = data;
		}
	}


	static bool FilePath2Hash(const char* filename, MD5::MD5HashCode& hash){

		std::string x(filename);
		std::transform(x.begin(), x.end(), x.begin(), [](int x) { return (char)std::tolower(x); });
		auto _filename = x.c_str();

		auto file = m_AssetCache.find(_filename);
		if (file == m_AssetCache.end())return false;

		hash = file->second.first;
		return true;
	}
	static bool Hash2FilePath(MD5::MD5HashCode& hash, std::string& filename){

		auto file = m_AssetMetaCache.find(hash);
		if (file == m_AssetMetaCache.end())return false;

		filename = file->second.first;
		return true;
	}

	static void InitializeMetaData(const char* filename);

	static void Remove(const char* filename){

		std::string x(filename);
		std::transform(x.begin(), x.end(), x.begin(), [](int x) { return (char)std::tolower(x); });
		auto _filename = x.c_str();

		auto file = m_AssetCache.find(_filename);
		if (file == m_AssetCache.end())return;

		auto hashfile = m_AssetMetaCache.find(file->second.first);
		if (hashfile != m_AssetMetaCache.end()){
			m_DeleteCache = hashfile->second;
			m_AssetMetaCache.erase(hashfile);
		}
		
		m_AssetCache.erase(file);
		

	}

	static void FileUpdate(const char* filename){
		std::string x(filename);
		std::transform(x.begin(), x.end(), x.begin(), [](int x) { return (char)std::tolower(x); });
		auto _filename = x.c_str();

		auto file = m_AssetCache.find(_filename);
		if (file != m_AssetCache.end()){
			if (file->second.second) {
				file->second.second->FileUpdate(filename);
			}
		}
		else{
			//AssetDataTemplatePtr temp;
			//Instance(filename,temp);
		}
	}

	static void CreateInspector(const char* filename){
		AssetDataTemplatePtr temp;

		Instance(filename, temp);
		if (temp){
			temp->CreateInspector();
		}
	}
	static AssetDataTemplatePtr GetAssetFile(std::string Filename){
		std::transform(Filename.begin(), Filename.end(), Filename.begin(), [](int x) { return (char)std::tolower(x); });
		auto _filename = Filename.c_str();

		auto cache = m_AssetCache.find(_filename);
		if (cache == m_AssetCache.end())return NULL;
		return cache->second.second;
	}
	static void MoveAssetFile(std::string NewFilename, AssetDataTemplatePtr file){
		std::transform(NewFilename.begin(), NewFilename.end(), NewFilename.begin(), [](int x) { return (char)std::tolower(x); });
		auto _filename = NewFilename.c_str();

		InitializeMetaData(_filename);
		auto cache = m_AssetCache.find(_filename);

		if (cache == m_AssetCache.end())return;
		cache->second.second = file;
		auto temp = m_AssetMetaCache.find(cache->second.first);
		if (temp != m_AssetMetaCache.end()){
			temp->second.second = file;
		}
	}
	static void AssetFileRename(std::string OldName, std::string NewName){

		std::transform(OldName.begin(), OldName.end(), OldName.begin(), [](int x) { return (char)std::tolower(x); });
		std::transform(NewName.begin(), NewName.end(), NewName.begin(), [](int x) { return (char)std::tolower(x); });

		if (OldName == NewName)return;

		auto _oldfilename = OldName.c_str();
		auto cache = m_AssetCache.find(_oldfilename);
		if (cache != m_AssetCache.end()){
			auto meta = m_AssetMetaCache.find(cache->second.first);
			if (meta != m_AssetMetaCache.end()){
				meta->second.first = NewName;
			}

			m_AssetCache.insert(std::make_pair(NewName, cache->second));
			m_AssetCache.erase(cache);
		}

	}

private:
	static std::unordered_map<std::string, std::pair<MD5::MD5HashCode,AssetDataTemplatePtr>> m_AssetCache;
	static std::map<MD5::MD5HashCode, std::pair<std::string, AssetDataTemplatePtr>> m_AssetMetaCache;
	static std::pair<std::string, AssetDataTemplatePtr> m_DeleteCache;
};

//template<class T>
//class AssetDataTemplate : public IAssetDataTemplate {
//public:
//
//	AssetDataTemplate()
//		:IAssetDataTemplate(_AssetFileType){
//	}
//
//	static AssetDataTemplatePtr Create(const char* filename);
//	static AssetDataTemplatePtr Create(T* fileData);
//
//	virtual ~AssetDataTemplate()override{}
//	void CreateInspector()override{}
//	void FileUpdate(const char* filename)override;
//	std::string GetFileName()override;
//
//	const T* GetFileData();
//	T* _GetFileData();
//
//	static const AssetFileType _AssetFileType;
//
//
//protected:
//	T* m_FileData;
//
//private:
//	AssetDataTemplate<T>& operator = (const AssetDataTemplate<T>&) = delete;
//	AssetDataTemplate<T>(const AssetDataTemplate<T>&) = delete;
//
//};
//
//
//class MetaFileData;
//using MetaAssetDataPtr = shared_ptr < AssetDataTemplate<MetaFileData> >;
//const AssetFileType AssetDataTemplate<MetaFileData>::_AssetFileType = AssetFileType::Meta;
//void AssetDataTemplate<MetaFileData>::CreateInspector(){}
//
//class MeshFileData;
//using MeshAssetDataPtr = shared_ptr < AssetDataTemplate<MeshFileData> >;
//const AssetFileType AssetDataTemplate<MeshFileData>::_AssetFileType = AssetFileType::Temesh;
//void AssetDataTemplate<MeshFileData>::CreateInspector();
//
//class BoneFileData;
//using BoneAssetDataPtr = shared_ptr < AssetDataTemplate<BoneFileData> >;
//const AssetFileType AssetDataTemplate<BoneFileData>::_AssetFileType = AssetFileType::Tebone;
//void AssetDataTemplate<BoneFileData>::CreateInspector();
//
//class PrefabFileData;
//using PrefabAssetDataPtr = shared_ptr < AssetDataTemplate<PrefabFileData> >;
//const AssetFileType AssetDataTemplate<PrefabFileData>::_AssetFileType = AssetFileType::Prefab;
//void AssetDataTemplate<PrefabFileData>::CreateInspector();
//
//class ShaderFileData;
//using ShaderAssetDataPtr = shared_ptr < AssetDataTemplate<ShaderFileData> >;
//const AssetFileType AssetDataTemplate<ShaderFileData>::_AssetFileType = AssetFileType::Shader;
//void AssetDataTemplate<ShaderFileData>::CreateInspector();
//
//class TextureFileData;
//using TextureAssetDataPtr = shared_ptr < AssetDataTemplate<TextureFileData> >;
//const AssetFileType AssetDataTemplate<TextureFileData>::_AssetFileType = AssetFileType::Image;
//void AssetDataTemplate<TextureFileData>::CreateInspector(){}
//
//class PhysxMaterialFileData;
//using PhysxMaterialAssetDataPtr = shared_ptr < AssetDataTemplate<PhysxMaterialFileData> >;
//const AssetFileType AssetDataTemplate<PhysxMaterialFileData>::_AssetFileType = AssetFileType::PhysxMaterial;
//void AssetDataTemplate<PhysxMaterialFileData>::CreateInspector();
//
//class SoundFileData;
//using SoundAssetDataPtr = shared_ptr < AssetDataTemplate<SoundFileData> >;
//const AssetFileType AssetDataTemplate<SoundFileData>::_AssetFileType = AssetFileType::Sound;
//void AssetDataTemplate<SoundFileData>::CreateInspector(){}
//
//
//class MovieFileData;
//using MovieAssetDataPtr = shared_ptr < AssetDataTemplate<MovieFileData> >;
//const AssetFileType AssetDataTemplate<MovieFileData>::_AssetFileType = AssetFileType::Movie;
//void AssetDataTemplate<MovieFileData>::CreateInspector(){}
//
//
//template <class T>
//const AssetFileType AssetDataTemplate<T>::_AssetFileType = AssetFileType::None;

#include "details.h"


#include "Assets.h"
