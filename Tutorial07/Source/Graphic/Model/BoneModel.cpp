

#include <algorithm>


#include "BoneModel.h"


#include "Graphic/Loader/Model/pmx.h"











// スケール、回転、移動行列
XMMATRIX SRTMatrix(const XMVECTOR& scale, const XMVECTOR& quat_rot, const XMVECTOR& trans)
{
	return XMMatrixMultiply(
		XMMatrixMultiply(
		XMMatrixScalingFromVector(scale),
		XMMatrixRotationQuaternion(quat_rot)),
		XMMatrixTranslationFromVector(trans));
}
// スケール、回転、移動行列
XMMATRIX SQTMatrix(const XMVECTOR& scale, const XMVECTOR& quat_rot, const XMVECTOR& trans)
{
	return XMMatrixMultiply(
		XMMatrixMultiply(
		XMMatrixScalingFromVector(scale),
		XMMatrixRotationQuaternion(quat_rot)),
		XMMatrixTranslationFromVector(trans));
}

XMFLOAT4 VectorToFloat(const XMVECTOR& vec){
	return XMFLOAT4(XMVectorGetX(vec), XMVectorGetY(vec), XMVectorGetZ(vec), XMVectorGetW(vec));
}
XMVECTOR FloatToVector(const XMFLOAT4& f){
	return XMVectorSet(f.x, f.y, f.z, f.w);
}
XMVECTOR FloatToVector(const XMFLOAT3& f){
	return XMVectorSet(f.x, f.y, f.z, 0.0f);
}

XMVECTOR GetAngle(XMVECTOR quat)
{
	XMMATRIX mtx = XMMatrixRotationQuaternion(quat);

	//ZYX　Y=-90～90°Y軸＝ねじり方向
	float rx = -atan2f(XMVectorGetY(mtx.r[2]), XMVectorGetZ(mtx.r[2]));
	float ry = asinf(XMVectorGetX(mtx.r[2]));
	float rz = -atan2f(XMVectorGetX(mtx.r[1]), XMVectorGetX(mtx.r[0]));
	return XMVectorSet(rx, ry, rz, 0);
}

XMVECTOR LimitAngle(const XMVECTOR& quat, const XMVECTOR& rotmin, const XMVECTOR& rotmax)
{
	XMVECTOR rot_xyz = GetAngle(quat);
	/*	XMMATRIX mtx = XMMatrixRotationQuaternion(quat);

	//ZYX　Y=-90～90°Y軸＝ねじり方向
	float rx = -atan2f(XMVectorGetY(mtx.r[2]),XMVectorGetZ(mtx.r[2]));
	float ry = asinf(XMVectorGetX(mtx.r[2]));
	float rz = -atan2f(XMVectorGetX(mtx.r[1]),XMVectorGetX(mtx.r[0]));
	XMVECTOR rot_xyz = {rx,ry,rz,0};
	*rotang_before = rot_xyz;
	*/
	rot_xyz = XMVectorMax(rot_xyz, rotmin);
	rot_xyz = XMVectorMin(rot_xyz, rotmax);
	XMMATRIX mtx = XMMatrixRotationZ(XMVectorGetZ(rot_xyz));
	mtx = XMMatrixMultiply(mtx, XMMatrixRotationY(XMVectorGetY(rot_xyz)));
	mtx = XMMatrixMultiply(mtx, XMMatrixRotationX(XMVectorGetX(rot_xyz)));

	return XMQuaternionRotationMatrix(mtx);
}

