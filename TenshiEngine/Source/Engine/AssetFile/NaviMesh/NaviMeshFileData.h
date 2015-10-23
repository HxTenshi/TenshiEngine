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

		mLinkPolygonLength[0] = -1.0f;
		mLinkPolygonLength[1] = -1.0f;
		mLinkPolygonLength[2] = -1.0f;
	}


	NaviMeshPolygon(const NaviMeshPolygonData& Data){
		mNaviMeshPolygonData = Data;

		mLinkPolygon[0] = NULL;
		mLinkPolygon[1] = NULL;
		mLinkPolygon[2] = NULL;

		mLinkPolygonLength[0] = -1.0f;
		mLinkPolygonLength[1] = -1.0f;
		mLinkPolygonLength[2] = -1.0f;

	}

	void SetLink(int id, NaviMeshPolygon* ptr){
		mLinkPolygon[id] = ptr;
		mLinkPolygonLength[id] = XMVector3Length(ptr->GetData().CenterPosition - mNaviMeshPolygonData.CenterPosition).x;
	}
	void GetLine(int id, unsigned int* idx1, unsigned int* idx2){
		int id1 = id;
		int id2 = (id + 1) % 3;
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

	bool CheckHitRay(XMVECTOR p1, XMVECTOR p2) const{

		//交差点を求める
		auto pos1 = mNaviMeshPolygonData.Vertex[0];
		auto v1 = p1 - pos1;
		auto v2 = p2 - pos1;
		auto d1 = XMVector3Length(v1).x;
		auto d2 = XMVector3Length(v2).x;
		auto a = d1 / (d1 + d2);
		//点の位置
		auto v3 = pos1 + (1 - a)*v1 + a*v2;

		//ポリゴン内か求める
		auto pl1 = XMVector3Normalize(mNaviMeshPolygonData.Vertex[1] - mNaviMeshPolygonData.Vertex[0]);
		auto pl2 = XMVector3Normalize(mNaviMeshPolygonData.Vertex[2] - mNaviMeshPolygonData.Vertex[1]);
		auto pl3 = XMVector3Normalize(mNaviMeshPolygonData.Vertex[0] - mNaviMeshPolygonData.Vertex[2]);
		auto pv1 = XMVector3Normalize(v3 - mNaviMeshPolygonData.Vertex[0]);
		auto pv2 = XMVector3Normalize(v3 - mNaviMeshPolygonData.Vertex[1]);
		auto pv3 = XMVector3Normalize(v3 - mNaviMeshPolygonData.Vertex[2]);


		auto dot1 = XMVector3Dot(XMVector3Cross(pl1, pv1), mNaviMeshPolygonData.Normal).x;
		auto dot2 = XMVector3Dot(XMVector3Cross(pl2, pv2), mNaviMeshPolygonData.Normal).x;
		auto dot3 = XMVector3Dot(XMVector3Cross(pl3, pv3), mNaviMeshPolygonData.Normal).x;

		if (XMVector3Length(pl1 - pv1).x < 0.1f){
			dot1 = 1.0f;
		}
		if (XMVector3Length(pl2 - pv2).x < 0.1f){
			dot2 = 1.0f;
		}
		if (XMVector3Length(pl3 - pv3).x < 0.1f){
			dot3 = 1.0f;
		}

		if (dot1 >= 0 && dot2 >= 0 && dot3 >= 0){
			return true;
		}

		return false;


		//auto v = p1 - p2;
		//auto vp = p1 - mNaviMeshPolygonData.CenterPosition;
		//auto t = XMVector3Dot(XMVector3Normalize(v), vp).x / XMVector3Length(v).x;
		//auto h = XMVector3Length(v * t - vp).x;
		//
		//return h < 1.0f;
	}

private:
	NaviMeshPolygonData mNaviMeshPolygonData;

	NaviMeshPolygon *mLinkPolygon[3];
	float mLinkPolygonLength[3];

};

struct NaviMeshWallData{
	XMVECTOR Vertex[2];
	unsigned int Index[2];
	int NaviMeshWallID;
};


class NaviMeshWall{
public:
	NaviMeshWall(){
	}


	NaviMeshWall(const NaviMeshWallData& Data){
		mNaviMeshWallData = Data;
	}


	const NaviMeshWallData& GetData() const{
		return mNaviMeshWallData;
	}
	const NaviMeshPolygon* GetNaviMesh() const{
		return mNaviMeshPolygon;
	}

