#include "NaviMeshComponent.h"

#include "Window/Window.h"

#include "Game/Game.h"

#include "Engine/Inspector.h"
#include "Engine/AssetLoad.h"

NaviMeshComponent::NaviMeshComponent() {
	mStart = NULL;
	mEnd = NULL;

	mStartTarget = NULL;
	mEndTarget = NULL;

#ifdef _ENGINE_MODE
	m_EngineRouteView=false;
	m_EngineMeshView =false;
	m_EngineWallView =false;
#endif
}
NaviMeshComponent::~NaviMeshComponent() {

}
void NaviMeshComponent::Initialize() {
	AssetLoad::Instance(m_Mesh.m_Hash, m_Mesh);
	SetNaviMesh(m_Mesh);
}

#ifdef _ENGINE_MODE
void NaviMeshComponent::EngineUpdate() {
	static bool Init = false;
	static Shader mShader;
	static Material mMaterial;
	static ConstantBuffer<CBChangesEveryFrame> mLineCBuffer;
	static ID3D11Buffer*	mpVertexBuffer;
	static ID3D11Buffer*	mpIndexBuffer;

	if (!Init) {
		Init = true;
		mShader.Create("EngineResource/DebugLine.fx");

		mMaterial.SetAlbedo( XMFLOAT4{ 1.0f, 0.2f, 0.2f, 1 });
		mMaterial.Create();


		auto render = RenderingEngine::GetEngine(ContextType::MainDeferrd);

		mLineCBuffer = ConstantBuffer<CBChangesEveryFrame>::create(2);
		mLineCBuffer.mParam.mWorld = XMMatrixIdentity();
		mLineCBuffer.UpdateSubresource(render->m_Context);

		{
			XMFLOAT4 Vertices[] = { { 0.0f, 0.0f, 0.0f, 1.0f },{ 0.0f, 0.0f, 1.0f, 1.0f } };
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
	
	if (mNavigatePtr &&m_EngineRouteView) {

		for (auto& navi : mNavigatePtr->GetRoute()) {

			auto pos = navi.Position;
			auto v = navi.Line;

			Game::AddDrawList(DrawStage::Engine, [&, pos, v]() {

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
				mLineCBuffer.mParam.mWorld.r[1] = pos + v;
				mLineCBuffer.UpdateSubresource(render->m_Context);
				mLineCBuffer.VSSetConstantBuffers(render->m_Context);

				render->m_Context->DrawIndexed(2, 0, 0);

				render->m_Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

				render->PopDS();
			});

		}

	}
	if (mNaviMesh.GetNaviMeshNum() != 0&& m_EngineMeshView) {

		for (auto& navi : mNaviMesh.GetNaviMeshData()) {

			auto pos = navi.GetData().CenterPosition;

			for (int i = 0; i < 3; i++) {
				auto link = navi.GetLink()[i];
				if (link == NULL)continue;
				auto v = link->GetData().CenterPosition;

				Game::AddDrawList(DrawStage::Engine, [&, pos, v]() {

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
					mLineCBuffer.mParam.mWorld.r[1] = v;
					mLineCBuffer.UpdateSubresource(render->m_Context);
					mLineCBuffer.VSSetConstantBuffers(render->m_Context);

					render->m_Context->DrawIndexed(2, 0, 0);

					render->m_Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

					render->PopDS();
				});

			}
		}
	}

	if (mNaviMesh.GetNaviMeshNum() != 0 && m_EngineWallView) {

		for (auto& navi : mNaviMesh.GetNaviMeshWallData()) {

			auto pos = navi.GetData().Vertex[0];
			auto v = navi.GetData().Vertex[1];

			Game::AddDrawList(DrawStage::Engine, [&, pos, v]() {

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
				mLineCBuffer.mParam.mWorld.r[1] = v;
				mLineCBuffer.UpdateSubresource(render->m_Context);
				mLineCBuffer.VSSetConstantBuffers(render->m_Context);

				render->m_Context->DrawIndexed(2, 0, 0);

				render->m_Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

				render->PopDS();
			});
		}
	}

}
#endif
void NaviMeshComponent::Update() {

#ifdef _ENGINE_MODE
	EngineUpdate();
#endif

}
void NaviMeshComponent::Finish() {
	mStart = NULL;
	mEnd = NULL;

	mStartTarget = NULL;
	mEndTarget = NULL;
}

#ifdef _ENGINE_MODE
void NaviMeshComponent::CreateInspector() {

	Inspector ins("NaviMesh",this);
	ins.Add("NaviMesh", &m_Mesh, [&]() {
		SetNaviMesh(m_Mesh);
	});
	ins.Add("BaseNaviMeshObject", &mBaseNaviMeshObject, [&]() {
		SetBaseNaviMeshObject(mBaseNaviMeshObject);
	});
	ins.Add("StartPoint", &mStartTarget, [&]() {
		RootCreate(mStartTarget, mEndTarget);
	});
	ins.Add("EndPoint", &mEndTarget, [&]() {
		RootCreate(mStartTarget, mEndTarget);
	});
	ins.Add("DebugRouteView", &m_EngineRouteView, [&](bool f) {
		m_EngineRouteView = f;
	});
	ins.Add("DebugConectView", &m_EngineMeshView, [&](bool f) {
		m_EngineMeshView = f;
	});
	ins.Add("DebugWallView", &m_EngineWallView, [&](bool f) {
		m_EngineWallView = f;
	});

	ins.Complete();


}
#endif

void NaviMeshComponent::IO_Data(I_ioHelper* io) {
#define _KEY(x) io->func( x , #x)
	_KEY(m_Mesh);
	ioGameObjectHelper::func(&mBaseNaviMeshObject, "mBaseNaviMeshObject", io, &gameObject);
	ioGameObjectHelper::func(&mStartTarget, "mStartTarget", io, &gameObject);
	ioGameObjectHelper::func(&mEndTarget, "mEndTarget", io, &gameObject);
#undef _KEY
}

void NaviMeshComponent::SetBaseNaviMeshObject(GameObject obj)
{
	if (obj == gameObject) {
		return;
	}
	mBaseNaviMeshObject = obj;
}

void NaviMeshComponent::SetNaviMesh(MeshAsset mesh)
{
	m_Mesh = mesh;
	if(!m_Mesh.IsLoad())
		AssetLoad::Instance(m_Mesh.m_Hash, m_Mesh);
	if(m_Mesh.IsLoad()){
		mNaviMesh.Create(mesh.m_Ptr);
		RootCreate(mStartTarget, mEndTarget);
		mBaseNaviMeshObject = NULL;
	}
}

void NaviMeshComponent::RootCreate(GameObject start, GameObject end)
{
	mNavigatePtr = NULL;
	mNaviMeshCreatorPtr = NULL;
	mStart = NULL;
	mEnd = NULL;

	mStartTarget = start;
	mEndTarget = end;

	if (!mStartTarget || !mEndTarget)return;

	if (mBaseNaviMeshObject) {
		if (auto com = mBaseNaviMeshObject->GetComponent<NaviMeshComponent>()) {
			mStart = com->mNaviMesh.FindNaviPosition(mStartTarget->mTransform->WorldPosition());
			mEnd = com->mNaviMesh.FindNaviPosition(mEndTarget->mTransform->WorldPosition());


			if (!mStart || !mEnd)return;

			mNaviMeshCreatorPtr = make_shared<NavigateCreator>(mStart, mEnd, &com->mNaviMesh);

			mNaviMeshCreatorPtr->N2();
			mNavigatePtr = mNaviMeshCreatorPtr->Result();
		}
	}
	else {

		mStart = mNaviMesh.FindNaviPosition(mStartTarget->mTransform->WorldPosition());
		mEnd = mNaviMesh.FindNaviPosition(mEndTarget->mTransform->WorldPosition());

		if (!mStart || !mEnd)return;

		mNaviMeshCreatorPtr = make_shared<NavigateCreator>(mStart, mEnd, &mNaviMesh);

		mNaviMeshCreatorPtr->N2();
		mNavigatePtr = mNaviMeshCreatorPtr->Result();
	}
}

void NaviMeshComponent::Move(float speed)
{
	if (mNavigatePtr) {
		return mNavigatePtr->Move(speed);
	}
}

bool NaviMeshComponent::IsMoveEnd()
{
	if (mNavigatePtr) {
		return mNavigatePtr->IsMoveEnd();
	}
	return true;
}

XMVECTOR NaviMeshComponent::GetPosition()
{
	if (mNavigatePtr) {
		return mNavigatePtr->GetPosition();
	}
	return XMVectorZero();
}

XMVECTOR NaviMeshComponent::GetRouteVector()
{
	if (mNavigatePtr) {
		auto navi = mNavigatePtr->GetCurrentRoute();
		if (!navi)return XMVectorZero();
		return XMVector3Normalize(navi->Line);
	}
	return XMVectorZero();
}

float NaviMeshComponent::GetRemainderRouteDistance()
{
	if (mNavigatePtr) {
		return mNavigatePtr->GetEndRouteDistance();
	}
	return 0.0f;
}

float NaviMeshComponent::GetRouteDistance()
{
	if (mNavigatePtr) {
		return mNavigatePtr->GetTotalRouteDistance();
	}
	return 0.0f;
}
