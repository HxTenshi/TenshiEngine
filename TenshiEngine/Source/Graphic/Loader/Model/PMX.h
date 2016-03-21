#pragma once

#include <string>

#include <vector>

class pmx{
public:
	void Read(int* p, unsigned char size, FILE* fp);
	bool mLoadResult;
#pragma pack(push,1)	//アラインメント制御をオフる
	//ヘッダー
	struct t_header{
		unsigned char magic[4];
		float version;
		unsigned char configNum;
		unsigned char config[8];
		enum{
			ENCODE = 0,	//[0] - エンコード方式  | 0:UTF16 1:UTF8
			ADDUV,		//[1] - 追加UV数 	| 0～4 詳細は頂点参照
			VIDX,		//[2] - 頂点Indexサイズ | 1,2,4 のいずれか
			TIDX,		//[3] - テクスチャIndexサイズ | 1,2,4 のいずれか
			MIDX,		//[4] - 材質Indexサイズ | 1,2,4 のいずれか
			BIDX,		//[5] - ボーンIndexサイズ | 1,2,4 のいずれか
			PIDX,		//[6] - モーフIndexサイズ | 1,2,4 のいずれか
			RIDX,		//[7] - 剛体Indexサイズ | 1,2,4 のいずれか
		};
		enum {
			ENCODE_UTF16 = 0,
			ENCODE_UTF8 = 1,
		};
	};
	t_header header;
	struct t_model_info{
		std::string modelName;
		std::string modelNameEn;
		std::string modelInfo;
		std::string modelInfoEn;
	};
	t_model_info modelInfo;

	void LoadText(FILE* fp, std::string& str);

	//頂点リスト
	int vert_count;

	template<class SizeType>
	struct t_bone_BDEF1
	{
		SizeType bone_num[1];

	};
	template<class SizeType>
	struct t_bone_BDEF2
	{
		SizeType bone_num[2];
		float bone_weight[1];
	};
	template<class SizeType>
	struct t_bone_BDEF4
	{
		SizeType bone_num[4];
		float bone_weight[4];
	};
	template<class SizeType>
	struct t_bone_SDEF
	{
		SizeType bone_num[2];
		float bone_weight;
		float C[3];
		float R0[3];
		float R1[3];
	};

	struct t_vertex_data{
		enum {
			ADDUV_MAX = 4,
			BDEF1 = 0,
			BDEF2 = 1,
			BDEF4 = 2,
			SDEF = 3,
			QDEF = 4,
		};
		struct t_vertex{
			struct t_v{
				float pos[3];
				float normal_vec[3];
				float uv[2];
			};
			t_v v;

			float avAdd[4][4];

			unsigned char weight_type;
			int bone_num[4];
			float bone_weight[4];
			//SDEF 
			float C[3];
			float R0[3];
			float R1[3];

			float edge_flag;
		};
		t_vertex *vertex;
	};
	t_vertex_data vertex_data;

	void NewVoidPointer(void** p, int newNum, unsigned char size);
	void DeleteVoidPointer(void** p, unsigned char size);
	
	//面頂点リスト
	int face_vert_count;
	void *face_vert_index;

	//テクスチャリスト
	int texture_count;
	std::string* textureName;

	//Material
	int material_count;
	struct t_material{
		std::string MaterialName;
		std::string MaterialNameEn;
		struct t_m{
			float Diffuse[4];// 減衰色
			float Specular[3];//	光沢色
			float Specularity;
			float Ambient[3];//	環境色
			unsigned char bitFlag;// 描画フラグ(8bit) - 各bit 0:OFF 1 : ON,0x01 : 両面描画, 0x02 : 地面影, 0x04 : セルフシャドウマップへの描画, 0x08 : セルフシャドウの描画,0x10 : エッジ描画

			float EdgeColor[4];
			float EdgeSize;
		} material;

		int TextureIdx;//テクスチャIndexサイズ | 通常テクスチャ, テクスチャテーブルの参照Index
		int SphereIdx;//テクスチャIndexサイズ | スフィアテクスチャ, テクスチャテーブルの参照Index  ※テクスチャ拡張子の制限なし

		unsigned char SphereMode;// | スフィアモード 0 : 無効 1 : 乗算(sph) 2 : 加算(spa) 3 : サブテクスチャ(追加UV1のx, yをUV参照して通常テクスチャ描画を行う)
		unsigned char ToonMode;// 共有Toonフラグ 0 : 継続値は個別Toon 1 : 継続値は共有Toon

		int ToonIdx;//共有Toonフラグ:0 の場合 テクスチャIndexサイズ | Toonテクスチャ, テクスチャテーブルの参照Index 共有Toonフラグ: 1 の場合 共有Toonテクスチャ[0～9]->それぞれ toon01.bmp～toon10.bmp に対応

		std::string memo;
		int face_vert_count;//	この材質で使う面頂点リストのデータ数

	};
	t_material* material;// 材質データ

	struct IkLink
	{
		int idxBone;
		unsigned char bLimit;
		float Min[3];//bLimit=1
		float Max[3];//bLimit=1
	};
	struct t_ik_data{
		int idx;// ボーンIndexサイズ | IKターゲットボーンのボーンIndex;
		int Loop;
		float LimitAng;
		std::vector<IkLink> IKLink;
	};
	int bone_count;
	struct t_bone{
		enum{
			BIT_JOINT = 0x0001,			// 接続先(PMD子ボーン指定)表示方法 -> 0:座標オフセットで指定 1:ボーンで指定
			BIT_ROT = 0x0002,			// 回転可能
			BIT_MOVE = 0x0004,			// 移動可能
			BIT_VISIBLE = 0x0008,		// 表示
			BIT_MANIP = 0x0010,			// 操作可
			BIT_IK = 0x0020,			// IK
			BIT_GIVE_LOCAL = 0x0080,	// ローカル付与 | 付与対象 0:ユーザー変形値／IKリンク／多重付与 1:親のローカル変形量
			BIT_GIVE_ROT = 0x0100,		// 回転付与
			BIT_GIVE_MOVE = 0x0200,		// 移動付与
			BIT_FIX_AXIS = 0x0400,		// 軸固定
			BIT_LOCAL_AXIS = 0x0800,	// ローカル軸
			BIT_POST_PHYSICS = 0x1000,	// 物理後変形
			BIT_EXT_PARENT = 0x2000,	// 外部親変形
		};
		std::string BoneName;
		std::string BoneNameEn;
		float bone_head_pos[3]; // x, y, z // ボーンのヘッドの位置
		int parent_bidx;	//ボーンIndexサイズ 親ボーン番号(ない場合は0xFFFF)
		int Hierarchy;//変形階層
		unsigned short bone_flag;		// ボーンの種類

		float JointPos[3];	//BIT_JOINT=0
		int idxJoint;		//BIT_JOINT=1
		int idxGive;		//BIT_GIVE_ROT=1 || BIT_GIVE_MOVE=1
		float fGive;		//BIT_GIVE_ROT=1 || BIT_GIVE_MOVE=1
		float FixAxis[3];	//BIT_FIX_AXIS=1
		float AxisX[3];	//BIT_LOCAL_AXIS=1
		float AxisZ[3];	//BIT_LOCAL_AXIS=1
		int iExtParent;	//BIT_EXT_PARENT=1
		t_ik_data Ik;
	};
	t_bone* bone;

	unsigned long ik_count;


#pragma pack(pop)	//アラインメント制御エンド

	//コンストラクタ
	pmx(const char *);

	//デストラクタ
	~pmx();
};