	bool CheckHitRay(XMVECTOR p1, XMVECTOR p2) const{

		
		auto s0 = mNaviMeshWallData.Vertex[0];
		auto v0 = mNaviMeshWallData.Vertex[1] - s0;

		auto s1 = p1;
		auto v1 = p2 - s1;

		s0.y = s0.z;
		v0.y = v0.z;
		s1.y = s1.z;
		v1.y = v1.z;

		auto v = s1 - s0;
		auto crs_v0_v1 = XMVector2Cross(v0, v1).x;

		//平行
		if (crs_v0_v1 == 0.0f){
			return false;
		}

		auto crs_v_v0 = XMVector2Cross(v, v0).x;
		auto crs_v_v1 = XMVector2Cross(v, v1).x;

		auto t0 = crs_v_v1 / crs_v0_v1;
		auto t1 = crs_v_v0 / crs_v0_v1;

		if (t0 - FLT_EPSILON < 0 || t0 + FLT_EPSILON>1 || t1 - FLT_EPSILON < 0 || t1 + FLT_EPSILON>1){
			return false;
		}

		return true;
	}

private:
	NaviMeshWallData mNaviMeshWallData;

	NaviMeshPolygon *mNaviMeshPolygon;

};


class NaviMesh{
public:

	XMVECTOR toVector(const XMFLOAT3& f3){
		return XMVectorSet(f3.x, f3.y, f3.z, 1.0f);
	}

	void Create(const std::string& MeshFile){
		MeshAssetDataPtr mMeshAsset;
		AssetDataBase::Instance(MeshFile.c_str(), mMeshAsset);

		if (!mMeshAsset)return;
		if (!mMeshAsset.Get())return;
		if (!mMeshAsset.Get()->GetFileData())return;

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
			auto v2 = XMVector3Normalize(polydata.Vertex[2] - polydata.Vertex[0]);
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
				//すでにリンク済み
				if (base->GetLink()[lineID]){
					continue;
				}

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

		for (int i = 0; i < polynum; i++){

			for (int lineID = 0; lineID < 3; lineID++){

				auto base = &mNaviMeshDataBase[i];
				//すでにリンク済み
				if (base->GetLink()[lineID]){
					continue;
				}


				unsigned int line[2];
				base->GetLine(lineID, &line[0], &line[1]);

				auto id0 = lineID;
				auto id1 = (lineID + 1) % 3;

				NaviMeshWallData walldata;
				walldata.Index[0] = base->GetData().Index[id0];
				walldata.Index[1] = base->GetData().Index[id1];
				walldata.Vertex[0] = base->GetData().Vertex[id0];
				walldata.Vertex[1] = base->GetData().Vertex[id1];
				walldata.NaviMeshWallID = (int)mNaviMeshWallDataBase.size();
				mNaviMeshWallDataBase.push_back(NaviMeshWall(walldata));
			}
		}
	}

	//線分と壁の当たらない位置を返す
	const XMVECTOR* WallHitOuterPosition(XMVECTOR p1, XMVECTOR p2){

		auto minLen = 10000000000000000.0f;
		const XMVECTOR* temp = NULL;
		for (auto& wall : mNaviMeshWallDataBase){
			if (!wall.CheckHitRay(p1, p2))continue;

			auto l = XMVector3Length(wall.GetData().Vertex[0] - p2).x;
			if (minLen > l){
				minLen = l;
				temp = &wall.GetData().Vertex[0];
			}
			l = XMVector3Length(wall.GetData().Vertex[1] - p2).x;
			if (minLen > l){
				minLen = l;
				temp = &wall.GetData().Vertex[1];
			}
		}
		return temp;
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
	std::vector<NaviMeshWall> mNaviMeshWallDataBase;
};

struct NaviLine{
	const NaviMeshPolygon* NaviMeshPoly;
	XMVECTOR Position;
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
		float l = (*mCurrent).Length;
		if (l == 0.0f)l = 1.0f;
		float par = mCurrentMove / l;
		return (*mCurrent).Position + (*mCurrent).Line * par;
	}

	bool IsMoveEnd(){
		if (!mCreateRoute)return true;
		return --mRoute.end() == mCurrent;
	}

