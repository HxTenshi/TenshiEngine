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
#include "AssetFile/Animation/AnimationFileData.h"
#include "AssetFile/Material/MaterialFileData.h"

#include "Engine/Inspector.h"


decltype(AssetFactory::m_Factory) AssetFactory::m_Factory;
class __AssetFactory :public AssetFactory{
public: __AssetFactory(){}
};

decltype(AssetDataBase::m_AssetCache) AssetDataBase::m_AssetCache;
decltype(AssetDataBase::m_AssetMetaCache) AssetDataBase::m_AssetMetaCache;
decltype(AssetDataBase::m_DeleteCache) AssetDataBase::m_DeleteCache = std::make_pair("", AssetDataTemplatePtr(NULL));

static __AssetFactory factory;

#include "Engine/Asset/GenerateMetaFile.h"
//static
void AssetDataBase::InitializeMetaData(const char* filename){

	std::string x(filename);
	std::transform(x.begin(), x.end(), x.begin(), [](int x) { return (char)std::tolower(x); });
	auto _filename = x.c_str();

	auto s = (std::string(_filename) + (".meta"));
	auto file = m_AssetCache.find(_filename);

	if (file == m_AssetCache.end()){

		AssetDataTemplatePtr data;
		data = AssetFactory::Create(s.c_str());
		if (data && (AssetFileType::Meta == data->m_AssetFileType)){
			MetaAssetDataPtr meta = data;
			m_AssetMetaCache.insert(std::make_pair(*meta->GetFileData()->GetHash(), std::make_pair(_filename, AssetDataTemplatePtr(NULL))));
		
			m_AssetCache.insert(std::make_pair(_filename, std::make_pair(*meta->GetFileData()->GetHash(), AssetDataTemplatePtr(NULL))));
		
		}
		else{
			MakeMetaFile(_filename);
			data = AssetFactory::Create(s.c_str());
			if (data && (AssetFileType::Meta == data->m_AssetFileType)){
				MetaAssetDataPtr meta = data;
				m_AssetMetaCache.insert(std::make_pair(*meta->GetFileData()->GetHash(), std::make_pair(_filename, AssetDataTemplatePtr(NULL))));
		
				m_AssetCache.insert(std::make_pair(_filename, std::make_pair(*meta->GetFileData()->GetHash(), AssetDataTemplatePtr(NULL))));
		
			}
		}
	}
	else {
		return;
	}

}

AssetFactory::AssetFactory(){
	//ÉÄÅ[ÉrÅ[é¸ÇË
	{
		//MFStartup(MF_VERSION);
	}
	factory<MeshFileData>("tesmesh");
	factory<MeshFileData>("tedmesh");
	factory<MeshFileData>("tedmesh2");

	factory<BoneFileData>("tebone2");
	
	factory<PrefabFileData>("prefab");
	
	factory<ShaderFileData>("fx");

	factory<TextureFileData>("png");
	factory<TextureFileData>("jpg");
	factory<TextureFileData>("jpge");
	factory<TextureFileData>("bmp");
	factory<TextureFileData>("tif");
	factory<TextureFileData>("tga");
	factory<TextureFileData>("hdr");
	
	factory<PhysxMaterialFileData>("pxmaterial");
	
	factory<SoundFileData>("wav");
	factory<SoundFileData>("wave");
	
	factory<MovieFileData>("mp4");
	factory<MovieFileData>("wmv");
	factory<MovieFileData>("avi");
	
	factory<MetaFileData>("meta");
	
	factory<AnimationFileData>("vmd");

	factory<MaterialFileData>("material");
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

		//Game::GetAllObject([&](GameObject tar){
		//	auto str = tar->Prefab();
		//	if (m_FileData->GetFileName() == str){

		//		tar->PastePrefabParam(before);

		//	}
		//});
		Window::ClearInspector();
	};
	Inspector ins("Prefab",NULL);
	ins.AddButton("Save",collback);
	ins.Complete();
}

void AssetDataTemplate<ShaderFileData>::CreateInspector(){

	Window::AddLog(std::to_string((int)this));
	std::function<void()> collback = [&](){
		m_FileData->Create(m_FileData->GetFileName().c_str());

	};

	Inspector ins("Shader", NULL);
	ins.AddButton("Compile", collback);
	ins.Complete();
}

#include "Game/Component/PhysxColliderComponent.h"
void AssetDataTemplate<PhysxMaterialFileData>::CreateInspector(){


	Inspector ins("PhysxMaterial", NULL);

	//std::function<void()> collback = [&](){
	//	m_FileData->Create(m_FileData->GetFileName().c_str());

	//	Game::GetAllObject([&](GameObject tar){
	//		auto com = tar->GetComponent<PhysXColliderComponent>();
	//		if (com){
	//			com->ChangeMaterial(com->GetMaterial());
	//		}
	//	});
	//};

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

	//ins.AddButton("Compile", collback);
	ins.Complete();
}