bool RotDir(const XMVECTOR& dir_from, const XMVECTOR& dir_to, float ang_limit, XMVECTOR* axis, float* ang)
{
	XMVECTOR eps0 = XMVectorSet(0.0005f, 0.0005f, 0.0005f, 1.0f);
	//XMVECTOR eps0 = { 0.0000001f, 0.0000001f, 0.0000001f, 0.0000001f };
	XMVECTOR ik_dir = XMVector3Normalize(dir_to);
	XMVECTOR tg_dir = XMVector3Normalize(dir_from);
	if (XMVector3NearEqual(tg_dir, ik_dir, eps0)){
		return false;
	}
	// 回転軸と角度 
	eps0 = XMVectorSet(0.0000001f, 0.0000001f, 0.0000001f, 1.0f); //XMVectorSet(0.0005f, 0.0005f, 0.0005f, 0.0f);
	XMVECTOR rot_axis = XMVector3Cross(tg_dir, ik_dir);
	if (XMVector3NearEqual(rot_axis, XMVectorSet(0, 0, 0, 0), eps0)){
		return false;
	}
	*axis = XMVector3Normalize(rot_axis);
	*ang = XMVectorGetX(XMVectorACos(XMVector3Dot(tg_dir, ik_dir)));

	// 回転量制限
	if (*ang > ang_limit)*ang = ang_limit;
	if (*ang < -ang_limit)*ang = -ang_limit;
	return true;
}

// http://d.hatena.ne.jp/edvakf/20111016/1318716097
float Bezier(float x1, float x2, float y1, float y2, float x)
{
	x1 = x1 / 127.0f;
	y1 = y1 / 127.0f;
	x2 = x2 / 127.0f;
	y2 = y2 / 127.0f;
	float t = 0.5f, s = 0.5f;
	for (int i = 0; i < 15; i++) {
		float ft = (3 * s*s *t *x1) + (3 * s *t*t *x2) + (t*t*t) - x;
		if (ft == 0) break;
		if (ft > 0){
			t -= 1.0f / float(4 << i);
		}
		else{ // ft < 0
			t += 1.0f / float(4 << i);
		}
		s = 1 - t;
	}
	return (3 * s*s *t *y1) + (3 * s *t*t *y2) + (t*t*t);
}






BoneModel::BoneModel()
	: mIsCreateAnime(false)
	, mLastKeyNo(0)
{
}
BoneModel::~BoneModel()
{
}

HRESULT BoneModel::Create(const char* FileName){

	AssetDataBase::Instance(FileName, mBoneAssetDataPtr);
	if (!mBoneAssetDataPtr)return E_FAIL;

	createBone();

	auto& bones = mBoneAssetDataPtr->GetFileData().GetBoneData().mBoneBuffer;

	if (bones.size()){
		mCBBoneMatrix = ConstantBufferArray<cbBoneMatrix>::create(7, bones.size());
		if (!mCBBoneMatrix.mBuffer)
			return E_FAIL;
		DWORD mBoneNum = mBone.size();
		for (DWORD mid = 0; mid < mBoneNum; mid++){
			mCBBoneMatrix.mParam[mid].BoneMatrix[0] = XMFLOAT4(1,0,0,0);
			mCBBoneMatrix.mParam[mid].BoneMatrix[1] = XMFLOAT4(0,1,0,0);
			mCBBoneMatrix.mParam[mid].BoneMatrix[2] = XMFLOAT4(0,0,1,0);
		}
		mCBBoneMatrix.UpdateSubresource();
		SetConstantBuffer();
	}
	return S_OK;
}


