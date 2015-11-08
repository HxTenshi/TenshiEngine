#pragma once


#include <d3d11.h>
#include <xnamath.h>

#include "Graphic/Model/Model.h"
#include "Graphic/Font/Font.h"
#include <map>
#include "Device/DirectX11Device.h"
#include "Graphic/RenderTarget/RenderTarget.h"
#include "Window/TreeViewWIndow.h"

#include "Input/Input.h"
#include "Component.h"
#include "Actor.h"

#include "Sound/Sound.h"

#include "../PhysX/PhysX3.h"
#include "Engine/ICommand.h"

enum class DrawStage{
	Diffuse,
	Depth,
	Normal,
	Engine,
	UI,
	Count
};
typedef void(*DrawFunc)();


class EditorCamera{
public:
	EditorCamera(){
		mRClickMousePos = XMVectorSet(0.0f,0.0f, 0.0f, 0.0f);
		//mRClickEyeVect = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		//mRClickRotateAxis = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		mCameraComponent = new CameraComponent();
		mCamera.mTransform = shared_ptr<TransformComponent>(new TransformComponent());
		mCamera.AddComponent<TransformComponent>(mCamera.mTransform);
		mCamera.AddComponent(shared_ptr<CameraComponent>(mCameraComponent));
		mCamera.Initialize();
		UINT width = WindowState::mWidth;
		UINT height = WindowState::mHeight;
		//XMMatrixPerspectiveFovLH(XM_PIDIV4, width / (FLOAT)height, 0.01f, 100.0f);
		mCamera.mTransform->Position(XMVectorSet(0, 3, -10,1));
		mCamera.mTransform->Scale(XMVectorSet(1, 1, 1, 1));
	}
	~EditorCamera(){

	}
	void Update(float deltaTime){
		auto pos = mCamera.mTransform->Position();
		if (Input::Down(KeyCoord::Key_A)){
			pos.x -= 0.1f;
		}
		if (Input::Down(KeyCoord::Key_D)){
			pos.x += 0.1f;
		}
		if (Input::Down(KeyCoord::Key_W)){
			pos.z += 0.1f;
		}
		if (Input::Down(KeyCoord::Key_S)){
			pos.z -= 0.1f;
		}
		if (Input::Down(KeyCoord::Key_Q)){
			pos.y += 0.1f;
		}
		if (Input::Down(KeyCoord::Key_E)){
			pos.y -= 0.1f;
		}
		if (Input::Trigger(MouseCoord::Right)){
			int x, y;
			Input::MousePosition(&x, &y);
			mRClickMousePos = XMVectorSet((FLOAT)x, (FLOAT)y, 0.0f, 0.0f);
		
		
			//mRClickEyeVect = mCamera.mTransform->Forward();
			//mRClickRotateAxis = XMVector3Cross(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), -XMVector3Normalize(mRClickEyeVect));

			//mRClickRotate = mCamera.mTransform->Rotate();
			
		}
		if (Input::Down(MouseCoord::Right)){
			int x, y;
			Input::MousePosition(&x, &y);
			XMVECTOR DragVect = XMVectorSet((FLOAT)x, (FLOAT)y, 0.0f, 0.0f) - mRClickMousePos;
		
			DragVect *= 0.01f;
			XMVECTOR xrot = XMQuaternionRotationAxis(mCamera.mTransform->Left(), DragVect.y);
			//Eye = XMVector3Rotate(-mRClickEyeVect, xrot) + At;
		
			XMVECTOR yrot = XMQuaternionRotationAxis(XMVectorSet(0,1,0,1), DragVect.x);
			//XMVECTOR yrot = XMQuaternionRotationAxis(mCamera.mTransform->Up(), DragVect.x);
			//Eye = XMVector3Rotate(-mRClickEyeVect, yrot) + At;
		
			XMVECTOR rot = XMQuaternionMultiply(xrot, yrot);
			XMVECTOR rotate = mCamera.mTransform->Rotate();
			rot = XMQuaternionMultiply(rotate, rot);
			mCamera.mTransform->Rotate(rot);
			mRClickMousePos = XMVectorSet((FLOAT)x, (FLOAT)y, 0.0f, 0.0f);
		
		}
		mCamera.mTransform->Position(pos);
		mCamera.UpdateComponent(deltaTime);
	}

