#pragma once

class pmd{
public:
	bool mLoadResult;
#pragma pack(push,1)	//アラインメント制御をオフる
	//ヘッダー
	struct t_header{
		unsigned char magic[3];
		float version;
		unsigned char model_name[20];
		unsigned char comment[256];
	};
	t_header header;
	//頂点リスト
	unsigned long vert_count;
	struct t_vertex{
		float pos[3];
		float normal_vec[3];
		float uv[2];
		unsigned short bone_num[2];
		unsigned char bone_weight;
		unsigned char edge_flag;
	};
	t_vertex *vertex;
	//面頂点リスト
	unsigned long face_vert_count;
	unsigned short *face_vert_index;

	unsigned long material_count;
	struct t_material{
		float diffuse_color[3];// 減衰色
		float alpha;//	減衰色の不透明度
		float specularity;
		float specular_color[3];//	光沢色
		float mirror_color[3];//	環境色
		unsigned char toon_index;
		unsigned char edge_flag;//	輪郭フラグ
		unsigned long face_vert_count;//	この材質で使う面頂点リストのデータ数
		char texture_file_name[20];//	テクスチャファイル名[20] + %0
	};
	t_material* material;// 材質データ

	unsigned short bone_count;
	struct t_bone{
		char bone_name[20];	// ボーン名
		unsigned short parent_bidx;	// 親ボーン番号(ない場合は0xFFFF)
		unsigned short tail_pos_bidx; // tail位置のボーン番号(チェーン末端の場合は0xFFFF) // 親：子は1：多なので、主に位置決め用
		unsigned char bone_type;		// ボーンの種類
		unsigned short ik_parent_bidx;// IKボーン番号(影響IKボーン。ない場合は0)
		float bone_head_pos[3]; // x, y, z // ボーンのヘッドの位置
	};
	t_bone* bone;
	//  ・ボーンの種類
	//	0:回転 1 : 回転と移動 2 : IK 3 : 不明 4 : IK影響下 5 : 回転影響下 6 : IK接続先 7 : 非表示
	//	・ボーンの種類(MMD 4.0～)
	//	8 : 捻り 9 : 回転運動

	unsigned short ik_data_count; // IKデータ数
	struct t_ik_data{
		unsigned short ik_bone_index; // IKボーン番号
		unsigned short ik_target_bone_index; // IKターゲットボーン番号 // IKボーンが最初に接続するボーン
		unsigned char ik_chain_length; // IKチェーンの長さ(子の数)
		unsigned short iterations; // 再帰演算回数 // IK値1
		float control_weight; // 演算1回あたりの制限角度 // IK値2
		unsigned short* ik_child_bone_index; //[ik_chain_length] IK影響下のボーン番号
	};
	t_ik_data* ik_data; // IKデータ((11+2*ik_chain_length)/IK)


	//表情リスト
	unsigned short skin_count; // 表情数
	struct t_skin_vert_data{//(type：base)
		unsigned long skin_vert_index; // 表情用の頂点の番号(頂点リストにある番号)
		float skin_vert_pos[3]; // x, y, z // 表情用の頂点の座標(頂点自体の座標)
	};
	//・t_skin_vert_data(type：base以外)
	//	DWORD base_skin_vert_index; // 表情用の頂点の番号(baseの番号。skin_vert_index)
	//float skin_vert_pos_offset[3]; // x, y, z // 表情用の頂点の座標オフセット値(baseに対するオフセット)
	struct t_skin_data{
		;
		char skin_name[20]; //　表情名
		unsigned long skin_vert_count; // 表情用の頂点数
		unsigned short skin_type; // 表情の種類 // 0：base、1：まゆ、2：目、3：リップ、4：その他
		t_skin_vert_data* skin_vert_data; // 表情用の頂点のデータ(16Bytes/vert)
	};
	t_skin_data* skin_data; // 表情データ((25+16*skin_vert_count)/skin)


	//表情枠用表示リスト
	unsigned char skin_disp_count; // 表情枠に表示する表情数
	unsigned short* skin_index; // 表情番号


	//ボーン枠用枠名リスト
	unsigned char bone_disp_name_count; // ボーン枠用の枠名数 // センター(1番上に表示される枠)は含まない
	char* disp_name[50]; // 枠名(50Bytes/枠)

	//補足：
	//	PMDeditorを使う場合は、枠名を0x0A00で終わらせる必要があります。(0x00のみだと表示されません)→0.0.4.2cで確認。
	//	MMDでは文字列終端は0x00のみでも表示可能です。→6.08で確認。(付属モデルは0x0A00で終端しているようです)



	//ボーン枠用表示リスト
	unsigned long bone_disp_count; // ボーン枠に表示するボーン数 (枠0(センター)を除く、すべてのボーン枠の合計)
	struct t_bone_disp{
		unsigned short bone_index; // 枠用ボーン番号
		unsigned char bone_disp_frame_index; // 表示枠番号 // センター:00 他の表示枠:01～ // センター枠にセンター(index 0)以外のボーンを指定しても表示されません。
	};
	t_bone_disp* bone_disp; // 枠用ボーンデータ (3Bytes/bone)







	char toon_file_name[10][100];
#pragma pack(pop)	//アラインメント制御エンド

	//コンストラクタ
	pmd(const char *);

	//デストラクタ
	~pmd();
};