void BoneModel::createBone(){

	auto& bones = mBoneAssetDataPtr->GetFileData().GetBoneData().mBoneBuffer;
	auto& boneName = mBoneAssetDataPtr->GetFileData().GetBoneData().mBoneName;

	DWORD mBoneNum = bones.size();
	mBone.clear();
	mIk.clear();
	mBone.resize(mBoneNum);

	DWORD ikCount = 0;
	for (DWORD i = 0; i < mBoneNum; i++){
		auto& bone = bones[i];


		mBone[i].mStrName = boneName[i];
		mBone[i].mHierarchy.mIdxSelf = i;
		mBone[i].mHierarchy.mIdxParent = bone.parent_bidx;
		if (bone.parent_bidx >= (int)mBoneNum) mBone[i].mHierarchy.mIdxParent = UINT(-1);


		XMVECTOR head_pos = XMVectorSet(bone.bone_head_pos[0], bone.bone_head_pos[1], bone.bone_head_pos[2], 0.0f);
		XMVECTOR parent_pos = { 0, 0, 0, 1 };
		if (mBone[i].mHierarchy.mIdxParent < (int)mBoneNum){
			UINT p = mBone[i].mHierarchy.mIdxParent;
			parent_pos = XMVectorSet(bones[p].bone_head_pos[0], bones[p].bone_head_pos[1], bones[p].bone_head_pos[2], 0.0f);
		}

		XMVECTOR local_pos = XMVectorSubtract(head_pos, parent_pos);

		mBone[i].mPos = XMFLOAT3(XMVectorGetX(local_pos), XMVectorGetY(local_pos), XMVectorGetZ(local_pos));
		mBone[i].mScale = XMFLOAT3(1.0f, 1.0f, 1.0f);
		XMVECTOR q = XMQuaternionIdentity();
		mBone[i].mRot = XMFLOAT4(XMVectorGetX(q), XMVectorGetY(q), XMVectorGetZ(q), XMVectorGetW(q));

		//ワールド行列計算
		XMVECTOR scale = { 1, 1, 1, 1 };
		mBone[i].mMtxPose = SRTMatrix(scale, q, local_pos);
		if (mBone[i].mHierarchy.mIdxParent < (int)mBoneNum){
			mBone[i].mMtxPose = XMMatrixMultiply(mBone[i].mMtxPose, mBone[mBone[i].mHierarchy.mIdxParent].mMtxPose);
		}
		if (bone.bone_flag & pmx::t_bone::BIT_IK){
			mBone[i].mIkBoneIdx = (WORD)bone.t_ik_data_idx;

			createIk(ikCount, i);
			ikCount++;
		}
		else{
			mBone[i].mIkBoneIdx = 0;
		}


		mBone[i].mMtxPoseInit = mBone[i].mMtxPose;

	}
}

void BoneModel::createIk(DWORD ikCount, UINT bidx){


	auto& bones = mBoneAssetDataPtr->GetFileData().GetBoneData().mBoneBuffer;
	auto& iks = mBoneAssetDataPtr->GetFileData().GetBoneData().mIK_Links;

	auto& bone = bones[bidx];
	auto& ik = iks[bidx];

	mIk.push_back(Ik());

	mIk[ikCount].bone_index = bidx;
	mIk[ikCount].target_bone_index = bone.t_ik_data_idx;
	mIk[ikCount].chain_length = ik.size();
	mIk[ikCount].iterations = bone.t_ik_data_Loop;
	mIk[ikCount].control_weight = bone.t_ik_data_LimitAng;
	for (auto& ikc : ik){
		mIk[ikCount].child_bone_index.push_back(ikc.idxBone);
	}
}

void BoneModel::SetConstantBuffer() const{
	if (mCBBoneMatrix.mBuffer)mCBBoneMatrix.VSSetConstantBuffers();
}

void BoneModel::CreateAnime(vmd& VMD){

	auto& bones = mBoneAssetDataPtr->GetFileData().GetBoneData().mBoneBuffer;
	if (bones.size() == 0)return;
	mIsCreateAnime = true;

	mMotion.clear();

	DWORD mBoneNum = mBone.size();
	mMotion.resize(mBoneNum);

	mLastKeyNo = 0;

	for (unsigned int i = 0; i < VMD.mKeyNum; i++){
		auto& key = VMD.mKeyFrame[i];

		DWORD bid = 0;
		for (; bid < mBoneNum; bid++){
			if (mBone[bid].mStrName == key.BoneName){
				break;
			}
		}
		if (bid >= mBoneNum){
			continue;
		}

		Motion& mot = mMotion[bid];
		KeyFrame temp = KeyFrame(key);

		//相対値から絶対値に変更
		temp.mKeyFrame.Location[0] += mBone[bid].mPos.x;
		temp.mKeyFrame.Location[1] += mBone[bid].mPos.y;
		temp.mKeyFrame.Location[2] += mBone[bid].mPos.z;

		mot.mKeyFrame.push_back(temp);



		if (temp.mKeyFrame.FrameNo > mLastKeyNo){
			mLastKeyNo = temp.mKeyFrame.FrameNo;
		}

	}
	for (DWORD mid = 0; mid < mBoneNum; mid++){
		std::sort(mMotion[mid].mKeyFrame.begin(), mMotion[mid].mKeyFrame.end());

		if (mMotion[mid].mKeyFrame.size() == 0){
			Motion& mot = mMotion[mid];
			vmd::VMDKeyFrame key;
			const char* c = mBone[mid].mStrName.c_str();
			for (int i = 0; i < 15; i++){
				key.BoneName[i] = c[i];
				if (c[i] == '\0')break;
			}
			key.FrameNo = 0;
			key.Rotatation[0] = 0.0f;
			key.Rotatation[1] = 0.0f;
			key.Rotatation[2] = 0.0f;
			key.Rotatation[3] = 0.0f;
			for (int i = 0; i < 64; i++)
				key.Interpolation[i] = (char)0.0f;


			//相対値から絶対値に変更
			key.Location[0] = mBone[mid].mPos.x;
			key.Location[1] = mBone[mid].mPos.y;
			key.Location[2] = mBone[mid].mPos.z;

			KeyFrame temp = KeyFrame(key);
			mot.mKeyFrame.push_back(temp);

		}
	}
}

