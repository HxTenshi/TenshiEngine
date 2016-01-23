#pragma once

class IPolygonsData;

class MeshFileData{
public:
	MeshFileData();
	~MeshFileData();

	void Create(const char* filename);

	const IPolygonsData* GetPolygonsData() const;

private:
	//コピー禁止
	MeshFileData(const MeshFileData&) = delete;
	MeshFileData& operator=(const MeshFileData&) = delete;

	IPolygonsData* m_Polygons;	// 頂点とインデックスデータ
};
