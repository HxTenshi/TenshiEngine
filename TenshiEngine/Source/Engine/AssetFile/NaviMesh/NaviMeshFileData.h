#pragma once

#include <vector>

#include "../../AssetDataBase.h"

#include "../Mesh/MeshFileData.h"
#include "Engine/AssetLoader.h"


struct NaviMeshPolygonData{
	XMVECTOR Vertex[3];
	unsigned int Index[3];
	XMVECTOR CenterPosition;
	XMVECTOR Normal;
	int NaviMeshID;
};

class NaviMeshPolygon{
public:
	NaviMeshPolygon(){
		mLinkPolygon[0] = NULL;
		mLinkPolygon[1] = NULL;
		mLinkPolygon[2] = NULL;
	}


	NaviMeshPolygon(const NaviMeshPolygonData& Data){
		mNaviMeshPolygonData = Data;

		mLinkPolygon[0] = NULL;
		mLinkPolygon[1] = NULL;
		mLinkPolygon[2] = NULL;

	}

	void SetLink(int id, NaviMeshPolygon* ptr){
		mLinkPolygon[id] = ptr;
		mLinkPolygonLength[id] = XMVector3Length(ptr->GetData().CenterPosition - mNaviMeshPolygonData.CenterPosition).x;
	}
	void GetLine(int id, unsigned int* idx1, unsigned int* idx2){
		int id1 = id;
		int id2 = (id + 1)%3;
		*idx1 = mNaviMeshPolygonData.Index[id1];
		*idx2 = mNaviMeshPolygonData.Index[id2];
	}

	const NaviMeshPolygonData& GetData() const{
		return mNaviMeshPolygonData;
	}
	const NaviMeshPolygon* const* GetLink() const{
		return mLinkPolygon;
	}

	float GetLinkLength(int id) const{
		return mLinkPolygonLength[id];
	}


private:
	NaviMeshPolygonData mNaviMeshPolygonData;

	NaviMeshPolygon *mLinkPolygon[3];
	float mLinkPolygonLength[3];

};


class NaviMesh{
public:

	XMVECTOR toVector(const XMFLOAT3& f3){
		return XMVectorSet(f3.x, f3.y, f3.z, 1.0f);
	}

	void Create(const std::string& MeshFile){
		MeshAssetDataPtr mMeshAsset;
		AssetDataBase::Instance(MeshFile.c_str(), mMeshAsset);

		if (!mMeshAsset.Get())return;

		auto data = mMeshAsset->GetFileData()->GetPolygonsData();
		auto idxnum = data->GetIndexNum();
		auto polynum = idxnum / 3;

		mNaviMeshDataBase.resize(polynum);

		for (int i = 0; i < polynum; i ++){
			NaviMeshPolygonData polydata;

			int id = i * 3;

			polydata.Index[0] = data->GetIndex(id + 0);
			polydata.Index[1] = data->GetIndex(id + 1);
			polydata.Index[2] = data->GetIndex(id + 2);

			polydata.Vertex[0] = toVector(data->GetVertexPos(polydata.Index[0]));
			polydata.Vertex[1] = toVector(data->GetVertexPos(polydata.Index[1]));
			polydata.Vertex[2] = toVector(data->GetVertexPos(polydata.Index[2]));

			polydata.CenterPosition = (polydata.Vertex[0] + polydata.Vertex[1] + polydata.Vertex[2]) / 3.0f;
			
			auto v1 = XMVector3Normalize(polydata.Vertex[1] - polydata.Vertex[0]);
			auto v2 = XMVector3Normalize(polydata.Vertex[1] - polydata.Vertex[0]);
			polydata.Normal = XMVector3Normalize(XMVector3Cross(v1, v2));

			polydata.NaviMeshID = i;

			mNaviMeshDataBase[i] = NaviMeshPolygon(polydata);
		}

		for (int i = 0; i < polynum; i++){

			for (int lineID = 0; lineID < 3; lineID++){

				unsigned int line[2];
				auto base = &mNaviMeshDataBase[i];
				base->GetLine(lineID, &line[0], &line[1]);
				bool linkflag = false;

				for (int j = i + 1; j < polynum; j++){

					for (int lineTarID = 0; lineTarID < 3; lineTarID++){
						unsigned int linetarget[2];
						auto target = &mNaviMeshDataBase[j];
						target->GetLine(lineTarID, &linetarget[0], &linetarget[1]);

						//ラインが一致
						if ((line[0] == linetarget[0] && line[1] == linetarget[1]) ||
							(line[0] == linetarget[1] && line[1] == linetarget[0])){

							base->SetLink(lineID,target);
							target->SetLink(lineTarID, base);
							linkflag = true;
							break;
						}
					}
					if (linkflag)break;
				}

			}
		}
	}


	int GetNaviMeshNum(){
		return (int)mNaviMeshDataBase.size();
	}

	NaviMeshPolygon* FindNaviPosition(XMVECTOR position){

		auto min = 10000000000000000.0f;
		int id = -1;
		for (auto &poly : mNaviMeshDataBase){
			float l = XMVector3Length(poly.GetData().CenterPosition - position).x;
			if (min > l){
				id = poly.GetData().NaviMeshID;
				min = l;
			}
		}

		return id == -1 ? NULL : &mNaviMeshDataBase[id];

	}
private:
	std::vector<NaviMeshPolygon> mNaviMeshDataBase;
};

struct NaviLine{
	const NaviMeshPolygon* NaviMeshPoly;
	XMVECTOR Line;
	float Length;
};


class Navigate{

public:
	Navigate(std::list<NaviLine> route,float TotalLength)
		: mRoute(route)
		, mTotalLength(TotalLength)
		, mMoveTotal(0.0f)
		, mCurrentMove(0.0f)
		, mCreateRoute(false)
	{
		mCurrent = mRoute.begin();
		if (mRoute.size() != 0){
			mCreateRoute = true;
		}
	}

