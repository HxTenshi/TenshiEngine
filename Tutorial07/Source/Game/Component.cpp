#include "Component.h"

#include "../ScriptComponent/main.h"
typedef IDllScriptComponent* (__cdecl *CreateInstance_)(const char*);
typedef void(__cdecl *DeleteInstance_)(IDllScriptComponent*);

std::map<std::string, std::function<Component*()>> ComponentFactory::mFactoryComponents;
bool ComponentFactory::mIsInit = false;

//同じDLLを持ってるコンポーネントを管理
class UseScriptActors{
public:
	UseScriptActors(){
		hModule = NULL;
		mCreate = NULL;
		mDelete = NULL;
		DllLoad();
	}
	~UseScriptActors(){
		UnLoad();
	}
	void ReCompile(){
		for (auto& p : mList){
			p->Unload();
		}

		UnLoad();

		char cdir[255];
		GetCurrentDirectory(255, cdir);
		std::string  pass = cdir;
#ifdef _DEBUG
		pass += "/ScriptComponent/createdll_auto_d.bat";
#else
		pass += "/ScriptComponent/createdll_auto.bat";
#endif

		SHELLEXECUTEINFO	sei = { 0 };
		//構造体のサイズ
		sei.cbSize = sizeof(SHELLEXECUTEINFO);
		//起動側のウインドウハンドル
		sei.hwnd = Window::GetMainHWND();
		//起動後の表示状態
		sei.nShow = SW_SHOWNORMAL;
		//このパラメータが重要で、セットしないとSHELLEXECUTEINFO構造体のhProcessメンバがセットされない。
		sei.fMask = SEE_MASK_NOCLOSEPROCESS;
		//起動プログラム
		sei.lpFile = pass.c_str();
		//プロセス起動
		if (!ShellExecuteEx(&sei) || (const int)sei.hInstApp <= 32){
			MessageBox(Window::GetMainHWND(), "ファイルを開けませんでした", "失敗", MB_OK);
			return;
		}
		//終了を待つ
		WaitForSingleObject(sei.hProcess, INFINITE);

		DllLoad();

		for (auto& p : mList){
			p->Load();
		}
	}

	void UnLoad(){
		FreeLibrary(hModule);
		hModule = NULL;
		mCreate = NULL;
		mDelete = NULL;
	}
	void DllLoad(){

		// DLLのロード
#ifdef _DEBUG
		hModule = LoadLibrary("../ScriptComponent/Debug/ScriptComponent.dll");
#else
		hModule = LoadLibrary("../ScriptComponent/Release/ScriptComponent.dll");
#endif
		if (hModule == NULL)
		{
#ifdef _DEBUG
			hModule = LoadLibrary("ScriptComponent/Debug/ScriptComponent.dll");
#else
			hModule = LoadLibrary("ScriptComponent/Release/ScriptComponent.dll");
#endif
		}


		mCreate = (CreateInstance_)GetProcAddress(hModule, "CreateInstance");
		if (mCreate == NULL)
		{
			FreeLibrary(hModule);
			hModule = NULL;
			return;
		}
		mDelete = (DeleteInstance_)GetProcAddress(hModule, "ReleseInstance");
		if (mDelete == NULL)
		{
			mCreate = NULL;
			FreeLibrary(hModule);
			hModule = NULL;
			return;
		}
	}
	IDllScriptComponent* Create(const std::string& ClassName){
		if (!mCreate)return NULL;
		//dllで作成したクラスインスタンスを作成する
		return ((CreateInstance_)mCreate)(ClassName.c_str());
	}

	void Deleter(IDllScriptComponent* script){
		((DeleteInstance_)mDelete)(script);
	}
	std::list<ScriptComponent*> mList;
	void* mCreate;
	void* mDelete;
	HMODULE hModule;
};
static UseScriptActors actors;


//static
void ScriptManager::ReCompile(){
	actors.ReCompile();
}

