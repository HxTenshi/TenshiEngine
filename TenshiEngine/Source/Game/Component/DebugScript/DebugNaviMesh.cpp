#include "DebugNaviMesh.h"

#include "Window/Window.h"

#include "Game/Game.h"

DebugNaviMesh::DebugNaviMesh(){
	mStart = NULL;
	mEnd = NULL;
	mGameObject = NULL;
	mSpeed = 1.0f;
}
DebugNaviMesh::~DebugNaviMesh(){

}
void DebugNaviMesh::Initialize(Actor* gameObject){
	mGameObject = gameObject;
	mNaviMesh.Create(mFileName);
}

void DebugNaviMesh::EngineUpdate(){

	static bool Init = false;
	static Shader mShader;
	static Material mMaterial; 
	static ConstantBuffer<CBChangesEveryFrame> mLineCBuffer;
	static ID3D11Buffer*	mpVertexBuffer;
	static ID3D11Buffer*	mpIndexBuffer;

	if (!Init){
		Init = true;
		mShader.Create("EngineResource/DebugLine.fx");

		mMaterial.mDiffuse = XMFLOAT4{ 1.0f, 0.2f, 0.2f, 1 };
		mMaterial.Create();


		auto render = RenderingEngine::GetEngine(ContextType::MainDeferrd);

		mLineCBuffer = ConstantBuffer<CBChangesEveryFrame>::create(2);
		mLineCBuffer.mParam.mWorld = XMMatrixIdentity();
		mLineCBuffer.UpdateSubresource(render->m_Context);

		{
			XMFLOAT4 Vertices[] = { { 0.0f, 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } };
			UINT TypeSize = sizeof(XMFLOAT4);
			UINT VertexNum = 2;

			D3D11_BUFFER_DESC bd;
			ZeroMemory(&bd, sizeof(bd));
			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.ByteWidth = TypeSize * VertexNum;
			bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bd.CPUAccessFlags = 0;
			D3D11_SUBRESOURCE_DATA InitData;
			ZeroMemory(&InitData, sizeof(InitData));
			InitData.pSysMem = Vertices;
			Device::mpd3dDevice->CreateBuffer(&bd, &InitData, &mpVertexBuffer);
		}
		{
			WORD Indices[] = { 0, 1 };
			UINT IndexNum = 2;

			D3D11_BUFFER_DESC bd;
			ZeroMemory(&bd, sizeof(bd));
			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.ByteWidth = sizeof(WORD) * IndexNum;
			bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
			bd.CPUAccessFlags = 0;
			D3D11_SUBRESOURCE_DATA InitData;
			ZeroMemory(&InitData, sizeof(InitData));
			InitData.pSysMem = Indices;
			Device::mpd3dDevice->CreateBuffer(&bd, &InitData, &mpIndexBuffer);
		}

	}

	if (mNavigatePtr){

		for (auto& navi : mNavigatePtr->GetRoute()){

			auto pos = navi.Position;
			auto v = navi.Line;

			Game::AddDrawList(DrawStage::Engine, [&, pos,v](){

				auto render = RenderingEngine::GetEngine(ContextType::MainDeferrd);

				render->m_Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

				render->PushSet(DepthStencil::Preset::DS_Zero_Alawys);

				mShader.SetShader(false, render->m_Context);
				mMaterial.PSSetShaderResources(render->m_Context);
				UINT mStride = sizeof(XMFLOAT4);
				UINT offset = 0;
				render->m_Context->IASetVertexBuffers(0, 1, &mpVertexBuffer, &mStride, &offset);
				render->m_Context->IASetIndexBuffer(mpIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

				mLineCBuffer.mParam.mWorld.r[0] = pos;
				mLineCBuffer.mParam.mWorld.r[1] = pos+v;
				mLineCBuffer.UpdateSubresource(render->m_Context);
				mLineCBuffer.VSSetConstantBuffers(render->m_Context);

				render->m_Context->DrawIndexed(2, 0, 0);

				render->m_Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

				render->PopDS();
			});

		}

	}

}
void DebugNaviMesh::Update(){

	if (mNavigatePtr && mGameObject){
		mNavigatePtr->Move(mSpeed);
		auto pos = mNavigatePtr->GetPosition();
		mGameObject->mTransform->Position(pos);
	}
}
void DebugNaviMesh::Finish(){

}

#ifdef _ENGINE_MODE
void DebugNaviMesh::CreateInspector(std::vector<InspectorDataSet>& data){

	Window::AddInspector(new TemplateInspectorDataSet<std::string>("Class", &mFileName, [&](std::string f){mFileName = f;
		mNaviMesh.Create(mFileName);
	}), data);

	Window::AddInspector(new TemplateInspectorDataSet<float>("speed", &mSpeed, [&](float f){mSpeed = f;
	}), data);
	Window::AddInspector(new InspectorButtonDataSet("Find", [&](){
		Find();
	}), data);
	Window::AddInspector(new InspectorButtonDataSet("n1", [&](){
		mNaviMeshCreatorPtr->N1();
		mNavigatePtr = mNaviMeshCreatorPtr->Result();
	}), data);
	Window::AddInspector(new InspectorButtonDataSet("n2", [&](){

		mNaviMeshCreatorPtr->N2();
		mNavigatePtr = mNaviMeshCreatorPtr->Result();
	}), data);

	
}
#endif

void DebugNaviMesh::IO_Data(I_ioHelper* io){
#define _KEY(x) io->func( x , #x)
	_KEY(mFileName);
	_KEY(mSpeed);
#undef _KEY
}

void DebugNaviMesh::Find(){
	auto start = Game::FindNameActor("NaviStart");
	auto end = Game::FindNameActor("NaviEnd");

	mStart = mNaviMesh.FindNaviPosition(start->mTransform->WorldPosition());
	mEnd = mNaviMesh.FindNaviPosition(end->mTransform->WorldPosition());

	mNaviMeshCreatorPtr = make_shared<NavigateCreator>(mStart, mEnd, &mNaviMesh);

	mNavigatePtr = mNaviMeshCreatorPtr->Result();
}