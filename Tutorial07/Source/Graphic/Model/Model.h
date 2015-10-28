#pragma once

#include "ConstantBuffer/ConstantBuffer.h"
#include "Graphic/Shader/VertexShader.h"
#include "Graphic/Shader/PixelShader.h"

#include "ConstantBuffer/ConstantBufferDif.h"

#include "../Loader/Animation/VMD.h"
#include <vector>

class ModelBuffer;
class ModelBufferPMD;
class ModelBufferPMX;
class ModelBufferTexture;

class Mesh{
public:
	void Draw()const;
	ModelBuffer* mpModelBuffer;
	UINT mFace_vert_count;//	面頂点リストのデータ数
	UINT mFace_vert_start_count;//	面頂点リストのデータ開始位置
};
#include "../Material/Material.h"

class MaterialComponent;
class Model{
public:
	Model();
	virtual ~Model();
	virtual HRESULT Create(const char* FileName, shared_ptr<MaterialComponent> resultMaterial);
	void Release();
	void SetConstantBuffer() const;
	void Update();
	void IASet() const;
	void Draw(shared_ptr<MaterialComponent> material) const;
	ModelBuffer* GetModelBuffer(){ return mModelBuffer; }
	void PlayVMD(float time);

	bool CheckHitPoint(const XMVECTOR& point);

public:
	XMMATRIX		mWorld;
	//std::vector<Material> mMaterials;
protected:

	ModelBuffer* mModelBuffer;

	std::vector<Mesh> mMeshs;

	ConstantBuffer<CBChangesEveryFrame> mCBuffer;
	ConstantBufferArray<cbBoneMatrix> mCBBoneMatrix;

	vmd mVMD;

	friend ModelBuffer;
	friend ModelBufferPMD;
	friend ModelBufferPMX;
	friend ModelBufferTexture;
};

class ModelTexture : public Model{
public:
	ModelTexture();
	~ModelTexture();
	HRESULT Create(const char* FileName, shared_ptr<MaterialComponent> resultMaterial) override;
};