#include "Graphic/Material/Material.h"
void AssetDataTemplate<MaterialFileData>::CreateInspector() {

	auto mate = m_FileData->GetMaterial();
	if (!mate)return;

	std::function<void(Color)> collbackAlb = [&](Color f) {
		auto mate = m_FileData->GetMaterial();
		if (!mate)return;
		mate->mDiffuse.x = f.r;
		mate->mDiffuse.y = f.g;
		mate->mDiffuse.z = f.b;
		mate->mDiffuse.w = f.a;
		mate->mCBMaterial.mParam.Diffuse = mate->mDiffuse;
	};


	std::function<void(Color)> collbackSpec = [&](Color f) {
		auto mate = m_FileData->GetMaterial();
		if (!mate)return;
		mate->mSpecular.x = f.r;
		mate->mSpecular.y = f.g;
		mate->mSpecular.z = f.b;
		//mSpecular.w = f.a;
		mate->mCBMaterial.mParam.Specular = mate->mSpecular;
	};

	std::function<void(float)> collbackas = [&](float f) {
		auto mate = m_FileData->GetMaterial();
		if (!mate)return;
		mate->mSpecular.w = f;
		mate->mCBMaterial.mParam.Specular = mate->mSpecular;
	};

	std::function<void(Vector2)> collbackTex = [&](Vector2 f) {
		auto mate = m_FileData->GetMaterial();
		if (!mate)return;
		mate->mTexScale.x = f.x;
		mate->mTexScale.y = f.y;
		mate->mCBMaterial.mParam.TexScale = mate->mTexScale;
	};

	std::function<void(Vector2)> collbackOffset = [&](Vector2 f) {
		auto mate = m_FileData->GetMaterial();
		if (!mate)return;
		mate->mOffset.x = f.x;
		mate->mOffset.y = f.y;
		mate->mCBMaterial.mParam.MOffset = mate->mOffset;
	};

	std::function<void(float)> collbackH = [&](float f) {
		auto mate = m_FileData->GetMaterial();
		if (!mate)return;
		mate->mHeightPower.x = f;
		mate->mCBMaterial.mParam.HeightPower = mate->mHeightPower;
	};
	std::function<void(float)> collbackHDR = [&](float f) {
		auto mate = m_FileData->GetMaterial();
		if (!mate)return;
		mate->mHeightPower.y = f;
		mate->mCBMaterial.mParam.HeightPower = mate->mHeightPower;
	};

	std::function<void(Vector3)> collbackNormalScale = [&](Vector3 f) {
		auto mate = m_FileData->GetMaterial();
		if (!mate)return;
		mate->mNormalScale.x = f.x;
		mate->mNormalScale.y = f.y;
		mate->mNormalScale.z = f.z;
		mate->mCBMaterial.mParam.MNormaleScale = mate->mNormalScale;
	};
	std::function<void(float)> collbackThick = [&](float f) {
		mate->mAmbient.w = f;
		mate->mCBMaterial.mParam.Ambient.w = mate->mAmbient.w;
	};

	Inspector ins("Material", NULL);

	auto alb = Color(mate->mDiffuse);
	ins.Add("Albedo", &alb, collbackAlb);

	auto spec = Color(mate->mSpecular);
	ins.Add("Specular", &spec, collbackSpec);
	ins.AddSlideBar("Roughness", 0, 1, &mate->mSpecular.w, collbackas);

	static TextureAsset tex[6];
	tex[0].m_Hash = mate->mTexture[0].GetHash();
	tex[1].m_Hash = mate->mTexture[1].GetHash();
	tex[2].m_Hash = mate->mTexture[2].GetHash();
	tex[3].m_Hash = mate->mTexture[3].GetHash();
	tex[4].m_Hash = mate->mTexture[4].GetHash();
	tex[5].m_Hash = mate->mTexture[5].GetHash();
	ins.Add("AlbedoTextre",		&tex[0], [&]() {
		auto mate = m_FileData->GetMaterial();
		if (!mate)return;
		mate->SetTexture(tex[0], 0);
	});
	ins.Add("NormalTextre",		&tex[1], [&]() {	auto mate = m_FileData->GetMaterial();
	if (!mate)return; mate->SetTexture(tex[1], 1); });
	ins.Add("HeightTextre",		&tex[2], [&]() {	auto mate = m_FileData->GetMaterial();
	if (!mate)return; mate->SetTexture(tex[2], 2); });
	ins.Add("SpecularTextre",	&tex[3], [&]() {	auto mate = m_FileData->GetMaterial();
	if (!mate)return;mate->SetTexture(tex[3], 3);});
	ins.Add("RoughnessTextre",	&tex[4], [&]() {	auto mate = m_FileData->GetMaterial();
	if (!mate)return;mate->SetTexture(tex[4], 4);});
	ins.Add("EmissiveTextre",	&tex[5], [&]() {	auto mate = m_FileData->GetMaterial();
	if (!mate)return;mate->SetTexture(tex[5], 5);});
	ins.Add("EmissivePowor", &mate->mEmissivePowor, [&](float f) {
		auto mate = m_FileData->GetMaterial();
		if (!mate)return; mate->mEmissivePowor = f;
		mate->mCBMaterial.mParam.EmissivePowor = mate->mEmissivePowor;
	});

	auto texs = Vector2(mate->mTexScale);
	ins.Add("TextureScale", &texs, collbackTex);

	auto texNormal = Vector3(mate->mNormalScale);
	ins.Add("NormaleScale", &texNormal, collbackNormalScale);

	auto off = Vector2(mate->mOffset);
	ins.Add("Offset", &off, collbackOffset);

	ins.AddSlideBar("HightPower", -10, 10, &mate->mHeightPower.x, collbackH);
	ins.Add("HDR", &mate->mHeightPower.y, collbackHDR);
	ins.Add("Thickness", &mate->mAmbient.w, collbackThick);

	static ShaderAsset shader;
	shader.m_Hash = mate->mShader.GetAsset().m_Hash;
	ins.Add("Shader", &shader, [&]() {
		auto mate = m_FileData->GetMaterial();
		if (!mate)return;
		mate->CreateShader(shader);
	});

	ins.AddButton("SaveMaterial", [&]() {m_FileData->SaveFile(); });

	ins.Complete();
}

#endif