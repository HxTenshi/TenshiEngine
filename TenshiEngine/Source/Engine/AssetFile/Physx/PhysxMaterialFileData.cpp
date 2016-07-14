#include "PhysxMaterialFileData.h"
#include "Physx/Physx3.h"
#include "Game/Game.h"

PhysxMaterialFileData::PhysxMaterialFileData(){
	m_Param_sf=NULL;
	m_Param_df=NULL;
	m_Param_r = NULL;
	m_Material = NULL;
}
PhysxMaterialFileData::~PhysxMaterialFileData(){
	if (Game::GetPhysX() && Game::GetPhysX()->GetSDK() && m_Material){
		m_Material->release();
		m_Material = NULL;
	}
	m_FileName = "";
}

bool PhysxMaterialFileData::Create(const char* filename){


	if (m_Material){
		m_Material->release();
		m_Material = NULL;
	}
	File f(filename);
	if (!f)return false;

	float staticfric = 0, dinamicfric = 0, restiut = 0;
	f.In(&staticfric);
	f.In(&dinamicfric);
	f.In(&restiut);
	
	m_Material = Game::GetPhysX()->GetSDK()->createMaterial(staticfric,dinamicfric,restiut);
	m_FileName = filename;

	return true;
}

bool PhysxMaterialFileData::FileUpdate(){

	if (m_Material){
		m_Material->release();
		m_Material = NULL;
	}

	File f(m_FileName);
	if (!f)return false;
	float staticfric = 0, dinamicfric = 0, restiut = 0;
	f.In(&staticfric);
	f.In(&dinamicfric);
	f.In(&restiut);

	m_Material = Game::GetPhysX()->GetSDK()->createMaterial(staticfric, dinamicfric, restiut);

	return true;
}
void PhysxMaterialFileData::SaveFile(){

	File f(m_FileName);
	if (!f){
		f.FileCreate();
	}
	if (!m_Material){
		return;
	}
	float staticfric = 0, dinamicfric = 0, restiut = 0;
	staticfric = m_Material->getStaticFriction();
	dinamicfric = m_Material->getDynamicFriction();
	restiut = m_Material->getRestitution();
	f.Out(staticfric);
	f.Out(dinamicfric);
	f.Out(restiut);
}

physx::PxMaterial* PhysxMaterialFileData::GetMaterial() const{
	return m_Material;
}