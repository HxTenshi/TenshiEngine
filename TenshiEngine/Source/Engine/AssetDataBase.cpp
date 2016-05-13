#include "AssetDataBase.h"

#include "Device/DirectX11Device.h"


#include "MySTL/Utility.h"

#include "Window/Window.h"

#include "AssetFile\Bone\BoneFileData.h"
#include "AssetFile\Mesh\MeshFileData.h"
#include "AssetFile\Prefab\PrefabFileData.h"
#include "AssetFile\Shader\ShaderFileData.h"
#include "AssetFile\Material\TextureFileData.h"
#include "AssetFile\Physx\PhysxMaterialFileData.h"

decltype(AssetFactory::m_Factory) AssetFactory::m_Factory;
class __AssetFactory :public AssetFactory{
public: __AssetFactory(){}
};

decltype(AssetDataBase::m_AssetCache) AssetDataBase::m_AssetCache;

static __AssetFactory factory;

AssetFactory::AssetFactory(){
	m_Factory.insert(std::make_pair<std::string, std::function<AssetDataTemplatePtr(const char*)>>(std::string("tesmesh"), [](const char* filename){ return AssetDataTemplate<MeshFileData>::Create(filename); }));
	m_Factory.insert(std::make_pair<std::string, std::function<AssetDataTemplatePtr(const char*)>>(std::string("tedmesh"), [](const char* filename){ return AssetDataTemplate<MeshFileData>::Create(filename); }));
	m_Factory.insert(std::make_pair<std::string, std::function<AssetDataTemplatePtr(const char*)>>(std::string("tedmesh2"), [](const char* filename){ return AssetDataTemplate<MeshFileData>::Create(filename); }));

	m_Factory.insert(std::make_pair<std::string, std::function<AssetDataTemplatePtr(const char*)>>(std::string("tebone"), [](const char* filename){ return AssetDataTemplate<BoneFileData>::Create(filename); }));

	m_Factory.insert(std::make_pair<std::string, std::function<AssetDataTemplatePtr(const char*)>>(std::string("json"), [](const char* filename){ return AssetDataTemplate<PrefabFileData>::Create(filename); }));

	m_Factory.insert(std::make_pair<std::string, std::function<AssetDataTemplatePtr(const char*)>>(std::string("fx"), [](const char* filename){ return AssetDataTemplate<ShaderFileData>::Create(filename); }));

	m_Factory.insert(std::make_pair<std::string, std::function<AssetDataTemplatePtr(const char*)>>(std::string("png"), [](const char* filename){ return AssetDataTemplate<TextureFileData>::Create(filename); }));
	m_Factory.insert(std::make_pair<std::string, std::function<AssetDataTemplatePtr(const char*)>>(std::string("jpg"), [](const char* filename){ return AssetDataTemplate<TextureFileData>::Create(filename); }));
	m_Factory.insert(std::make_pair<std::string, std::function<AssetDataTemplatePtr(const char*)>>(std::string("jpge"), [](const char* filename){ return AssetDataTemplate<TextureFileData>::Create(filename); }));
	m_Factory.insert(std::make_pair<std::string, std::function<AssetDataTemplatePtr(const char*)>>(std::string("bmp"), [](const char* filename){ return AssetDataTemplate<TextureFileData>::Create(filename); }));
	m_Factory.insert(std::make_pair<std::string, std::function<AssetDataTemplatePtr(const char*)>>(std::string("tif"), [](const char* filename){ return AssetDataTemplate<TextureFileData>::Create(filename); }));
	m_Factory.insert(std::make_pair<std::string, std::function<AssetDataTemplatePtr(const char*)>>(std::string("tga"), [](const char* filename){ return AssetDataTemplate<TextureFileData>::Create(filename); }));

	m_Factory.insert(std::make_pair<std::string, std::function<AssetDataTemplatePtr(const char*)>>(std::string("pxmaterial"), [](const char* filename){ return AssetDataTemplate<PhysxMaterialFileData>::Create(filename); }));

}
AssetDataTemplatePtr AssetFactory::Create(const char* filename){

	std::string str = filename;

	auto type = behind_than_find_last_of(str, ".");
	if (type == "") return NULL;

	auto make = m_Factory.find(type);
	if (make == m_Factory.end())return NULL;

	return make->second(filename);

}

void AssetDataTemplate<MeshFileData>::CreateInspector(){

}

void AssetDataTemplate<BoneFileData>::CreateInspector(){

}

#include "Game/Game.h"
void AssetDataTemplate<PrefabFileData>::CreateInspector(){
	m_FileData->GetActor()->CreateInspector();

	auto data = Window::CreateInspector();
	std::function<void()> collback = [&](){
		auto before = m_FileData->Apply();

		Game::GetAllObject([&](Actor* tar){
			auto str = tar->Prefab();
			if (m_FileData->GetFileName() == str){

				tar->PastePrefabParam(before);

			}
		});
	};
	Window::AddInspector(new InspectorButtonDataSet("Apply", collback), data);

	Window::ViewInspector("Prefab", NULL, data);
}

void AssetDataTemplate<ShaderFileData>::CreateInspector(){

	auto data = Window::CreateInspector();
	std::function<void()> collback = [&](){
		m_FileData->FileUpdate();

	};
	Window::AddInspector(new InspectorButtonDataSet("Compile", collback), data);

	Window::ViewInspector("Shader", NULL, data);
}

#include "Game/Component/PhysxColliderComponent.h"
void AssetDataTemplate<PhysxMaterialFileData>::CreateInspector(){

	auto data = Window::CreateInspector();
	std::function<void()> collback = [&](){
		m_FileData->FileUpdate();

		
		Game::GetAllObject([&](Actor* tar){
			auto com = tar->GetComponent<PhysXColliderComponent>();
			if (com){
				com->ChangeMaterial();
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

		Window::AddInspector(new TemplateInspectorDataSet<float>("StaticFriction", &m_FileData->m_Param_sf, collbackf1), data);
		Window::AddInspector(new TemplateInspectorDataSet<float>("DynamicFriction", &m_FileData->m_Param_df, collbackf2), data);
		Window::AddInspector(new TemplateInspectorDataSet<float>("Restitution", &m_FileData->m_Param_r, collbackf3), data);
	}

	Window::AddInspector(new InspectorButtonDataSet("Compile", collback), data);

	Window::ViewInspector("PhysxMaterial", NULL, data);
}