ScriptComponent::ScriptComponent(){
	pDllClass = NULL;
}
ScriptComponent::~ScriptComponent(){
	Unload();
	actors.mList.remove(this);
}
void ScriptComponent::Initialize(){
	Load();
	actors.mList.push_back(this);
}
void ScriptComponent::Load(){
	if (pDllClass)return;

	//dllで作成したクラスインスタンスを作成する
	pDllClass = actors.Create(mClassName);
}
void ScriptComponent::Unload(){

	if (pDllClass)
		actors.Deleter(pDllClass);

	pDllClass = NULL;
}
void ScriptComponent::ReCompile(){
	Unload();
	Load();

}
void ScriptComponent::Update(){

	if (pDllClass){
		pDllClass->Update(gameObject);
	}
}


#include "Game.h"
void MeshDrawComponent::Update(){

	if (!mModel){
		mModel = gameObject->GetComponent<ModelComponent>();
		if (!mModel)
			mModel = gameObject->GetComponent<TextureModelComponent>();
		if (!mModel)return;
	}
	if (!mModel->mModel)return;

	if (!mMaterial){
		mMaterial = gameObject->GetComponent<MaterialComponent>();
	}
	if (!mMaterial)return;

	Game::AddDrawList(DrawStage::Diffuse, std::function<void()>([&](){
		mModel->SetMatrix();
		Model& model = *mModel->mModel;

		if (mWriteDepth){
			model.Draw(mMaterial);
		}
		else{
			NoWriteDepth(model);
		}
	}));

}

void TextComponent::DrawTextUI(){
	Game::AddDrawList(DrawStage::UI, [&](){

		if (!mModel)return;
		mModel->SetMatrix();
		Model& model = *mModel->mModel;

		ID3D11DepthStencilState* pBackDS;
		UINT ref;
		Device::mpImmediateContext->OMGetDepthStencilState(&pBackDS, &ref);

		D3D11_DEPTH_STENCIL_DESC descDS = CD3D11_DEPTH_STENCIL_DESC(CD3D11_DEFAULT());
		descDS.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		//descDS.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		descDS.DepthFunc = D3D11_COMPARISON_ALWAYS;
		ID3D11DepthStencilState* pDS_tex = NULL;
		Device::mpd3dDevice->CreateDepthStencilState(&descDS, &pDS_tex);
		Device::mpImmediateContext->OMSetDepthStencilState(pDS_tex, 0);

		D3D11_RASTERIZER_DESC descRS = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT());
		descRS.CullMode = D3D11_CULL_BACK;
		descRS.FillMode = D3D11_FILL_SOLID;

		ID3D11RasterizerState* pRS = NULL;
		Device::mpd3dDevice->CreateRasterizerState(&descRS, &pRS);

		Device::mpImmediateContext->RSSetState(pRS);

		model.Draw(mTexMaterial);

		Device::mpImmediateContext->RSSetState(NULL);
		if (pRS)pRS->Release();

		Device::mpImmediateContext->OMSetDepthStencilState(NULL, 0);
		pDS_tex->Release();

		Device::mpImmediateContext->OMSetDepthStencilState(pBackDS, ref);
		if (pBackDS)pBackDS->Release();
	});
}

void ModelComponent::CreateInspector(){
	auto data = Window::CreateInspector();
	Window::AddInspector(new InspectorLabelDataSet("Model"), data);
	std::function<void(std::string)> collbackpath = [&](std::string name){
		Window::ClearInspector();
		mFileName = name;
		if (mModel){
			mModel->Release();
			delete mModel;
		}
		mModel = new Model();
		//mModel->Create(mFileName.c_str(), shared_ptr<MaterialComponent>());
		shared_ptr<MaterialComponent> material = shared_ptr<MaterialComponent>(new MaterialComponent());
		mModel->Create(mFileName.c_str(), material);
		gameObject->RemoveComponent<MaterialComponent>();
		gameObject->AddComponent<MaterialComponent>(material);

	};
	Window::AddInspector(new InspectorStringDataSet("Model", &mFileName, collbackpath), data);
	Window::ViewInspector("Model",data);
}

void CameraComponent::Update(){
	XMVECTOR null;
	gameObject->mTransform->Scale(XMVectorSet(1, 1, 1, 1));
	mView = XMMatrixInverse(&null, gameObject->mTransform->GetMatrix());
	mCBNeverChanges.mParam.mView = XMMatrixTranspose(mView);
	mCBNeverChanges.UpdateSubresource();
	Game::SetMainCamera(this);
}