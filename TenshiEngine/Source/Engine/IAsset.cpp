#include "IAsset.h"

#include "Engine/AssetDataBase.h"



//template<class T>
//void Load(MD5::MD5HashCoord hash){
//	Free();
//	shared_ptr<AssetDataTemplate<T>> temp;
//	AssetDataBase::Instance(hash, temp);
//	if (temp) {
//		m_Ptr = temp;
//		m_Hash = hash;
//		m_Name = temp->GetFileName();
//
//	}
//}

//#define _ASSET_T(x) \
//template <>\
//void Asset<x>::Load(MD5::MD5HashCoord hash) {\
//	Free();\
//	shared_ptr<AssetDataTemplate<x>> temp;\
//	AssetDataBase::Instance(hash, temp);\
//	if (temp) {\
//		m_Ptr = temp;\
//		m_Hash = hash;\
//		m_Name = temp->GetFileName();\
//	}\
//}\
//
//_ASSET_T(MetaFileData)
//_ASSET_T(MeshFileData)
//_ASSET_T(BoneFileData)
//_ASSET_T(PrefabFileData)
//_ASSET_T(ShaderFileData)
//_ASSET_T(TextureFileData)
//_ASSET_T(PhysxMaterialFileData)
//_ASSET_T(SoundFileData)
//_ASSET_T(MovieFileData)