void BoneModel::PlayVMD(float time){
	if (!mBoneAssetDataPtr)return;
	if (mMotion.empty())return;
	VMDAnimation(time);

	// ボーン差分行列の作成、定数バッファに転送
	for (UINT ib = 0; ib<mBone.size(); ++ib){
		XMVECTOR Determinant;
		XMMATRIX invmtx = XMMatrixInverse(&Determinant, mBone[ib].mMtxPoseInit);
		XMMATRIX mtx = XMMatrixMultiplyTranspose(invmtx, mBone[ib].mMtxPose);
		mCBBoneMatrix.mParam[ib].BoneMatrix[0] = XMFLOAT4(mtx.r[0].x, mtx.r[0].y, mtx.r[0].z, mtx.r[0].w);//mtx.r[0];
		mCBBoneMatrix.mParam[ib].BoneMatrix[1] = XMFLOAT4(mtx.r[1].x, mtx.r[1].y, mtx.r[1].z, mtx.r[1].w);//mtx.r[1];
		mCBBoneMatrix.mParam[ib].BoneMatrix[2] = XMFLOAT4(mtx.r[2].x, mtx.r[2].y, mtx.r[2].z, mtx.r[2].w);//mtx.r[2];
	}

	mCBBoneMatrix.UpdateSubresource();
}

// とりあえずアニメーション
void BoneModel::VMDAnimation(float key_time)
{
	if (!mBoneAssetDataPtr)return;
	if (mMotion.empty())return;

	auto& bones = mBoneAssetDataPtr->GetFileData().GetBoneData().mBoneBuffer;
	if (bones.empty())return;

	for (DWORD mid = 0; mid < bones.size(); mid++){

		Motion& mot = mMotion[mid];

		if (mot.mKeyFrame.size() == 0)continue;
		UINT idx = 0;
		for (auto key : mot.mKeyFrame){
			if (key.mKeyFrame.FrameNo >= key_time){
				break;

			}
			++idx;
		}

		if (idx >= mot.mKeyFrame.size())idx = mot.mKeyFrame.size() - 1;

		UINT prev = idx;
		if (idx>0)prev = idx - 1;

		vmd::VMDKeyFrame& key0 = mot.mKeyFrame[prev].mKeyFrame;
		vmd::VMDKeyFrame& key1 = mot.mKeyFrame[idx].mKeyFrame;

		float t = 1.0f;
		if (idx != prev){
			t = float(key_time - key0.FrameNo) / float(key1.FrameNo - key0.FrameNo);
		}
		if (t < 0.0f)t = 0.0f;
		if (t > 1.0f)t = 1.0f;


		{
			XMVECTOR pos0 = XMVectorSet(key0.Location[0], key0.Location[1], key0.Location[2], 0.0f);
			XMVECTOR pos1 = XMVectorSet(key1.Location[0], key1.Location[1], key1.Location[2], 0.0f);
			XMVECTOR rot0 = XMVectorSet(key0.Rotatation[0], key0.Rotatation[1], key0.Rotatation[2], key0.Rotatation[3]);
			XMVECTOR rot1 = XMVectorSet(key1.Rotatation[0], key1.Rotatation[1], key1.Rotatation[2], key1.Rotatation[3]);

			float tx = Bezier(key0.Interpolation[0], key0.Interpolation[8], key0.Interpolation[4], key0.Interpolation[12], t);
			float ty = Bezier(key0.Interpolation[1], key0.Interpolation[9], key0.Interpolation[5], key0.Interpolation[13], t);
			float tz = Bezier(key0.Interpolation[2], key0.Interpolation[10], key0.Interpolation[6], key0.Interpolation[14], t);
			float tr = Bezier(key0.Interpolation[3], key0.Interpolation[11], key0.Interpolation[7], key0.Interpolation[15], t);


			XMVECTOR v = XMVectorLerpV(pos0, pos1, XMVectorSet(tx, ty, tz, 0.0f));
			mBone[mid].mPos = XMFLOAT3(XMVectorGetX(v), XMVectorGetY(v), XMVectorGetZ(v));
			XMVECTOR q = XMQuaternionSlerp(rot0, rot1, tr);
			mBone[mid].mRot = XMFLOAT4(XMVectorGetX(q), XMVectorGetY(q), XMVectorGetZ(q), XMVectorGetW(q));

			////ワールド行列計算
			//XMVECTOR scale = { 1, 1, 1, 1 };
			//mBone[mid].mMtxPose = SRTMatrix(scale, q, v);
			//if (mBone[mid].mHierarchy.mIdxParent < mBoneNum){
			//	mBone[mid].mMtxPose = XMMatrixMultiply(mBone[mid].mMtxPose, mBone[mBone[mid].mHierarchy.mIdxParent].mMtxPose);
			//}
		}
	}
	UpdatePose();
	VMDIkAnimation();
}