	XMVECTOR ScreenToWorldPoint(const XMVECTOR& point){
		XMVECTOR d;
		XMMATRIX projInv;
		projInv = XMMatrixInverse(&d, mCameraComponent->mProjection);
		XMMATRIX viewInv;
		viewInv = XMMatrixInverse(&d, mCameraComponent->mView);

		XMMATRIX vpInv;
		vpInv = XMMatrixIdentity();
		vpInv._11 = WindowState::mWidth / 2.0f; vpInv._22 = -(WindowState::mHeight / 2.0f);
		vpInv._41 = WindowState::mWidth / 2.0f; vpInv._42 = WindowState::mHeight / 2.0f;

		vpInv = XMMatrixInverse(&d, vpInv);

		//vpInv = XMMatrixMultiply(projInv, viewInv);
		//vpInv = XMMatrixMultiply(projInv, viewInv);
		vpInv = vpInv * projInv * viewInv;
		//vpInv = projInv * viewInv;

		//float nearFar = 100.0f - 0.01f;

		XMVECTOR worldPoint = point;// XMVectorSet(WindowState::mWidth, WindowState::mHeight, nearFar, 1.0f);
		worldPoint.z = 0.9999f;
		worldPoint = XMVector3TransformCoord(worldPoint, vpInv);
		return worldPoint;
	}

	XMVECTOR PointRayVector(const XMVECTOR& point){

		XMVECTOR end = ScreenToWorldPoint(point);
		XMVECTOR vect = end - mCamera.mTransform->Position();
		vect = XMVector3Normalize(vect);
		return vect;
	}

	XMVECTOR GetPosition(){
		return mCamera.mTransform->Position();;
	}
private:
	XMVECTOR	mRClickMousePos;
	//XMVECTOR	mRClickEyeVect;
	//XMVECTOR	mRClickRotateAxis;
	//XMVECTOR	mRClickRotate;
	Actor		mCamera;
	CameraComponent* mCameraComponent;
};


class Box : public Actor{
public:

	Box(){
		Name("new Box");
		mTransform = shared_ptr<TransformComponent>(new TransformComponent());
		mComponents.AddComponent<TransformComponent>(mTransform);
		auto material = shared_ptr<MaterialComponent>(new MaterialComponent());
		mComponents.AddComponent<MaterialComponent>(material);
		mComponents.AddComponent<ModelComponent>(shared_ptr<ModelComponent>(new ModelComponent("box",material)));
		mComponents.AddComponent<MeshDrawComponent>(shared_ptr<MeshDrawComponent>(new MeshDrawComponent()));
		t = 0.0f;

		float scale = 1.0f;
		//auto t = shared_ptr<TransformComponent>(new TransformComponent());
		//mTransform = t;
		//mComponents.AddComponent<TransformComponent>(t);
		mTransform->Scale(XMVectorSet(scale, scale, scale, 1.0f));
	}
	Box(const XMVECTOR& Position){
		Name("new Box");
		mTransform = shared_ptr<TransformComponent>(new TransformComponent());
		mComponents.AddComponent<TransformComponent>(mTransform);
		auto material = shared_ptr<MaterialComponent>(new MaterialComponent());
		mComponents.AddComponent<MaterialComponent>(material);
		mComponents.AddComponent<ModelComponent>(shared_ptr<ModelComponent>(new ModelComponent("box", material)));
		mComponents.AddComponent<MeshDrawComponent>(shared_ptr<MeshDrawComponent>(new MeshDrawComponent()));

		mTransform->Position(Position);
		t = 0.0f;

		float scale = 1.0f;
		mTransform->Scale(XMVectorSet(scale, scale, scale, 1.0f));

	}
	~Box(){
	}

	void Update(float deltaTime)override{

	}
private:
	float t;
};


