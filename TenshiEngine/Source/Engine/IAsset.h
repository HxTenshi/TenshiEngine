#pragma once

class IAssetDataTemplate;
#include "Library/MD5.h"
#include "MySTL/ptr.h"

class IAsset{
public:
	IAsset() {
		m_Ptr = NULL;
		memset(&m_Hash, 0, sizeof(m_Hash));
		m_Name = "";
	}
	virtual ~IAsset() {}
	//virtual void Load(MD5::MD5HashCoord hash) { (void)hash; }
	void Free() {
		m_Ptr = NULL;
		memset(&m_Hash, 0, sizeof(m_Hash));
		m_Name = "";
	}
	bool IsLoad()const {
		return m_Ptr != NULL;
	}
	shared_ptr <IAssetDataTemplate> m_Ptr;
	MD5::MD5HashCode m_Hash;
	std::string m_Name;
};


template<class T>
class Asset : public IAsset {
public:
	virtual ~Asset(){}
	//void Load(MD5::MD5HashCoord hash) override;

	T* Get() const{
		return (T*)((shared_ptr<AssetDataTemplate<T>>)m_Ptr)->GetFileData();
	}

	T* Get(){
		return (T*)((shared_ptr<AssetDataTemplate<T>>)m_Ptr)->GetFileData();
	}
};

//class MetaFileData;
//class MeshFileData;
//class BoneFileData;
//class PrefabFileData;
//class ShaderFileData;
//class TextureFileData;
//class PhysxMaterialFileData;
//class SoundFileData;
//class MovieFileData;
//
//template <>
//void Asset<MetaFileData>::Load(MD5::MD5HashCoord hash);
//template <>
//void Asset<MeshFileData>::Load(MD5::MD5HashCoord hash);
//template <>
//void Asset<BoneFileData>::Load(MD5::MD5HashCoord hash);
//template <>
//void Asset<PrefabFileData>::Load(MD5::MD5HashCoord hash);
//template <>
//void Asset<ShaderFileData>::Load(MD5::MD5HashCoord hash);
//template <>
//void Asset<TextureFileData>::Load(MD5::MD5HashCoord hash);
//template <>
//void Asset<PhysxMaterialFileData>::Load(MD5::MD5HashCoord hash);
//template <>
//void Asset<SoundFileData>::Load(MD5::MD5HashCoord hash);
//template <>
//void Asset<MovieFileData>::Load(MD5::MD5HashCoord hash);