	const std::list<NaviLine>& GetRoute(){
		return mRoute;
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


	void N1(){


		NormalizeRoute();

		for (auto line : mRoute){
			mTotalLength += line.Length;
		}


	}
	void N2(){


		NormalizeWallRoute();

		for (auto line : mRoute){
			mTotalLength += line.Length;
		}


	}
private:
	void Create(){


		int num = mNaviMesh->GetNaviMeshNum();
		mNaviMeshRouteMap.resize(num, RouteCost());

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

			auto linkcost = base->GetLinkLength(i) + basecost.Cost;

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

		NaviLine line;
		line.NaviMeshPoly = current;
		line.Position = current->GetData().CenterPosition;
		line.Line = XMVectorZero();
		line.Length = 0.0f;
		mRoute.push_front(line);

		for (int max = 0; max < 1000;max++){

			float costs[] = { -1.0f, -1.0f, -1.0f };
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

			NaviLine line;
			line.NaviMeshPoly = back;
			line.Position = back->GetData().CenterPosition;
			line.Line = current->GetData().CenterPosition - back->GetData().CenterPosition;
			line.Length = current->GetLinkLength(lowId);
			mRoute.push_front(line);

			if (back != mStart && back != NULL){
				current = back;
			}
			else{
				break;
			}
		}
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

	//Mesh内を通るようにしながら無駄な道をスキップさせる
	void NormalizeRoute(){
	
		auto current = mRoute.begin();
		
		while (mRoute.end() != current){
			auto v1 = (*current).Position;
	
			auto target = std::list<NaviLine>::iterator(current);
			++target;
			while (mRoute.end() != target){
				auto v2 = (*target).Position;
			
				auto temp = std::list<NaviLine>::iterator(target);
				bool outFlag = false;
				while (temp != current){
					if(!temp->NaviMeshPoly->CheckHitRay(v1, v2)){
						outFlag = true;
						break;
					}
					--temp;
				}
				if (outFlag){
					break;
				}
				++target;
			}
			--target;
	
			if (current != target){
				(*current).Line = (*target).Position - v1;
				(*current).Length = XMVector3Length((*current).Line).x;
				++current;
				current = mRoute.erase(current, target);
			}
			else{
				++current;
			}
		}
	
	}

	//壁に当たらないようにしながら無駄な道をスキップさせる
	void NormalizeWallRoute(){

		auto current = mRoute.begin();
		while (mRoute.end() != current){

			auto v1 = (*current).Position;

			auto target = std::list<NaviLine>::iterator(current);
			++target;
			bool hit = false;
			while (mRoute.end() != target){
				auto v2 = (*target).Position;

				auto pos = mNaviMesh->WallHitOuterPosition(v1, v2);
				//Hit
				if (pos){
					hit = true;
					break;
				}

				++target;
			}
			--target;

			XMVECTOR NextPos;
			if (hit){
				while (true){
					auto p = FindShareVertex(target);
					if (p){
						auto v2 = (*p);
						auto pos = mNaviMesh->WallHitOuterPosition(v1, v2);
						if (pos){
							--target;
							if (current == target){
								break;
							}
							continue;
						}

						(*target).Position = v2;
					}

					break;
				}
			}
			NextPos = (*target).Position;
			

			if (current != target){
				(*current).Line = NextPos - v1;
				(*current).Length = XMVector3Length((*current).Line).x;
				++current;
				current = mRoute.erase(current, target);
				
			}
			else{
				++current;
			}
		}

	}

	const XMVECTOR* FindShareVertex(std::list<NaviLine>::iterator vaviline){

		if (vaviline == mRoute.begin())return &vaviline->Position;

		auto back = std::list<NaviLine>::iterator(vaviline);
		auto next = std::list<NaviLine>::iterator(vaviline);
		--back;
		++next;

		if (next == mRoute.end())return &vaviline->Position;

		for (int i = 0; i < 3; i++){
			auto id = vaviline->NaviMeshPoly->GetData().Index[i];
			bool f1 = false, f2 = false;
			for (int j = 0; j < 3; j++){
				if (back->NaviMeshPoly->GetData().Index[j] == id){
					f1 = true;
					break;
				}
			}
			for (int j = 0; j < 3; j++){
				if (next->NaviMeshPoly->GetData().Index[j] == id){
					f2 = true;
					break;
				}
			}
			if (f1&&f2){
				return &vaviline->NaviMeshPoly->GetData().Vertex[i];
			}
		}
		return &vaviline->Position;
	}

private:
	NaviMeshPolygon* mStart;
	NaviMeshPolygon* mEnd;
	NaviMesh* mNaviMesh;

	struct RouteCost{
		RouteCost():Cost(-1.0f),Count(0){}
		float Cost = -1.0f;
		int Count = 0;
	};
	std::vector<RouteCost> mNaviMeshRouteMap;

	std::list<NaviLine> mRoute;
	float mTotalLength;
};