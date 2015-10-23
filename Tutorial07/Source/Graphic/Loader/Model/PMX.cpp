
#include "PMX.h"
#include <windows.h>
#include <stdio.h>

bool utf16_to_utf8(const wchar_t* utf16, unsigned int len, std::string& str)
{
	setlocale(LC_CTYPE, "jpn");
	size_t size;
	if (0 != wcstombs_s(&size, NULL, 0, utf16, len*sizeof(wchar_t))){
		return false;
	}
	std::vector<char> ch(size);
	if (0 != wcstombs_s(&size, ch.data(), ch.size(), utf16, len*sizeof(wchar_t))){
		return false;
	}
	str = ch.data();
	return true;
}

void pmx::LoadText(FILE* fp, std::string& str){
	int count = 0;
	fread(&count, sizeof(int), 1, fp);
	wchar_t* pstr = new wchar_t[count/2+1];
	pstr[count / 2] = L'\0';
	fread(pstr, sizeof(wchar_t), count/2, fp);
	utf16_to_utf8(pstr, count, str);
	delete[] pstr;
}
void pmx::Read(int* p, unsigned char size, FILE* fp){
	if (size == 1){
		char temp;
		fread(&temp, size, 1, fp);
		*p = temp;
	}
	if (size == 2){
		short temp;
		fread(&temp, size, 1, fp);
		*p = temp;
	}
	if (size == 4){
		int temp;
		fread(&temp, size, 1, fp);
		*p = temp;
	}
}

void pmx::NewVoidPointer(void** p, int newNum, unsigned char size){
	if (size == 1){
		byte *temp = new byte[newNum];
		*p = temp;
	}
	if (size == 2){
		unsigned short* temp = new unsigned short[newNum];
		*p = temp;
	}
	if (size == 4){
		unsigned int* temp = new unsigned int[newNum];
		*p = temp;
	}
}
void pmx::DeleteVoidPointer(void** p, unsigned char size){
	if (size == 1){
		delete[] (byte*)*p;
	}
	if (size == 2){
		delete[] (short*)*p;
	}
	if (size == 4){
		delete[] (int*)*p;
	}
}

