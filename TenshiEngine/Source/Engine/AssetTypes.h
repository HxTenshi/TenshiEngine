#pragma once

#include "MySTL/ptr.h"
#include "AssetFileTypes.h"

class IAssetDataTemplate {
public:
	IAssetDataTemplate(AssetFileType type = AssetFileType::None)
		:m_AssetFileType(type) {
	}
	virtual ~IAssetDataTemplate() {}
	virtual void CreateInspector() = 0;
	virtual void FileUpdate(const char* filename) = 0;
	virtual std::string GetFileName() = 0;

	static const AssetFileType _AssetFileType = AssetFileType::None;
	const AssetFileType m_AssetFileType;
private:
	IAssetDataTemplate(const IAssetDataTemplate &) = delete;
	IAssetDataTemplate& operator=(const IAssetDataTemplate &) = delete;
};

using AssetDataTemplatePtr = shared_ptr < IAssetDataTemplate >;

template<class T>
class AssetDataTemplate : public IAssetDataTemplate {
public:

	AssetDataTemplate()
		:IAssetDataTemplate(AssetDataTemplate<T>::_AssetFileType) {
	}

	static AssetDataTemplatePtr Create(const char* filename);
	static AssetDataTemplatePtr Create(T* fileData);

	virtual ~AssetDataTemplate()override {}
	void CreateInspector()override {}
	void FileUpdate(const char* filename)override;
	std::string GetFileName()override;

	const T* GetFileData();
	T* _GetFileData();

	static const AssetFileType _AssetFileType;


protected:
	T* m_FileData;

private:
	AssetDataTemplate<T>& operator = (const AssetDataTemplate<T>&) = delete;
	AssetDataTemplate<T>(const AssetDataTemplate<T>&) = delete;

};

class MetaFileData;
using MetaAssetDataPtr = shared_ptr < AssetDataTemplate<MetaFileData> >;

class MeshFileData;
using MeshAssetDataPtr = shared_ptr < AssetDataTemplate<MeshFileData> >;

class BoneFileData;
using BoneAssetDataPtr = shared_ptr < AssetDataTemplate<BoneFileData> >;

class PrefabFileData;
using PrefabAssetDataPtr = shared_ptr < AssetDataTemplate<PrefabFileData> >;

class ShaderFileData;
using ShaderAssetDataPtr = shared_ptr < AssetDataTemplate<ShaderFileData> >;

class TextureFileData;
using TextureAssetDataPtr = shared_ptr < AssetDataTemplate<TextureFileData> >;

class PhysxMaterialFileData;
using PhysxMaterialAssetDataPtr = shared_ptr < AssetDataTemplate<PhysxMaterialFileData> >;

class SoundFileData;
using SoundAssetDataPtr = shared_ptr < AssetDataTemplate<SoundFileData> >;

class MovieFileData;
using MovieAssetDataPtr = shared_ptr < AssetDataTemplate<MovieFileData> >;
