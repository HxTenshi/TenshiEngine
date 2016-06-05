
#include "BoneFileData.h"


#include "MySTL/Utility.h"


BoneFileData::BoneFileData(){
}

BoneFileData::~BoneFileData(){
	m_BoneData.mBoneBuffer.clear();
	m_BoneData.mBoneName.clear();
	//m_BoneData.mIK_Links.clear();
}

void BoneFileData::Create(const char* filename){

	m_BoneData.mBoneBuffer.clear();
	m_BoneData.mBoneName.clear();
	//m_BoneData.mIK_Links.clear();

	m_FileName = filename;

	FILE *hFP;
	fopen_s(&hFP, filename, "rb");

	if (hFP == 0)return;

	auto& data = m_BoneData;

	fstd::r_vector(data.mBoneBuffer, hFP);
	//data.mIK_Links.resize(data.mBoneBuffer.size());
	//for (auto& ik : data.mIK_Links){
	//	fstd::r_vector(ik, hFP);
	//}
	//for (unsigned int i = 0; i < data.mIK_Links.size();i++){
	//	std::vector<IkLink>& ik = data.mIK_Links[i];
	//	fstd::r_vector(ik, hFP);
	//}
	data.mBoneName.resize(data.mBoneBuffer.size());
	//for (auto& name : data.mBoneName){
	for (unsigned int i = 0; i < data.mBoneName.size(); i++){
		auto& name = data.mBoneName[i];

		fstd::r(name, hFP);
	}

	fclose(hFP);
}

void BoneFileData::FileUpdate(){
	Create(m_FileName.c_str());
}

const BoneData& BoneFileData::GetBoneData() const{
	return m_BoneData;
}