class Text : public Actor{
public:
	Text(const XMFLOAT2& TopLeft, const XMFLOAT2& DownRight)
	{
		Name("new Text");

		mTransform = shared_ptr<TransformComponent>(new TransformComponent());
		mComponents.AddComponent<TransformComponent>(mTransform);

		auto mMaterialComponent = shared_ptr<MaterialComponent>(new MaterialComponent());
		mComponents.AddComponent<MaterialComponent>(mMaterialComponent);

		mComponents.AddComponent<ModelComponent>(shared_ptr<ModelComponent>(new TextureModelComponent("", mMaterialComponent)));

		mComponents.AddComponent<TextComponent>(shared_ptr<TextComponent>(new TextComponent()));


		auto mPixelTopLeft = TopLeft;
		auto mPixelDownRight = DownRight;
		XMFLOAT2 mTopLeft;
		XMFLOAT2 mDownRight;
		mTopLeft.x = TopLeft.x / WindowState::mWidth;
		mTopLeft.y = 1 - DownRight.y / WindowState::mHeight;

		mDownRight.x = DownRight.x / WindowState::mWidth;
		mDownRight.y = 1 - TopLeft.y / WindowState::mHeight;


		mTransform->Position(XMVectorSet(mTopLeft.x, mTopLeft.y, mDownRight.x, mDownRight.y));
	}
};
class Tex : public Actor{
public:
	Tex(const char* FileName, const XMFLOAT2& TopLeft, const XMFLOAT2& DownRight)
	{
		Name("new Texture");
		Texture mTexture;
		mTexture.Create(FileName);

		mTransform = shared_ptr<TransformComponent>(new TransformComponent());
		mComponents.AddComponent<TransformComponent>(mTransform);
		auto mMaterialComponent = shared_ptr<MaterialComponent>(new MaterialComponent());
		mComponents.AddComponent<MaterialComponent>(mMaterialComponent);

		mComponents.AddComponent<ModelComponent>(shared_ptr<ModelComponent>(new TextureModelComponent("", mMaterialComponent)));
		auto draw = shared_ptr<MeshDrawComponent>(new MeshDrawComponent());
		draw->mWriteDepth = false;
		mComponents.AddComponent<MeshDrawComponent>(draw);


		Material mTexMaterial;
		auto cbm = ConstantBuffer<cbChangesMaterial>::create(4);
		auto cbt = ConstantBuffer<cbChangesUseTexture>::create(6);
		mTexMaterial.Create(cbm, cbt);
		mTexMaterial.SetTexture(mTexture);

		//if (mModel_.mMaterials.size() > 0){
		//	mModel_.mMaterials[0] = *mTexMaterial;
		//}

		mMaterialComponent->SetMaterial(0,mTexMaterial);

		mPixelTopLeft = TopLeft;
		mPixelDownRight = DownRight;

		mTopLeft.x = TopLeft.x / WindowState::mWidth;
		mTopLeft.y = 1 - DownRight.y / WindowState::mHeight;

		mDownRight.x = DownRight.x / WindowState::mWidth;
		mDownRight.y = 1 - TopLeft.y / WindowState::mHeight;

		mTransform->Position(XMVectorSet(mTopLeft.x, mTopLeft.y, mDownRight.x, mDownRight.y));

	}
	Tex(Texture& Tex, const XMFLOAT2& TopLeft ,const XMFLOAT2& DownRight)
	{
		Name("new Texture");


		mTransform = shared_ptr<TransformComponent>(new TransformComponent());
		mComponents.AddComponent<TransformComponent>(mTransform);
		auto mMaterialComponent = shared_ptr<MaterialComponent>(new MaterialComponent());
		mComponents.AddComponent<MaterialComponent>(mMaterialComponent);

		mComponents.AddComponent<ModelComponent>(shared_ptr<ModelComponent>(new TextureModelComponent("", mMaterialComponent)));
		auto draw = shared_ptr<MeshDrawComponent>(new MeshDrawComponent());
		draw->mWriteDepth = false;
		mComponents.AddComponent<MeshDrawComponent>(draw);


		Material mTexMaterial;
		mTexMaterial.Create("texture.fx");
		mTexMaterial.SetTexture(Tex);

		//if (mModel_.mMaterials.size() > 0){
		//	mModel_.mMaterials[0] = *mTexMaterial;
		//}

		mMaterialComponent->SetMaterial(0,mTexMaterial);

		mPixelTopLeft = TopLeft;
		mPixelDownRight = DownRight;

		mTopLeft.x = TopLeft.x / WindowState::mWidth;
		mTopLeft.y = 1-DownRight.y / WindowState::mHeight;

		mDownRight.x = DownRight.x / WindowState::mWidth;
		mDownRight.y = 1-TopLeft.y / WindowState::mHeight;


		mTransform->Position(XMVectorSet(mTopLeft.x, mTopLeft.y, mDownRight.x, mDownRight.y));
	}
	~Tex(){
	}
protected:

	XMFLOAT2 mTopLeft;
	XMFLOAT2 mDownRight;

	XMFLOAT2 mPixelTopLeft;
	XMFLOAT2 mPixelDownRight;
};

