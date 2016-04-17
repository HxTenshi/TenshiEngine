#pragma once

#include "MySTL/ptr.h"

enum class AssetFileType{
	None,
	Prefab,
	Temesh,
	Tebone,
	Vmd,
	Image,
	Count,
};

class AssetData{
public:

	AssetData(AssetFileType type = AssetFileType::None)
		:m_AssetFileType(type){

	}
	virtual ~AssetData(){}
	virtual void CreateInspector(){}
	virtual void FileUpdate(const char* filename) = 0;

	const AssetFileType m_AssetFileType;

	static const AssetFileType _AssetFileType = AssetFileType::None;


protected:
	void* m_Data;
	char m_GUID[33];

private:
	void operator = (const AssetData&);
	AssetData(const AssetData&);

};
using AssetDataPtr = shared_ptr < AssetData > ;



#include <functional>
#include <map>
class AssetFactory{
public:
	static AssetDataPtr Create(const char* filename);
private:
	static std::map<std::string, std::function<AssetDataPtr(const char*)>> m_Factory;
protected:
	AssetFactory();
};

#include "Window/Window.h"
class AssetDataBase{
public:
	
	template <class T>
	static void Instance(const char* filename, shared_ptr<T>& out){

		auto file = m_AssetCache.find(filename);
		AssetDataPtr data;
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

			file->second->FileUpdate(filename);
		}
		else{
			shared_ptr<AssetData> temp;
			Instance(filename,temp);
		}
	}

	static void CreateInspector(const char* filename){

		shared_ptr<AssetData> data;
		Instance(filename, data);
		if (data){
			data->CreateInspector();
		}
	}


private:

	static std::map<std::string, AssetDataPtr> m_AssetCache;
};




#include "Engine/AssetFile/Mesh/MeshFileData.h"
#include "Engine/AssetFile/Mesh/MeshBufferData.h"

class MeshAssetData : public AssetData{
public:
	virtual ~MeshAssetData();

	static AssetDataPtr MeshAssetData::Create(const char* filename);
	void FileUpdate(const char* filename)override;

	const MeshFileData& GetFileData() const;
	const MeshBufferData& GetBufferData() const;

	static const AssetFileType _AssetFileType = AssetFileType::Temesh;

private:

	MeshAssetData();


	//コピー禁止
	MeshAssetData(const MeshAssetData&) = delete;
	MeshAssetData& operator=(const MeshAssetData&) = delete;

	MeshFileData	m_MeshFileData;		// ファイルを読み込んだデータ
	MeshBufferData	m_MeshBufferData;	// 描画に使用するデータ

};

using MeshAssetDataPtr = shared_ptr < MeshAssetData >;


#include "Engine/AssetFile/Bone/BoneFileData.h"

class BoneAssetData : public AssetData{
public:
	virtual ~BoneAssetData();

	static AssetDataPtr BoneAssetData::Create(const char* filename);
	void FileUpdate(const char* filename)override;

	const BoneFileData& GetFileData() const;

	static const AssetFileType _AssetFileType = AssetFileType::Tebone;

private:

	BoneAssetData();


	//コピー禁止
	BoneAssetData(const BoneAssetData&) = delete;
	BoneAssetData& operator=(const BoneAssetData&) = delete;

	BoneFileData	m_BoneFileData;		// ファイルを読み込んだデータ

};

using BoneAssetDataPtr = shared_ptr < BoneAssetData >;


#include "AssetFile/Prefab/PrefabFileData.h"

class PrefabAssetData : public AssetData{
public:
	virtual ~PrefabAssetData();

	static AssetDataPtr PrefabAssetData::Create(const char* filename);

	void FileUpdate(const char* filename)override;

	const PrefabFileData& GetFileData() const;

	void CreateInspector() override;

	static const AssetFileType _AssetFileType = AssetFileType::Prefab;

private:

	PrefabAssetData();


	//コピー禁止
	PrefabAssetData(const PrefabAssetData&) = delete;
	PrefabAssetData& operator=(const PrefabAssetData&) = delete;

	PrefabFileData	m_PrefabFileData;		// ファイルを読み込んだデータ

};

using PrefabAssetDataPtr = shared_ptr < PrefabAssetData >;