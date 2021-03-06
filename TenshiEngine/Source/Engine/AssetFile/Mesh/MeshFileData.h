#pragma once
#include "../AssetFileData.h"
#include"MeshBufferData.h"

#include "MySTL/Math.h"

class IPolygonsData;

class MeshFileData : public AssetFileData{
public:
	MeshFileData();
	~MeshFileData();

	bool Create(const char* filename) override;

	const MeshBufferData& GetBufferData() const;

	const IPolygonsData* GetPolygonsData() const;
	const BoundingBox& GetBoundingBox() const;
	const BoundingSphere& GetBoundingSphere() const;

private:
	//コピー禁止
	MeshFileData(const MeshFileData&) = delete;
	MeshFileData& operator=(const MeshFileData&) = delete;

	IPolygonsData* m_Polygons;	// 頂点とインデックスデータ
	MeshBufferData m_MeshBufferData;
	BoundingBox m_BoundingBox;
	BoundingSphere m_BoundingSphere;
};
