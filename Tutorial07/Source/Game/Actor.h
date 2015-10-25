#pragma once

#include <d3d11.h>
#include <xnamath.h>
#include "ComponentList.h"

class ITransformComponent;
class File;
//PhysXテスト用
namespace physx{
	class PxTransform;
}

enum class DrawBit : unsigned char{
	Diffuse = 0x0001,
	Depth = 0x0002,
	Normal = 0x0004,
};

unsigned char operator & (const unsigned char& bit, const DrawBit& bit2);
unsigned char operator | (const DrawBit& bit, const DrawBit& bit2);
unsigned char operator | (const unsigned char& bit, const DrawBit& bit2);

class Actor{
public:
	Actor();
	virtual ~Actor();
	virtual void UpdateComponent(float deltaTime);
	virtual void Update(float deltaTime);
	virtual void Draw(DrawBit drawbit)const;

	virtual bool ChackHitRay(const XMVECTOR& pos, const XMVECTOR& vect);
	template<class T>
	shared_ptr<T> GetComponent(){
		return mComponents.GetComponent<T>();
	}

	template<class T>
	void AddComponent(shared_ptr<T> component){
		mComponents.AddComponent<T>(component);
	}
	template<class T>
	void RemoveComponent(){
		mComponents.RemoveComponent<T>();
	}

	void CreateInspector();

	std::string Name(){return mName;}
	void Name(std::string name){mName = name;}

	UINT GetUniqueID(){
		return mUniqueID;
	}

	void ExportData(const std::string& pass);
	void ImportData(const std::string& fileName);

	shared_ptr<ITransformComponent> mTransform;


	//PhysXテスト用
	void SetTransform(physx::PxTransform* t);

	void* mTreeViewPtr;

protected:
	ComponentList mComponents;

	unsigned char mDrawBit;

private:
	std::string mName;

	UINT mUniqueID;
};