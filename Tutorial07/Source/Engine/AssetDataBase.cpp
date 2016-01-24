#include "AssetDataBase.h"

#include "Device/DirectX11Device.h"


#include "MySTL/Utility.h"

decltype(AssetFactory::m_Factory) AssetFactory::m_Factory;
class __AssetFactory :public AssetFactory{
public: __AssetFactory(){}
};

static __AssetFactory factory;

AssetFactory::AssetFactory(){
	m_Factory.insert(std::make_pair<std::string, std::function<AssetDataPtr(const char*)>>(std::string("tesmesh"), [](const char* filename){ return MeshAssetData::Create(filename); }));
	m_Factory.insert(std::make_pair<std::string, std::function<AssetDataPtr(const char*)>>(std::string("tedmesh"), [](const char* filename){ return MeshAssetData::Create(filename); }));
	m_Factory.insert(std::make_pair<std::string,std::function<AssetDataPtr(const char*)>>(std::string("tebone"), [](const char* filename){ return BoneAssetData::Create(filename); }));
}
//static
AssetDataPtr AssetFactory::Create(const char* filename){

	std::string str = filename;
	
	auto type = behind_than_find_last_of(str, ".");
	if (type == "") return NULL;

	auto make = m_Factory.find(type);
	if (make == m_Factory.end())return NULL;

	return make->second(filename);

}

//static
decltype(AssetDataBase::m_AssetCache) AssetDataBase::m_AssetCache;

////static
//template <class T>
//void AssetDataBase::Instance(const char* filename, shared_ptr<T> out){
//
//	auto file = m_AssetCache.find(filename);
//	AssetDataPtr data;
//	if (file == m_AssetCache.end()){
//
//		data = AssetFactory::Create(filename);
//		m_AssetCache.insert(std::make_pair(filename, data));
//	}
//	else{
//		data = file->second;
//	}
//	//}
//	if (T::_AssetFileType == data->m_AssetFileType){
//		out = data;
//	}
//}




MeshAssetData::~MeshAssetData()
{
}

MeshAssetData::MeshAssetData()
	:AssetData(_AssetFileType)
{
}

//static
AssetDataPtr MeshAssetData::Create(const char* filename)
{
	
	//auto temp = make_shared<MeshAssetData>();

	struct PrivateFactory : public  MeshAssetData{
		PrivateFactory() : MeshAssetData(){}
	};
	auto temp = make_shared<PrivateFactory>();

	temp->m_MeshFileData.Create(filename);
	if (!temp->m_MeshFileData.GetPolygonsData())return NULL;
	temp->m_MeshBufferData.Create(&temp->m_MeshFileData);
	if (!temp->m_MeshBufferData.GetVertexBuffer())return NULL;

	return temp;
}

const MeshFileData& MeshAssetData::GetFileData() const{
	return m_MeshFileData;
}
const MeshBufferData& MeshAssetData::GetBufferData() const{
	return m_MeshBufferData;
}










BoneAssetData::~BoneAssetData()
{
}

BoneAssetData::BoneAssetData()
	:AssetData(_AssetFileType)
{
}

//static
AssetDataPtr BoneAssetData::Create(const char* filename)
{
	//auto temp = make_shared<BoneAssetData>();

	struct PrivateFactory : public  BoneAssetData{
		PrivateFactory() : BoneAssetData(){}
	};
	auto temp = make_shared<PrivateFactory>();

	temp->m_BoneFileData.Create(filename);

	return temp;
}

const BoneFileData& BoneAssetData::GetFileData() const{
	return m_BoneFileData;
}