//--------------------------------------
//姿勢行列更新
void BoneModel::UpdatePose()
{

	DWORD mBoneNum = mBone.size();
	for (DWORD mid = 0; mid < mBoneNum; mid++){
		//ワールド行列計算
		mBone[mid].mMtxPose = SQTMatrix(FloatToVector(mBone[mid].mScale), FloatToVector(mBone[mid].mRot), FloatToVector(mBone[mid].mPos));
		if (mBone[mid].mHierarchy.mIdxParent < mBoneNum){
			mBone[mid].mMtxPose = XMMatrixMultiply(mBone[mid].mMtxPose, mBone[mBone[mid].mHierarchy.mIdxParent].mMtxPose);
		}
	}
}

//-------------------------------------------
// とりあえずIK
void BoneModel::VMDIkAnimation()
{

	//XMStoreFloat4()
	//XMLoadFloat4()
	if (mBone.empty())return;
	if (mMotion.empty())return;

	DWORD mBoneNum = mBone.size();
	DWORD mIkNum = mIk.size();
	// IK計算
	for (DWORD i = 0; i < mIkNum; i++){
		//{
		//	int i = 0;
		Ik& ik = mIk[i];
		UINT tg_idx = ik.target_bone_index;
		UINT ik_idx = ik.bone_index;

		for (UINT ite = 0; ite<ik.iterations; ++ite){
			for (UINT chn = 0; chn<ik.chain_length; ++chn){
				UINT link_idx = ik.child_bone_index[chn];//
				if (link_idx >= mBoneNum)continue;
				Bone& link_bone = mBone[link_idx];

				//UINT link_pidx = link_bone.mIkBoneIdx;
				UINT link_pidx = link_bone.mHierarchy.mIdxParent;

				//if (link_bone.mIkBoneIdx != 0){
				//	continue;
				//}

				if (link_pidx >= mBoneNum)continue;
				Bone& link_parent = mBone[link_pidx];

				Bone& tg_bone = mBone[tg_idx];
				(void)tg_bone;
				Bone& ik_bone = mBone[ik_idx];
				(void)ik_bone;

				XMVECTOR target_wpos = mBone[tg_idx].mMtxPose.r[3];
				XMVECTOR ik_wpos = mBone[ik_idx].mMtxPose.r[3];
				XMVECTOR lp_wpos = link_parent.mMtxPose.r[3];

				//Linkボーンのローカル空間に変換
				XMVECTOR Determinant;
				XMMATRIX inv_mtx = XMMatrixInverse(&Determinant, link_bone.mMtxPose);
				XMVECTOR tg_pos = XMVector4Transform(target_wpos, inv_mtx);
				XMVECTOR ik_pos = XMVector4Transform(ik_wpos, inv_mtx);
				XMVECTOR lp_pos = XMVector4Transform(lp_wpos, inv_mtx);


				// 回転軸と角度 
				XMVECTOR rot_axis = XMVectorSet(1, 0, 0, 0);
				float ang = 0.0f;
				bool same_dir = false;
				if (!RotDir(tg_pos, ik_pos, ik.control_weight, &rot_axis, &ang)){
					same_dir = true;
				}

				if (!same_dir){

					//tg_dirをik_dirに一致させるための回転
					XMVECTOR rot = XMQuaternionRotationAxis(rot_axis, ang);

					XMVECTOR lrot = FloatToVector(link_bone.mRot);
					XMVECTOR bone_rot_before = lrot;
					link_bone.mRot = VectorToFloat(XMQuaternionMultiply(rot, lrot));

					float dist_tg = XMVectorGetX(XMVector3Length(tg_pos));
					float dist_ik = XMVectorGetX(XMVector3Length(ik_pos));
					(void)dist_ik;
					float dist_lp = XMVectorGetX(XMVector3Length(lp_pos));
					(void)dist_lp;
					float dist_pltg = XMVectorGetX(XMVector3Length(lp_pos - tg_pos));
					float dist_plik = XMVectorGetX(XMVector3Length(lp_pos - ik_pos));
					float dot_tgik = XMVectorGetX(XMVector3Dot(XMVector3Normalize(tg_pos), XMVector3Normalize(ik_pos)));
					(void)dot_tgik;

					// 回転制限
					if (/*link.bLimit*/ 1){
						XMVECTOR rotmax, rotmin;
						//114.5916 = 2
						float a = 2;// XM_PI / 180.0f * 57.25f;
						rotmax = XMVectorSet(a, a, a, 0);//link.vMax;
						rotmin = XMVectorSet(-a, -a, -a, 0);//link.vMin;

						//名前に"ひざ"があったら回転制限
						if (std::string::npos != link_bone.mStrName.find("ひざ")){
							rotmax = XMVectorSet(-XM_PI / 180.0f*0.5f, 0, 0, 0);
							rotmin = XMVectorSet(-XM_PI, 0, 0, 0);
						}
						struct IkLink{
							XMFLOAT4 mMax;
							XMFLOAT4 mMin;
						};
						IkLink link = { VectorToFloat(rotmax), VectorToFloat(rotmin) };
						//Bone& link = link_bone;
						link_bone.mRot = VectorToFloat(LimitAngle(FloatToVector(link_bone.mRot), rotmin, rotmax));

						XMVECTOR angxyz = GetAngle(rot);
						//膝を曲げるための仮処理　かなりてきとう
						if (XMVectorGetX(angxyz) >= 0 &&
							//0.9f < dot_tgik &&
							//dist_tg > dist_ik &&
							dist_pltg > dist_plik &&
							link.mMax.x < 0 && link.mMax.y == link.mMin.y && link.mMax.z == link.mMin.z){
							//親リンクの回転接平面（できるだけこの平面に近づけたほうがよりIK目標に近づける）
							XMVECTOR lp_nor = XMVector3Normalize(-lp_pos);//平面の法線
							//lp_norとの内積が0になる位置を目標にする
							//２つあるので回転制限後の|内積|が小さいほう
							XMVECTOR tng = XMVector3Cross(XMVectorSet(1, 0, 0, 0), lp_nor);
							//+tngと-tngの２つ
							XMVECTOR rot_axis0, rot_axis1;
							float ang0 = 0, ang1 = 0;

							// 回転軸をXに限定
							rot_axis1 = rot_axis0 = XMVectorSet(1, 0, 0, 0);
							XMVECTOR tdir = XMVector3Normalize(XMVectorSetX(tg_pos, 0));
							tng = XMVector3Normalize(XMVectorSetX(tng, 0));
							RotDir(tdir, tng, ik.control_weight, &rot_axis0, &ang0);
							RotDir(tdir, -tng, ik.control_weight, &rot_axis1, &ang1);
							if (XMVectorGetX(rot_axis0) < 0.0f)ang0 = -ang0;
							if (XMVectorGetX(rot_axis1) < 0.0f)ang1 = -ang1;

							//これは絶対違う　ぴくぴく対策
							float coef = (dist_pltg - dist_plik) / dist_tg;
							if (coef > 1)coef = 1;
							ang0 *= coef;
							ang1 *= coef;


							//ang0,1は現在の位置からの相対角度　
							// 回転制限を考慮した相対角度に
							float angx_b = XMVectorGetX(GetAngle(bone_rot_before));
							float angx_a0 = angx_b + ang0;
							float angx_a1 = angx_b + ang1;
							if (angx_a0 < link.mMin.x) angx_a0 = link.mMin.x;
							if (angx_a0 > link.mMax.x) angx_a0 = link.mMax.x;
							if (angx_a1 < link.mMin.x) angx_a1 = link.mMin.x;
							if (angx_a1 > link.mMax.x) angx_a1 = link.mMax.x;
							ang0 = angx_a0 - angx_b;
							ang1 = angx_a1 - angx_b;


							XMVECTOR rot0 = XMQuaternionRotationRollPitchYaw(ang0, 0, 0);
							XMVECTOR rot1 = XMQuaternionRotationRollPitchYaw(ang1, 0, 0);

							XMVECTOR tdir0 = XMVector3TransformCoord(tdir, XMMatrixRotationQuaternion(rot0));
							XMVECTOR tdir1 = XMVector3TransformCoord(tdir, XMMatrixRotationQuaternion(rot1));
							float d0 = XMVectorGetX(XMVectorAbs(XMVector3Dot(tdir0, lp_nor)));
							float d1 = XMVectorGetX(XMVectorAbs(XMVector3Dot(tdir1, lp_nor)));
							if (d0 < d1){
								link_bone.mRot = VectorToFloat(XMQuaternionMultiply(rot0, bone_rot_before));
							}
							else{
								link_bone.mRot = VectorToFloat(XMQuaternionMultiply(rot1, bone_rot_before));
							}
						}
					}

				}




				//ワールド行列更新
				link_bone.mMtxPose = SQTMatrix(FloatToVector(link_bone.mScale), FloatToVector(link_bone.mRot), FloatToVector(link_bone.mPos));
				if (link_bone.mHierarchy.mIdxParent < mBoneNum){
					link_bone.mMtxPose = XMMatrixMultiply(link_bone.mMtxPose, mBone[link_bone.mHierarchy.mIdxParent].mMtxPose);
				}

				// 子階層のリンク再計算
				for (int lidown = chn - 1; lidown >= 0; --lidown){
					UINT idx = ik.child_bone_index[lidown];
					if (idx >= mBoneNum)continue;
					Bone& linkb = mBone[idx];
					linkb.mMtxPose = SQTMatrix(FloatToVector(linkb.mScale), FloatToVector(linkb.mRot), FloatToVector(linkb.mPos));
					if (linkb.mHierarchy.mIdxParent < mBoneNum){
						linkb.mMtxPose = XMMatrixMultiply(linkb.mMtxPose, mBone[linkb.mHierarchy.mIdxParent].mMtxPose);
					}
				}

				mBone[tg_idx].mMtxPose = SQTMatrix(FloatToVector(mBone[tg_idx].mScale), FloatToVector(mBone[tg_idx].mRot), FloatToVector(mBone[tg_idx].mPos));
				if (mBone[tg_idx].mHierarchy.mIdxParent < mBoneNum){
					mBone[tg_idx].mMtxPose = XMMatrixMultiply(mBone[tg_idx].mMtxPose, mBone[mBone[tg_idx].mHierarchy.mIdxParent].mMtxPose);
				}
			}
		}


		//Bone& b = mBone[tg_idx];
		//Bone& b2 = mBone[mBone[tg_idx].mHierarchy.mIdxParent];
		//Bone& b3 = mBone[b2.mHierarchy.mIdxParent];
		//int sa = 1;

		//IKの計算結果を子階層に反映
		//UpdatePose();
	}
	UpdatePose();
}



UINT BoneModel::GetMaxAnimeTime(){
	return mLastKeyNo;
}
