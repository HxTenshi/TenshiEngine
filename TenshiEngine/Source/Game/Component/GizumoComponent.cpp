#include "GizumoComponent.h"

#include "Graphic/Model/Model.h"

#include "Engine/Inspector.h"
#include "Engine/AssetLoad.h"
#include "Game/Game.h"

GizumoComponent::GizumoComponent()
{
	mModel = NULL;
	m_TextureAsset.Free();
}

GizumoComponent::~GizumoComponent()
{
	if (mModel) {
		mModel->Release();
		delete mModel;
		mModel = NULL;
	}
}

void GizumoComponent::Initialize()
{

	if (!mModel) {
		mModel = new Model();
	}
	if (!mModel->IsCreate()) {
		mModel->Create("EngineResource/TextureModel.tesmesh");
	}

	AssetLoad::Instance(m_TextureAsset.m_Hash, m_TextureAsset);

	if (!m_Material.IsCreate()) {
		m_Material.mDiffuse = XMFLOAT4(0.47f, 0.4f, 0.4f, 1.0f);
		m_Material.Create("EngineResource/Billboard.fx");
		m_Material.SetTexture(m_TextureAsset, 0);
	}
}

void GizumoComponent::Start()
{
}

#ifdef _ENGINE_MODE
void GizumoComponent::EngineUpdate()
{
	if (!mModel)return;


	Game::AddDrawList(DrawStage::Init, std::function<void()>([&]() {
		mModel->mWorld = XMMatrixMultiply(
			XMMatrixScaling(0.5f, 0.5f, 0.5f),
			XMMatrixTranslationFromVector(gameObject->mTransform->WorldPosition()));
		mModel->Update();
	}));
	Game::AddDrawList(DrawStage::Engine, std::function<void()>([this]() {
		auto render = RenderingEngine::GetEngine(ContextType::MainDeferrd);
		mModel->Draw(render->m_Context, m_Material);
	}));
}
#endif

void GizumoComponent::Update()
{
}

void GizumoComponent::Finish()
{
}

#ifdef _ENGINE_MODE
void GizumoComponent::CreateInspector()
{
	Inspector ins("Gizumo", this);
	ins.AddEnableButton(this);

	ins.Add("Texture", &m_TextureAsset, [&]() {
		m_Material.SetTexture(m_TextureAsset, 0);
	});
	ins.Complete();
}
#endif

void GizumoComponent::IO_Data(I_ioHelper * io)
{
	Enabled::IO_Data(io);

#define _KEY(x) io->func( x , #x)
	_KEY(m_TextureAsset);
#undef _KEY
}