class Player : public Actor{
public:
	Player(){
		Name("new Player");

		mTransform = shared_ptr<TransformComponent>(new TransformComponent());
		mComponents.AddComponent<TransformComponent>(mTransform);
		auto material = shared_ptr<MaterialComponent>(new MaterialComponent());
		mComponents.AddComponent<MaterialComponent>(material);
		mComponents.AddComponent<ModelComponent>(shared_ptr<ModelComponent>(new ModelComponent("model/mikoteto1/model.pmx", material)));
		mComponents.AddComponent<MeshDrawComponent>(shared_ptr<MeshDrawComponent>(new MeshDrawComponent()));
		mComponents.AddComponent<AnimetionComponent>(shared_ptr<AnimetionComponent>(new AnimetionComponent()));
		mComponents.AddComponent<ScriptComponent>(shared_ptr<ScriptComponent>(new ScriptComponent()));

		float scale = 0.1f;
		mTransform->Scale(XMVectorSet(scale, scale, scale, 1.0f));

	}
	~Player(){
	}

private:
};

#include "Window/TreeViewWindow.h"
class SelectActor{
public:
	SelectActor()
		:mSelect(NULL)
	{
		mDragBox = -1;
		mVectorBox[0].mTransform->Scale(XMVectorSet(0.5f, 0.5f, 0.5f, 1.0f));
		mVectorBox[1].mTransform->Scale(XMVectorSet(0.5f, 0.5f, 0.5f, 1.0f));
		mVectorBox[2].mTransform->Scale(XMVectorSet(0.5f, 0.5f, 0.5f, 1.0f));

		mSelectWireMaterial.mDiffuse.x = 0.95f;
		mSelectWireMaterial.mDiffuse.y = 0.6f;
		mSelectWireMaterial.mDiffuse.z = 0.1f;
		mSelectWireMaterial.Create();
	}

	void SetSelect(Actor* select){
		mDragBox = -1;

		mSelect = select;
		Window::mInspectorWindow.InsertConponentData(mSelect);
	}
	Actor* GetSelect(){
		return mSelect;
	}

	void UpdateInspector(){
		if (!mSelect)return;

		static unsigned long time_start = timeGetTime();
		unsigned long current_time = timeGetTime();
		unsigned long b = (current_time - time_start);
		if (b >= 16){
			time_start = timeGetTime();
			//ˆ—‚ª’Ç‚¢‚Â‚©‚È‚¢ê‡‚ª‚ ‚é
			Window::UpdateInspector();
		}
	}
	void Update(float deltaTime){
		//if (Window::GetTreeViewWindow()->IsActive()){
		//	void* s = (Actor*)Window::GetTreeViewWindow()->GetSelectItem();
		//	
		//	if (s){
		//		if (s != mTreeSelect){
		//			mTreeSelect = (Actor*)s;
		//			mSelect = mTreeSelect;
		//		}
		//	}
		//}

		if (!mSelect)return;

		mVectorBox[0].mTransform->Position(mSelect->mTransform->WorldPosition() + XMVectorSet(3.0f, 0.0f, 0.0f, 0.0f));
		mVectorBox[1].mTransform->Position(mSelect->mTransform->WorldPosition() + XMVectorSet(0.0f, 3.0f, 0.0f, 0.0f));
		mVectorBox[2].mTransform->Position(mSelect->mTransform->WorldPosition() + XMVectorSet(0.0f, 0.0f, 3.0f, 0.0f));

		mVectorBox[0].UpdateComponent(deltaTime);
		mVectorBox[1].UpdateComponent(deltaTime);
		mVectorBox[2].UpdateComponent(deltaTime);

		static XMVECTOR vect = XMVectorZero();
		if (Input::Down(MouseCoord::Left)){
			int x, y;
			Input::MouseLeftDragVector(&x, &y);
			float p = -y*0.05f;
			vect = mDragPos;

			if (mDragBox == 0){
				vect += XMVectorSet(p, 0.0f, 0.0f, 0.0f);
			}
			if (mDragBox == 1){
				vect += XMVectorSet(0.0f, p, 0.0f, 0.0f);
			}
			if (mDragBox == 2){
				vect += XMVectorSet(0.0f, 0.0f, p, 0.0f);
			}
		}

		if (Input::Up(MouseCoord::Left)){
			if (mDragBox != -1){
				mSelect->mTransform->Position(vect);
				mSelect->mTransform->SetUndo(vect);
			}
			mDragBox = -1;
		}
		if (mDragBox != -1){
			mSelect->mTransform->Position(vect);
		}
		SelectActorDraw();
	}

	void SelectActorDraw();

