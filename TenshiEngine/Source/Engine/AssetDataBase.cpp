#include "AssetDataBase.h"

#include "Device/DirectX11Device.h"
#include "Physx/Physx3.h"


#include "MySTL/Utility.h"

#include "Window/Window.h"

#include "Library\movie\hwndMovie.h"

#include "AssetFile\MetaFileData.h"

#include "AssetFile\Bone\BoneFileData.h"
#include "AssetFile\Mesh\MeshFileData.h"
#include "AssetFile\Prefab\PrefabFileData.h"
#include "AssetFile\Shader\ShaderFileData.h"
#include "AssetFile\Material\TextureFileData.h"
#include "AssetFile\Physx\PhysxMaterialFileData.h"
#include "AssetFile\Sound\SoundFileData.h"
#include "AssetFile\Movie\MovieFileData.h"

#include "Engine/Inspector.h"


decltype(AssetFactory::m_Factory) AssetFactory::m_Factory;
class __AssetFactory :public AssetFactory{
public: __AssetFactory(){}
};

decltype(AssetDataBase::m_AssetCache) AssetDataBase::m_AssetCache;
decltype(AssetDataBase::m_AssetMetaCache) AssetDataBase::m_AssetMetaCache;

static __AssetFactory factory;

#include "Engine/Asset/GenerateMetaFile.h"
//static
void AssetDataBase::InitializeMetaData(const char* filename){


	auto s = (std::string(filename) + (".meta"));
	auto file = m_AssetCache.find(filename);

	if (file == m_AssetCache.end()){

		AssetDataTemplatePtr data;
		data = AssetFactory::Create(s.c_str());
		if (data && (AssetFileType::Meta == data->m_AssetFileType)){
			MetaAssetDataPtr meta = data;
			m_AssetMetaCache.insert(std::make_pair(*meta->GetFileData()->GetHash(), filename));
		
			m_AssetCache.insert(std::make_pair(filename, std::make_pair(*meta->GetFileData()->GetHash(), AssetDataTemplatePtr(NULL))));
		
		}
		else{
			MakeMetaFile(filename);
			data = AssetFactory::Create(s.c_str());
			if (data && (AssetFileType::Meta == data->m_AssetFileType)){
				MetaAssetDataPtr meta = data;
				m_AssetMetaCache.insert(std::make_pair(*meta->GetFileData()->GetHash(), filename));
		
				m_AssetCache.insert(std::make_pair(filename, std::make_pair(*meta->GetFileData()->GetHash(), AssetDataTemplatePtr(NULL))));
		
			}
		}
	}

}


AssetFactory::AssetFactory(){
	//ムービー周り
	{
		//MFStartup(MF_VERSION);
	}

	m_Factory.insert(std::make_pair<std::string, std::function<AssetDataTemplatePtr(const char*)>>(std::string("tesmesh"), [](const char* filename){ return AssetDataTemplate<MeshFileData>::Create(filename); }));
	m_Factory.insert(std::make_pair<std::string, std::function<AssetDataTemplatePtr(const char*)>>(std::string("tedmesh"), [](const char* filename){ return AssetDataTemplate<MeshFileData>::Create(filename); }));
	m_Factory.insert(std::make_pair<std::string, std::function<AssetDataTemplatePtr(const char*)>>(std::string("tedmesh2"), [](const char* filename){ return AssetDataTemplate<MeshFileData>::Create(filename); }));

	m_Factory.insert(std::make_pair<std::string, std::function<AssetDataTemplatePtr(const char*)>>(std::string("tebone2"), [](const char* filename){ return AssetDataTemplate<BoneFileData>::Create(filename); }));

	m_Factory.insert(std::make_pair<std::string, std::function<AssetDataTemplatePtr(const char*)>>(std::string("json"), [](const char* filename){ return AssetDataTemplate<PrefabFileData>::Create(filename); }));

	m_Factory.insert(std::make_pair<std::string, std::function<AssetDataTemplatePtr(const char*)>>(std::string("fx"), [](const char* filename){ return AssetDataTemplate<ShaderFileData>::Create(filename); }));

	m_Factory.insert(std::make_pair<std::string, std::function<AssetDataTemplatePtr(const char*)>>(std::string("png"), [](const char* filename){ return AssetDataTemplate<TextureFileData>::Create(filename); }));
	m_Factory.insert(std::make_pair<std::string, std::function<AssetDataTemplatePtr(const char*)>>(std::string("jpg"), [](const char* filename){ return AssetDataTemplate<TextureFileData>::Create(filename); }));
	m_Factory.insert(std::make_pair<std::string, std::function<AssetDataTemplatePtr(const char*)>>(std::string("jpge"), [](const char* filename){ return AssetDataTemplate<TextureFileData>::Create(filename); }));
	m_Factory.insert(std::make_pair<std::string, std::function<AssetDataTemplatePtr(const char*)>>(std::string("bmp"), [](const char* filename){ return AssetDataTemplate<TextureFileData>::Create(filename); }));
	m_Factory.insert(std::make_pair<std::string, std::function<AssetDataTemplatePtr(const char*)>>(std::string("tif"), [](const char* filename){ return AssetDataTemplate<TextureFileData>::Create(filename); }));
	m_Factory.insert(std::make_pair<std::string, std::function<AssetDataTemplatePtr(const char*)>>(std::string("tga"), [](const char* filename){ return AssetDataTemplate<TextureFileData>::Create(filename); }));
	m_Factory.insert(std::make_pair<std::string, std::function<AssetDataTemplatePtr(const char*)>>(std::string("hdr"), [](const char* filename){ return AssetDataTemplate<TextureFileData>::Create(filename); }));

	m_Factory.insert(std::make_pair<std::string, std::function<AssetDataTemplatePtr(const char*)>>(std::string("pxmaterial"), [](const char* filename){ return AssetDataTemplate<PhysxMaterialFileData>::Create(filename); }));

	m_Factory.insert(std::make_pair<std::string, std::function<AssetDataTemplatePtr(const char*)>>(std::string("wav"), [](const char* filename){ return AssetDataTemplate<SoundFileData>::Create(filename); }));
	m_Factory.insert(std::make_pair<std::string, std::function<AssetDataTemplatePtr(const char*)>>(std::string("wave"), [](const char* filename){ return AssetDataTemplate<SoundFileData>::Create(filename); }));

	m_Factory.insert(std::make_pair<std::string, std::function<AssetDataTemplatePtr(const char*)>>(std::string("mp4"), [](const char* filename){ return AssetDataTemplate<MovieFileData>::Create(filename); }));
	m_Factory.insert(std::make_pair<std::string, std::function<AssetDataTemplatePtr(const char*)>>(std::string("wmv"), [](const char* filename){ return AssetDataTemplate<MovieFileData>::Create(filename); }));
	m_Factory.insert(std::make_pair<std::string, std::function<AssetDataTemplatePtr(const char*)>>(std::string("avi"), [](const char* filename){ return AssetDataTemplate<MovieFileData>::Create(filename); }));

	m_Factory.insert(std::make_pair<std::string, std::function<AssetDataTemplatePtr(const char*)>>(std::string("meta"), [](const char* filename){ return AssetDataTemplate<MetaFileData>::Create(filename); }));
}


