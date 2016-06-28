#pragma once


#include "MySTL/ptr.h"
#include <functional>
#include <unordered_map>
#include <string>

enum class AssetFileType{
	None,
	Prefab,
	Temesh,
	Tebone,
	Shader,
	Vmd,
	Image,
	PhysxMaterial,
	Sound,
	Movie,
	Count,
};

class IAssetDataTemplate{
public:
	IAssetDataTemplate(AssetFileType type = AssetFileType::None)
		:m_AssetFileType(type){
	}
	virtual ~IAssetDataTemplate(){}
	virtual void CreateInspector() = 0;
	virtual void FileUpdate() = 0;

	static const AssetFileType _AssetFileType = AssetFileType::None;
	const AssetFileType m_AssetFileType;
};

using AssetDataTemplatePtr = shared_ptr < IAssetDataTemplate >;


class AssetFactory{
public:
	static AssetDataTemplatePtr Create(const char* filename);
private:
	static std::unordered_map<std::string, std::function<AssetDataTemplatePtr(const char*)>> m_Factory;
protected:
	AssetFactory();
	~AssetFactory();
};

class AssetDataBase{
public:
	
	template <class T>
	static void Instance(const char* filename, shared_ptr<T>& out){

		auto file = m_AssetCache.find(filename);


		AssetDataTemplatePtr data;

		if (file == m_AssetCache.end()){

			data = AssetFactory::Create(filename);
			if (data){
				m_AssetCache.insert(std::make_pair(filename, data));
			}
		}
		else{
			data = file->second;
		}
		
		if (data && 
			(T::_AssetFileType == data->m_AssetFileType) ||
			T::_AssetFileType == AssetFileType::None){
			out = data;
		}
	}

	static void FileUpdate(const char* filename){

		auto file = m_AssetCache.find(filename);
		if (file != m_AssetCache.end()){

			file->second->FileUpdate();
		}
		else{
			AssetDataTemplatePtr temp;
			Instance(filename,temp);
		}
	}

	static void CreateInspector(const char* filename){
		AssetDataTemplatePtr temp;

		Instance(filename, temp);
		if (temp){
			temp->CreateInspector();
		}
	}


private:
	static std::unordered_map<std::string, AssetDataTemplatePtr> m_AssetCache;
};


template<class T>
class AssetDataTemplate : public IAssetDataTemplate {
public:

	AssetDataTemplate()
		:IAssetDataTemplate(_AssetFileType){
	}

	static AssetDataTemplatePtr Create(const char* filename);
	static AssetDataTemplatePtr Create(T* fileData);

	virtual ~AssetDataTemplate(){}
	void CreateInspector(){}
	void FileUpdate();

	const T* GetFileData();

	static const AssetFileType _AssetFileType;


protected:
	T* m_FileData;

private:
	AssetDataTemplate<T>& operator = (const AssetDataTemplate<T>&) = delete;
	AssetDataTemplate<T>(const AssetDataTemplate<T>&) = delete;

};

class MeshFileData;
using MeshAssetDataPtr = shared_ptr < AssetDataTemplate<MeshFileData> >;
const AssetFileType AssetDataTemplate<MeshFileData>::_AssetFileType = AssetFileType::Temesh;
void AssetDataTemplate<MeshFileData>::CreateInspector();

class BoneFileData;
using BoneAssetDataPtr = shared_ptr < AssetDataTemplate<BoneFileData> >;
const AssetFileType AssetDataTemplate<BoneFileData>::_AssetFileType = AssetFileType::Tebone;
void AssetDataTemplate<BoneFileData>::CreateInspector();

class PrefabFileData;
using PrefabAssetDataPtr = shared_ptr < AssetDataTemplate<PrefabFileData> >;
const AssetFileType AssetDataTemplate<PrefabFileData>::_AssetFileType = AssetFileType::Prefab;
void AssetDataTemplate<PrefabFileData>::CreateInspector();

class ShaderFileData;
using ShaderAssetDataPtr = shared_ptr < AssetDataTemplate<ShaderFileData> >;
const AssetFileType AssetDataTemplate<ShaderFileData>::_AssetFileType = AssetFileType::Shader;
void AssetDataTemplate<ShaderFileData>::CreateInspector();

class TextureFileData;
using TextureAssetDataPtr = shared_ptr < AssetDataTemplate<TextureFileData> >;
const AssetFileType AssetDataTemplate<TextureFileData>::_AssetFileType = AssetFileType::Image;
void AssetDataTemplate<TextureFileData>::CreateInspector(){}

class PhysxMaterialFileData;
using PhysxMaterialAssetDataPtr = shared_ptr < AssetDataTemplate<PhysxMaterialFileData> >;
const AssetFileType AssetDataTemplate<PhysxMaterialFileData>::_AssetFileType = AssetFileType::PhysxMaterial;
void AssetDataTemplate<PhysxMaterialFileData>::CreateInspector();

class SoundFileData;
using SoundAssetDataPtr = shared_ptr < AssetDataTemplate<SoundFileData> >;
const AssetFileType AssetDataTemplate<SoundFileData>::_AssetFileType = AssetFileType::Sound;
void AssetDataTemplate<SoundFileData>::CreateInspector(){}


class MovieFileData;
using MovieAssetDataPtr = shared_ptr < AssetDataTemplate<MovieFileData> >;
const AssetFileType AssetDataTemplate<MovieFileData>::_AssetFileType = AssetFileType::Movie;
void AssetDataTemplate<MovieFileData>::CreateInspector(){}


template <class T>
const AssetFileType AssetDataTemplate<T>::_AssetFileType = AssetFileType::None;

#include "details.h"