	void ChackHitRay(const XMVECTOR& pos, const XMVECTOR& vect){

		if (!mSelect)return;

		if (mVectorBox[0].ChackHitRay(pos, vect)){
			mDragPos = mSelect->mTransform->Position();
			mDragBox = 0;
		}
		if (mVectorBox[1].ChackHitRay(pos, vect)){
			mDragPos = mSelect->mTransform->Position();
			mDragBox = 1;
		}
		if (mVectorBox[2].ChackHitRay(pos, vect)){
			mDragPos = mSelect->mTransform->Position();
			mDragBox = 2;
		}
	}
private:

	Actor* mSelect;

	Box mVectorBox[3];
	int mDragBox;
	XMVECTOR mDragPos;

	Material mSelectWireMaterial;
};

class WorldGrid{
public:

	const int XlineNum = 100;
	const int ZlineNum = 100;

	WorldGrid(float GridSize){

		mShader.Create("Line.fx");

		const int _lineNum = 100+100+6;
		XMFLOAT4 v[_lineNum];
		WORD idx[_lineNum];

		for (int i = 0; i < XlineNum; i += 2){
			float x = (float)(i - XlineNum / 2)*GridSize;

			float z = GridSize*ZlineNum / 2;
			v[i] = { x, 0.0f, -z, 0.0f };
			v[i + 1] = { x, 0.0f, z, 0.0f };
			if (x == 0)v[i + 1] = { x, 0.0f, 0.0f, 0.0f };

			idx[i] = i;
			idx[i + 1] = i + 1;
		}

		for (int i = 0; i < ZlineNum; i += 2){
			float z = (float)(i - ZlineNum / 2)*GridSize;
			float x = GridSize*XlineNum / 2;
			v[XlineNum + i] = { -x, 0.0f, z, 0.0f };
			v[XlineNum + i + 1] = { x, 0.0f, z, 0.0f };
			if (z == 0)v[XlineNum + i + 1] = { 0.0f, 0.0f, z, 0.0f };

			idx[XlineNum + i] = XlineNum + i;
			idx[XlineNum + i + 1] = XlineNum + i + 1;
		}
		float z = GridSize*ZlineNum / 2;
		float x = GridSize*XlineNum / 2;
		int i = XlineNum + ZlineNum;
		v[i] = { 0, 0.0f, 0.0f, 0.0f };
		v[i + 1] = { x, 0.0f, 0.0f, 0.0f };
		idx[i] = i;
		idx[i + 1] = i + 1;

		i += 2;
		v[i] = { 0.0f, 0.0f, 0, 0.0f };
		v[i + 1] = { 0.0f, 0.0f, z, 0.0f };
		idx[i] = i;
		idx[i + 1] = i + 1;

		i += 2;
		v[i] = { 0.0f, 0, 0.0f, 0.0f };
		v[i + 1] = { 0.0f, z, 0.0f, 0.0f };
		idx[i] = i;
		idx[i + 1] = i + 1;

		createVertex(v, sizeof(XMFLOAT4), _lineNum);
		createIndex(idx, _lineNum);

		Material _Material;
		auto cbm = ConstantBuffer<cbChangesMaterial>::create(4);
		auto cbt = ConstantBuffer<cbChangesUseTexture>::create(6);
		_Material.mDiffuse = XMFLOAT4{0.2f,0.2f,0.2f,1};
		_Material.Create(cbm, cbt);

		mMaterial.SetMaterial(0,_Material);

		Material Materials[3];
		int xyz = 0;
		Materials[xyz].mDiffuse = XMFLOAT4{ 0.4f, 0.0f, 0.0f, 1 };
		Materials[xyz].Create(cbm, cbt);
		mMaterialXYZ[xyz].SetMaterial(0,Materials[xyz]);
		xyz = 1;
		Materials[xyz].mDiffuse = XMFLOAT4{ 0.0f, 0.0f, 0.4f, 1 };
		Materials[xyz].Create(cbm, cbt);
		mMaterialXYZ[xyz].SetMaterial(0,Materials[xyz]);
		xyz = 2;
		Materials[xyz].mDiffuse = XMFLOAT4{ 0.0f, 0.4f, 0.0f, 1 };
		Materials[xyz].Create(cbm, cbt);
		mMaterialXYZ[xyz].SetMaterial(0,Materials[xyz]);

		mLineCBuffer = ConstantBuffer<CBChangesEveryFrame>::create(2);
		mLineCBuffer.mParam.mWorld = XMMatrixIdentity();
		mLineCBuffer.UpdateSubresource();

	}
	~WorldGrid(){
		mpVertexBuffer->Release();
		mpIndexBuffer->Release();

	}

