
#include "MeshFileData.h"

#include "Engine/AssetLoader.h"

#include "MySTL/Utility.h"


MeshFileData::MeshFileData(){
	m_Polygons = NULL;
}

MeshFileData::~MeshFileData(){
	if (m_Polygons){
		delete m_Polygons;
		m_Polygons = NULL;
	}
}

void MeshFileData::Create(const char* filename){

	this->~MeshFileData();

	FILE *hFP;
	fopen_s(&hFP, filename, "rb");

	if (hFP == 0)return;

	std::string str = filename;
	auto type = behind_than_find_last_of(str, ".");

	IPolygonsData* _data = NULL;
	if (type == "tesmesh"){
		auto data = new PolygonsData<SimpleVertex, unsigned short, int>();
		fstd::r_vector(data->Vertexs, hFP);
		fstd::r_vector(data->Indices, hFP);
		fstd::r_vector(data->Meshs, hFP);
		_data = data;
	}else if (type == "tedmesh"){
		auto data = new PolygonsData<SimpleBoneVertex, unsigned short, int>();
		fstd::r_vector(data->Vertexs, hFP);
		fstd::r_vector(data->Indices, hFP);
		fstd::r_vector(data->Meshs, hFP);
		_data = data;
	}



	m_Polygons = _data;

	fclose(hFP);
}

const IPolygonsData* MeshFileData::GetPolygonsData() const{
	return m_Polygons;
}