	void Move(float speed){

		if (speed >= 0 && mCreateRoute){

			if (--mRoute.end() == mCurrent){
				mCurrentMove = 0;
				mMoveTotal = mTotalLength;
				return;
			}

			if (((*mCurrent).Length - mCurrentMove) < speed){

				mCurrentMove = 0;

				speed -= (*mCurrent).Length;

				++mCurrent;
				Move(speed);

				mMoveTotal += (*mCurrent).Length;
			}
			else{
				mCurrentMove += speed;
				mMoveTotal += speed;
			}
		}
	}

	XMVECTOR GetPosition(){
		if (!mCreateRoute)return XMVectorZero();
		float par = mCurrentMove / (*mCurrent).Length;
		return (*mCurrent).NaviMeshPoly->GetData().CenterPosition + (*mCurrent).Line * par;
	}

	bool IsMoveEnd(){
		if (!mCreateRoute)return true;
		return --mRoute.end() == mCurrent;
	}

private:
	std::list<NaviLine> mRoute;
	std::list<NaviLine>::iterator mCurrent;
	float mTotalLength;
	float mMoveTotal;
	float mCurrentMove;
	bool mCreateRoute;
};

class NavigateCreator{
public:
	NavigateCreator(
		NaviMeshPolygon* start,
		NaviMeshPolygon* end,
		NaviMesh* naviMesh
		)
		: mStart(start)
		, mEnd(end)
		, mNaviMesh(naviMesh)
		, mTotalLength(0.0f)
	
	{
		
		Create();
	}

	shared_ptr<Navigate> Result(){
		return  shared_ptr<Navigate>(new Navigate(mRoute, mTotalLength));
		// make_shared<Navigate, std::list<NaviLine>, float>(mRoute, mTotalLength);
	}

private:
	void Create(){


		int num = mNaviMesh->GetNaviMeshNum();
		mNaviMeshRouteMap.resize(num);

		{
			auto endID = mStart->GetData().NaviMeshID;
			mNaviMeshRouteMap[endID].Cost = 0.0f;
		}

		if (Search()){
			CreateRoute(mEnd);
		}

		for (auto line : mRoute){
			mTotalLength += line.Length;
		}


	}

	bool Search(){
		std::list<const NaviMeshPolygon*> list[2];

		std::list<const NaviMeshPolygon*>* p_next = &list[0];
		std::list<const NaviMeshPolygon*>* p_current = &list[1];

		p_next->push_back(mStart);

		NaviMeshPolygon* base = mStart;

		for (int count = 1;; count++){

			std::swap(p_next, p_current);

			p_next->clear();
			for (auto link : (*p_current)){

				if (SearchLink(link, count, *p_next)){
					return true;
				}

			}
			//ルートが見つからない
			if (p_next->size() == 0){
				return false;
			}
		}
		return false;
	}


	bool SearchLink(const NaviMeshPolygon* base, int count, std::list<const NaviMeshPolygon*>& next){
		auto link = base->GetLink();
		for (int i = 0; i < 3; i++){

			auto target = link[i];
			//リンクがないなら
			if (target == NULL)continue;
			//ゴールまでたどり着いたら
			if (target == mEnd){
				return true;
			}

			auto baseID = base->GetData().NaviMeshID;
			auto basecost = mNaviMeshRouteMap[baseID];

			auto linkcost = target->GetLinkLength(i) + basecost.Cost;

			auto id = target->GetData().NaviMeshID;
			float cost = mNaviMeshRouteMap[id].Cost;
			if (cost == -1.0f || cost > linkcost){
				mNaviMeshRouteMap[id].Cost = linkcost;

				if (mNaviMeshRouteMap[id].Count != count){
					mNaviMeshRouteMap[id].Count = count;
					next.push_back(target);
				}
			}
		}

		return false;
	}

	//スタートまでルート作成
	void CreateRoute(const NaviMeshPolygon* current){

		float costs[] = {-1.0f,-1.0f,-1.0f};
		const NaviMeshPolygon* const* link = current->GetLink();
		for (int i = 0; i < 3; i++){

			auto target = link[i];
			//リンクがないなら
			if (target == NULL)continue;

			auto id = target->GetData().NaviMeshID;
			costs[i] = mNaviMeshRouteMap[id].Cost;
		}

		int lowId = GetLowCost(0, costs[0], 1, costs[1]);
		lowId = GetLowCost(lowId, costs[lowId], 2, costs[2]);


		//再起で道リスト作成
		auto back = link[lowId];

		if (back != mStart && back != NULL){
			CreateRoute(back);
		}

		NaviLine line;
		line.NaviMeshPoly = back;
		line.Line = current->GetData().CenterPosition - back->GetData().CenterPosition;
		line.Length = current->GetLinkLength(lowId);
		mRoute.push_back(line);
	}

	//-1.0fを配慮したローコストの選択
	int GetLowCost(int id1, float cost1, int id2, float cost2){
		if (cost1 == -1.0f){
			return id2;
		}
		else if (cost2 == -1.0f){
			return id1;
		}
		else{
			if (cost1 < cost2){
				return id1;
			}
			else{
				return id2;
			}
		}
		return id1;
	}

private:
	NaviMeshPolygon* mStart;
	NaviMeshPolygon* mEnd;
	NaviMesh* mNaviMesh;

	struct RouteCost{
		float Cost = -1.0f;
		int Count = 0;
	};
	std::vector<RouteCost> mNaviMeshRouteMap;

	std::list<NaviLine> mRoute;
	float mTotalLength;
};