	void Draw()const{
		Device::mpImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
		mShader.SetShader();
		mMaterial.GetMaterial(0).PSSetShaderResources();
		UINT mStride = sizeof(XMFLOAT4);
		UINT offset = 0;
		Device::mpImmediateContext->IASetVertexBuffers(0, 1, &mpVertexBuffer, &mStride, &offset);
		Device::mpImmediateContext->IASetIndexBuffer(mpIndexBuffer, DXGI_FORMAT_R16_UINT, 0);


		mLineCBuffer.VSSetConstantBuffers();
		mLineCBuffer.PSSetConstantBuffers();
		Device::mpImmediateContext->DrawIndexed(XlineNum, 0, 0);

		Device::mpImmediateContext->DrawIndexed(ZlineNum, XlineNum, 0);

		mMaterialXYZ[0].GetMaterial(0).PSSetShaderResources();
		Device::mpImmediateContext->DrawIndexed(2, XlineNum + ZlineNum, 0);
		mMaterialXYZ[1].GetMaterial(0).PSSetShaderResources();
		Device::mpImmediateContext->DrawIndexed(2, XlineNum + ZlineNum+2, 0);
		mMaterialXYZ[2].GetMaterial(0).PSSetShaderResources();
		Device::mpImmediateContext->DrawIndexed(2, XlineNum + ZlineNum+4, 0);
		
		Device::mpImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}

	HRESULT createVertex(void* Vertices, UINT TypeSize, UINT VertexNum){
		HRESULT hr = S_OK;

		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = TypeSize * VertexNum;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;
		D3D11_SUBRESOURCE_DATA InitData;
		ZeroMemory(&InitData, sizeof(InitData));
		InitData.pSysMem = Vertices;
		hr = Device::mpd3dDevice->CreateBuffer(&bd, &InitData, &mpVertexBuffer);
		if (FAILED(hr))
			return hr;

		return S_OK;
	}

	HRESULT createIndex(WORD* Indices, UINT IndexNum){
		HRESULT hr = S_OK;

		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(WORD) * IndexNum;
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;
		D3D11_SUBRESOURCE_DATA InitData;
		ZeroMemory(&InitData, sizeof(InitData));
		InitData.pSysMem = Indices;
		hr = Device::mpd3dDevice->CreateBuffer(&bd, &InitData, &mpIndexBuffer);
		if (FAILED(hr))
			return hr;

		return S_OK;
	}

private:
	Shader mShader;

	ID3D11Buffer*	mpVertexBuffer;
	ID3D11Buffer*	mpIndexBuffer;

	MaterialComponent mMaterial;
	MaterialComponent mMaterialXYZ[3];
	ConstantBuffer<CBChangesEveryFrame> mLineCBuffer;
};

class Particle : public Actor{
public:

	Particle(){
		Name("new Particle");

		mTransform = shared_ptr<TransformComponent>(new TransformComponent());
		mComponents.AddComponent<TransformComponent>(mTransform);
		Material mTexMaterial;
		auto cbm = ConstantBuffer<cbChangesMaterial>::create(4);
		auto cbt = ConstantBuffer<cbChangesUseTexture>::create(6);
		mTexMaterial.Create(cbm, cbt);
		mTexMaterial.SetTexture("texture.png");
		mMaterial.SetMaterial(0,mTexMaterial);


		auto materialComponent = shared_ptr<MaterialComponent>(new MaterialComponent());
		mComponents.AddComponent<MaterialComponent>(materialComponent);
		materialComponent->SetMaterial(0,mTexMaterial);

		mComponents.AddComponent<ParticleComponent>(shared_ptr<ParticleComponent>(new ParticleComponent()));


	}
	~Particle(){
	}

private:


	MaterialComponent mMaterial;
};


class Game{
public:
	static void AddResource(const std::string& filename){
		auto pos = filename.find_last_of(".");
		auto type = filename.substr(pos);

		if (type == ".png"
			|| type == ".bmp"
			|| type == ".jpg"
			|| type == ".jpge"){
			AddObject(new Tex(filename.c_str(), { 1000, 0 }, { 1200, 200 }));
			return;
		}
		if (type == ".pmd"
			|| type == ".pmx"){
			auto act = new Actor();
			auto material = shared_ptr<MaterialComponent>(new MaterialComponent());
			act->AddComponent(material);
			act->AddComponent(shared_ptr<ModelComponent>(new ModelComponent(filename.c_str(),material)));
			act->AddComponent(shared_ptr<MeshDrawComponent>(new MeshDrawComponent()));
			act->AddComponent(shared_ptr<AnimetionComponent>(new AnimetionComponent()));
			AddObject(act);
			return;
		}

		MessageBox(Window::mhWnd, "ƒtƒ@ƒCƒ‹‚ðŠJ‚¯‚Ü‚¹‚ñ‚Å‚µ‚½", "Ž¸”s", MB_OK);
	}