//コンストラクタ
pmx::pmx(const char *filename)
{
	FILE *hFP;
	hFP = fopen(filename, "rb");
	if (hFP != 0){
		//ヘッダー読み込み
		fread(&header, sizeof(t_header), 1, hFP);

		//ヘッダー読み込み
		LoadText(hFP, modelInfo.modelName);
		LoadText(hFP, modelInfo.modelNameEn);
		LoadText(hFP, modelInfo.modelInfo);
		LoadText(hFP, modelInfo.modelInfoEn);

		//頂点数読み込み
		fread(&vert_count, sizeof(vert_count), 1, hFP);

		//頂点データ読み込み
		vertex_data.vertex = new t_vertex_data::t_vertex[vert_count];
		int adduvNum = header.config[t_header::ADDUV];
		unsigned char boneidxsize = header.config[t_header::BIDX];
		for (int i = 0 ; i < vert_count; i++){
			auto& v = vertex_data.vertex[i];
			fread(&v.v, sizeof(t_vertex_data::t_vertex::t_v), 1, hFP);
			//可変頂点データ読み込み
			for (int j = 0; j < 4; j++){
				if (j < adduvNum){
					fread(&v.avAdd[j][0], sizeof(float), 4, hFP);
				}
				else{
					v.avAdd[j][0] = 0.0f;
					v.avAdd[j][1] = 0.0f;
					v.avAdd[j][2] = 0.0f;
					v.avAdd[j][3] = 0.0f;
				}
			}
			fread(&v.weight_type, sizeof(byte), 1, hFP);

			int bnum = 0;
			int wnum = 0;
			if (v.weight_type == t_vertex_data::BDEF1){
				bnum = 1;
			}
			if (v.weight_type == t_vertex_data::BDEF2){
				bnum = 2;
				wnum = 1;
			}
			if (v.weight_type == t_vertex_data::BDEF4){
				bnum = 4;
				wnum = 4;
			}
			if (v.weight_type == t_vertex_data::SDEF){
				bnum = 2;
				wnum = 1;
			}
			if (v.weight_type == t_vertex_data::QDEF){
				bnum = 4;
				wnum = 4;
			}

			for (int j = 0; j < 4;j++){
				if (j < bnum){
					Read(&v.bone_num[j], boneidxsize, hFP);
				}
				else v.bone_num[j] = 0.0f;
			}
			for (int j = 0; j < 4; j++){
				if (j < wnum)fread(&v.bone_weight[j], sizeof(float), 1, hFP);
				else v.bone_weight[j] = 0.0f;
			}
			if (v.weight_type == t_vertex_data::SDEF){
				fread(&v.C, sizeof(float), 3, hFP);
				fread(&v.R0, sizeof(float), 3, hFP);
				fread(&v.R1, sizeof(float), 3, hFP);
			}

			fread(&v.edge_flag, sizeof(float), 1, hFP);
		}

		//面頂点リスト読み込み
		fread(&face_vert_count, sizeof(face_vert_count), 1, hFP);
		
		//面頂点リストデータ読み込み
		NewVoidPointer(&face_vert_index, face_vert_count, header.config[t_header::VIDX]);
		fread(face_vert_index, header.config[t_header::VIDX], face_vert_count, hFP);

		fread(&texture_count, sizeof(texture_count), 1, hFP);
		textureName = new std::string[texture_count];
		for (int i = 0; i < texture_count; i++){
			LoadText(hFP, textureName[i]);
		}


		//マテリアルリスト読み込み
		fread(&material_count, sizeof(material_count), 1, hFP);

		//マテリアルリストデータ読み込み
		material = new t_material[material_count];
		for (int i = 0; i < material_count; i++){
			auto& m = material[i];
			LoadText(hFP, m.MaterialName);
			LoadText(hFP, m.MaterialNameEn);
			fread(&m.material, sizeof(t_material::t_m), 1, hFP);

			//fread(&m.TextureIdx, header.config[t_header::TIDX], 1, hFP);
			//fread(&m.SphereIdx, header.config[t_header::TIDX], 1, hFP);
			Read(&m.TextureIdx, header.config[t_header::TIDX], hFP);
			Read(&m.SphereIdx, header.config[t_header::TIDX], hFP);
			fread(&m.SphereMode, sizeof(char), 1, hFP);
			fread(&m.ToonMode, sizeof(char), 1, hFP);
			byte size = 1;
			if (m.ToonMode == 0)size = header.config[t_header::TIDX];
			//fread(&m.ToonIdx, size, 1, hFP);
			Read(&m.ToonIdx, size, hFP);
			LoadText(hFP, m.memo);
			fread(&m.face_vert_count, sizeof(int), 1, hFP);
		}

		//ボーン読み込み
		fread(&bone_count, sizeof(bone_count), 1, hFP);

		ik_count = 0;
		//ボーン読み込み
		bone = new t_bone[bone_count];
		for (int i = 0; i < bone_count; i++){
			auto& b = bone[i];
			LoadText(hFP, b.BoneName);
			LoadText(hFP, b.BoneNameEn);
			fread(&b.bone_head_pos[0], sizeof(float), 3, hFP);

			//fread(&b.parent_bidx, header.config[t_header::BIDX], 1, hFP);
			Read(&b.parent_bidx, header.config[t_header::BIDX], hFP);
			fread(&b.Hierarchy, sizeof(int), 1, hFP);
			fread(&b.bone_flag, sizeof(unsigned short), 1, hFP);

			if (b.bone_flag&t_bone::BIT_JOINT){
				//fread(&b.idxJoint, header.config[t_header::BIDX], 1, hFP);
				Read(&b.idxJoint, header.config[t_header::BIDX], hFP);
			}
			else{
				fread(&b.JointPos[0], sizeof(float), 3, hFP);
			}
			if (b.bone_flag & (t_bone::BIT_GIVE_ROT | t_bone::BIT_GIVE_MOVE)){
				//fread(&b.idxGive, header.config[t_header::BIDX], 1, hFP);
				Read(&b.idxGive, header.config[t_header::BIDX], hFP);
				fread(&b.fGive, sizeof(float), 1, hFP);
			}
			if (b.bone_flag & t_bone::BIT_FIX_AXIS){
				fread(&b.FixAxis, sizeof(float), 3, hFP);
			}

			if (b.bone_flag & t_bone::BIT_LOCAL_AXIS){
				fread(&b.AxisX, sizeof(float), 3, hFP);
				fread(&b.AxisZ, sizeof(float), 3, hFP);
			}

			if (b.bone_flag & t_bone::BIT_EXT_PARENT){
				fread(&b.iExtParent, sizeof(int), 3, hFP);
			}

			if (b.bone_flag & t_bone::BIT_IK){
				ik_count++;
				auto& ik = b.Ik;
				//fread(&ik.idx, header.config[t_header::BIDX], 1, hFP);
				Read(&ik.idx, header.config[t_header::BIDX], hFP);
				fread(&ik.Loop, sizeof(int), 1, hFP);
				fread(&ik.LimitAng, sizeof(float), 1, hFP);
				int iknum;
				fread(&iknum, sizeof(int), 1, hFP);
				ik.IKLink.resize(iknum);
				for (auto& l : ik.IKLink){
					//fread(&l.idxBone, header.config[t_header::BIDX], 1, hFP);
					Read(&l.idxBone, header.config[t_header::BIDX], hFP);
					fread(&l.bLimit, sizeof(char), 1, hFP);
					if (l.bLimit){
						fread(&l.Min[0], sizeof(float), 3, hFP);
						fread(&l.Max[0], sizeof(float), 3, hFP);
					}
				}
			}
		}


		fclose(hFP);

		mLoadResult = true;
	}
	else{
		MessageBox(NULL, "PMX fopen", "Err", MB_ICONSTOP);
		mLoadResult = false;
	}
}

//デストラクタ
pmx::~pmx()
{
	if (!mLoadResult)return;
	delete[] vertex_data.vertex;
	DeleteVoidPointer(&face_vert_index, header.config[t_header::VIDX]);
	delete[] material;
	delete[] bone;
	delete[] textureName;
}