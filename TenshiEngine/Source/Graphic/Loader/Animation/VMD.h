#pragma once

class vmd{
public:
	bool mLoadResult;
#pragma pack(push,1)	//アラインメント制御をオフる

	struct VMDHeader
	{
		char Header[30]; // "Vocaloid Motion Data 0002"
		char ModelName[20];
	};
	VMDHeader mHeader;

	unsigned int mKeyNum;
	struct VMDKeyFrame
	{
		char BoneName[15];
		unsigned int FrameNo;//time
		float Location[3];
		float Rotatation[4]; // Quaternion
		char Interpolation[64]; // [4][4][4]
	};
	VMDKeyFrame* mKeyFrame;

#pragma pack(pop)	//アラインメント制御エンド

	//コンストラクタ
	vmd(const char *);

	//デストラクタ
	~vmd();
};