	Game();
	~Game();

	static void AddObject(Actor* actor);
	static void DestroyObject(Actor* actor);
	static PxRigidActor* CreateRigitBody();
	static PhysX3Main* GetPhysX();
	static void RemovePhysXActor(PxActor* act);
	static Actor* FindUID(UINT uid);
	static void AddDrawList(DrawStage stage, std::function<void()> func);
	static void SetUndo(ICommand* command);
	static void SetMainCamera(CameraComponent* Camera);
	void ChangePlayGame(bool isPlay);
	void SaveScene();


	void fps(){
		static unsigned long time_start = timeGetTime();
		static int count_frames = 0;

		unsigned long current_time = timeGetTime();
		float b = (current_time - time_start);
		if (b == 0.0f){
			b = 1;
		}
		float fps = (float)count_frames / b * 1000;
		std::string title = "FPS:" + std::to_string(fps) + "‚¾‚æ`š";
		count_frames++;
		if (count_frames > 60){
			time_start = timeGetTime();
			count_frames = 1;
		}

		static Text text({ 0, 0 }, { 500, 800 });
		static bool init = false;
		if (!init){
			text.Initialize();
			init = true;
		}
		auto t = text.GetComponent<TextComponent>();
		t->ChangeText(title);
		text.UpdateComponent(1);
	}

	float GetDeltaTime(){
		float deltaTime;
		if (Device::mDriverType == D3D_DRIVER_TYPE_REFERENCE)
		{
			deltaTime = (float)XM_PI * 0.0125f;
		}
		else
		{
			static float t = 0.0f;
			static float bt = 0.0f;
			static DWORD dwTimeStart = 0;
			DWORD dwTimeCur = GetTickCount();
			if (dwTimeStart == 0)
				dwTimeStart = dwTimeCur;
			bt = t;
			t = (dwTimeCur - dwTimeStart) / 1000.0f;
			deltaTime = t - bt;
		}
		return deltaTime;
	}

	void Update(){
		if (mIsPlay){
			GamePlay();
		}
		else{
			GameStop();
		}
		mSelectActor.UpdateInspector();
	}
	void GameStop(){
		float deltaTime = GetDeltaTime();

		mCamera.Update(deltaTime);

		//if (Input::Trigger(KeyCoord::Key_C)){
		//	int x, y;
		//	Input::MousePosition(&x, &y);
		//	XMVECTOR point = XMVectorSet((FLOAT)x, (FLOAT)y, 0.0f, 1.0f);
		//	point = mCamera.ScreenToWorldPoint(point);
		//
		//	AddObject(new Box(point));
		//}
		//if (Input::Trigger(KeyCoord::Key_X)){
		//	int x, y;
		//	Input::MousePosition(&x, &y);
		//	XMVECTOR point = XMVectorSet((FLOAT)x, (FLOAT)y, 0.0f, 1.0f);
		//	point = mCamera.ScreenToWorldPoint(point);
		//	auto act = (new Box(point));
		//	AddObject(act);
		//	auto par = mSelectActor.GetSelect();
		//	if (par)act->mTransform->SetParent(par);
		//}

		if (Input::Down(KeyCoord::Key_LCONTROL) && Input::Trigger(KeyCoord::Key_Z)){
			mCommandManager.Undo();
		}

		if (Input::Down(KeyCoord::Key_LCONTROL) && Input::Trigger(KeyCoord::Key_Y)){
			mCommandManager.Redo();
		}


		mSelectActor.Update(deltaTime);

		if (Input::Trigger(MouseCoord::Left)){
			int x, y;
			Input::MousePosition(&x, &y);
			XMVECTOR point = XMVectorSet((FLOAT)x, (FLOAT)y, 0.0f, 1.0f);
			XMVECTOR vect = mCamera.PointRayVector(point);
			XMVECTOR pos = mCamera.GetPosition();
		
			mSelectActor.ChackHitRay(pos, vect);
			for (auto& p : mList){
				if (p.second->ChackHitRay(pos, vect)){
					mSelectActor.SetSelect(p.second);
				}
			}
		}

		mRootObject->DrawOnlyUpdateComponent(deltaTime);

		Game::AddDrawList(DrawStage::Engine, [&](){
			mWorldGrid.Draw();
		});


		fps();
	}
	void GamePlay(){
		float deltaTime = GetDeltaTime();

		mRootObject->UpdateComponent(deltaTime);
		mPhysX3Main->Display();

		fps();
	}