AssetFactory::~AssetFactory(){

}
AssetDataTemplatePtr AssetFactory::Create(const char* filename){

	std::string str = filename;

	auto type = behind_than_find_last_of(str, ".");
	if (type == "") return NULL;

	auto make = m_Factory.find(type);
	if (make == m_Factory.end())return NULL;

	return make->second(filename);

}

#ifdef _ENGINE_MODE

void AssetDataTemplate<MeshFileData>::CreateInspector(){

}

void AssetDataTemplate<BoneFileData>::CreateInspector(){

}

#include "Game/Game.h"
void AssetDataTemplate<PrefabFileData>::CreateInspector(){
	m_FileData->GetActor()->CreateInspector();

	
	std::function<void()> collback = [&](){
		auto before = m_FileData->Apply();

		Game::GetAllObject([&](GameObject tar){
			auto str = tar->Prefab();
			if (m_FileData->GetFileName() == str){

				tar->PastePrefabParam(before);

			}
		});
	};
	Inspector ins("Prefab",NULL);
	ins.AddButton("Apply",collback);
	ins.Complete();
}

void AssetDataTemplate<ShaderFileData>::CreateInspector(){

	std::function<void()> collback = [&](){
		m_FileData->FileUpdate();

	};

	Inspector ins("Shader", NULL);
	ins.AddButton("Compile", collback);
	ins.Complete();
}

#include "Game/Component/PhysxColliderComponent.h"
void AssetDataTemplate<PhysxMaterialFileData>::CreateInspector(){


	Inspector ins("PhysxMaterial", NULL);

	std::function<void()> collback = [&](){
		m_FileData->FileUpdate();

		Game::GetAllObject([&](GameObject tar){
			auto com = tar->GetComponent<PhysXColliderComponent>();
			if (com){
				com->ChangeMaterial(com->GetMaterial());
			}
		});
	};

	auto pmate = m_FileData->GetMaterial();

	if (pmate){

		std::function<void(float)> collbackf1 = [&](float value){

			m_FileData->m_Param_sf = value;
			auto mate = m_FileData->GetMaterial();
			mate->setStaticFriction(value);
			m_FileData->SaveFile();
		};
		std::function<void(float)> collbackf2 = [&](float value){

			m_FileData->m_Param_df = value;
			auto mate = m_FileData->GetMaterial();
			mate->setDynamicFriction(value);
			m_FileData->SaveFile();
		};
		std::function<void(float)> collbackf3 = [&](float value){

			m_FileData->m_Param_r = value;
			auto mate = m_FileData->GetMaterial();
			mate->setRestitution(value);
			m_FileData->SaveFile();
		};

		m_FileData->m_Param_sf = pmate->getStaticFriction();
		m_FileData->m_Param_df = pmate->getDynamicFriction();
		m_FileData->m_Param_r = pmate->getRestitution();

		ins.Add("StaticFriction", &m_FileData->m_Param_sf, collbackf1);
		ins.Add("DynamicFriction", &m_FileData->m_Param_df, collbackf2);
		ins.Add("Restitution", &m_FileData->m_Param_r, collbackf3);
	}

	ins.AddButton("Compile", collback);
	ins.Complete();
}

#endif