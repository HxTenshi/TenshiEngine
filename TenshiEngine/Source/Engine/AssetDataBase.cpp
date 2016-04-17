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
	m_Factory.insert(std::make_pair<std::string, std::function<AssetDataPtr(const char*)>>(std::string("tebone"), [](const char* filename){ return BoneAssetData::Create(filename); }));
	m_Factory.insert(std::make_pair<std::string, std::function<AssetDataPtr(const char*)>>(std::string("json"), [](const char* filename){ return PrefabAssetData::Create(filename); }));
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
void MeshAssetData::FileUpdate(const char* filename)
{
	m_MeshFileData.Create(filename);
	m_MeshBufferData.Create(&m_MeshFileData);
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

void BoneAssetData::FileUpdate(const char* filename)
{
	m_BoneFileData.Create(filename);
}


const BoneFileData& BoneAssetData::GetFileData() const{
	return m_BoneFileData;
}



PrefabAssetData::PrefabAssetData()
	:AssetData(_AssetFileType)
{
}
PrefabAssetData::~PrefabAssetData()
{
}


//static
AssetDataPtr PrefabAssetData::Create(const char* filename)
{

	struct PrivateFactory : public  PrefabAssetData{
		PrivateFactory() : PrefabAssetData(){}
	};
	auto temp = make_shared<PrivateFactory>();

	temp->m_PrefabFileData.Create(filename);

	return temp;
}
void PrefabAssetData::FileUpdate(const char* filename)
{
	m_PrefabFileData.FileUpdate(filename);
}

const PrefabFileData& PrefabAssetData::GetFileData() const{
	return m_PrefabFileData;
}

#include "Game/Game.h"
#include "Game/Actor.h"
void PrefabAssetData::CreateInspector(){
	m_PrefabFileData.GetActor()->CreateInspector();

	auto data = Window::CreateInspector();
	std::function<void()> collback = [&](){
		auto before = m_PrefabFileData.Apply();

		Game::GetAllObject([&](Actor* tar){
			auto str = tar->Prefab();
			if (m_PrefabFileData.GetFileName() == str){

				tar->PastePrefabParam(before);

			}
		});
	};
	Window::AddInspector(new InspectorButtonDataSet("Apply", collback), data);

	Window::ViewInspector("Prefab", NULL, data);
}