	void Draw(){
		if (!mMainCamera){
			Device::mRenderTargetBack->ClearView();
			mMeinViewRenderTarget.ClearView();
			ClearDrawList();
			return;
		}
		mMainCamera->VSSetConstantBuffers();
		mMainCamera->GSSetConstantBuffers();

		Device::mRenderTargetBack->ClearView();
		mMeinViewRenderTarget.ClearView();


		D3D11_DEPTH_STENCIL_DESC descDS = CD3D11_DEPTH_STENCIL_DESC(CD3D11_DEFAULT());
		descDS.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		descDS.DepthFunc = D3D11_COMPARISON_LESS;
		ID3D11DepthStencilState* pDS = NULL;
		Device::mpd3dDevice->CreateDepthStencilState(&descDS, &pDS);
		Device::mpImmediateContext->OMSetDepthStencilState(pDS, 0);


		const RenderTarget* r[1] = { &mMeinViewRenderTarget };
		RenderTarget::SetRendererTarget((UINT)1, r[0], Device::mRenderTargetBack);


		PlayDrawList(DrawStage::Depth);

		Device::mRenderTargetBack->ClearView();
		Device::mRenderTargetBack->SetRendererTarget();


		ID3D11ShaderResourceView *const pNULL[4] = { NULL, NULL, NULL, NULL };
		Device::mpImmediateContext->PSSetShaderResources(0, 4, pNULL);
		ID3D11SamplerState *const pSNULL[4] = { NULL, NULL, NULL, NULL };
		Device::mpImmediateContext->PSSetSamplers(0, 4, pSNULL);

		//if(false){
		//	D3D11_DEPTH_STENCIL_DESC descDS = CD3D11_DEPTH_STENCIL_DESC(CD3D11_DEFAULT());
		//	descDS.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		//	descDS.DepthFunc = D3D11_COMPARISON_LESS;
		//
		//	ID3D11DepthStencilState* pDS = NULL;
		//	Device::mpd3dDevice->CreateDepthStencilState(&descDS, &pDS);
		//
		//	Device::mpImmediateContext->OMSetDepthStencilState(pDS, 0);
		//
		//	//tex->Draw();
		//
		//Font::TextOutFont(0, 60, 60, "AAAAA", "Comic Sans MS", 0);
		//
		//Font::DrawEnd();
		//
		//	//Device::mpImmediateContext->PSSetShaderResources(0, 4, pNULL);
		//	//Device::mpImmediateContext->PSSetSamplers(0, 4, pSNULL);
		//
		//
		//	const RenderTarget* r2[2] = { &mRenderTargetBack, &mRenderTarget2 };
		//	RenderTarget::SetRendererTarget((UINT)2, r2[0], &mRenderTargetBack);
		//
		//	Device::mpImmediateContext->OMSetDepthStencilState(NULL, 0);
		//	pDS->Release();
		//}

		//for (Actor* p : mActors){
		//	p->Draw(DrawBit::Diffuse);
		//}
		//mActor->Draw(DrawBit::Diffuse);


		PlayDrawList(DrawStage::Diffuse);
		PlayDrawList(DrawStage::Engine);
		PlayDrawList(DrawStage::UI);

		ClearDrawList();

		Device::mpImmediateContext->OMSetDepthStencilState(NULL, 0);
		if (pDS)pDS->Release();

		SetMainCamera(NULL);

	}
	void ClearDrawList(){
		for (auto &list : mDrawList){
			list.second.clear();
		}
	}
	void PlayDrawList(DrawStage Stage){
		auto &list = mDrawList[Stage];
		for (auto &p : list){
			p();
		}
	}
private:
	std::map<UINT, Actor*> mGamePlayList;
	std::map<UINT, Actor*> mList;
	std::map<UINT, Actor*> mListBack;
	std::map<DrawStage, std::list<std::function<void()>>> mDrawList;
	static Actor* mRootObject;

	SelectActor mSelectActor;

	EditorCamera mCamera;

	WorldGrid mWorldGrid;

	SoundPlayer mSoundPlayer;
	Particle mParticle;


	PhysX3Main* mPhysX3Main;

	CommandManager mCommandManager;

	RenderTarget mMeinViewRenderTarget;
	CameraComponent* mMainCamera;
	bool mIsPlay;
};