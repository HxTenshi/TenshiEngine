#include "VMD.h"
#include <windows.h>
#include <stdio.h>

//コンストラクタ
vmd::vmd(const char * filename){


	FILE *hFP;
	hFP = fopen(filename, "rb");
	if (hFP != 0){
		//ヘッダー読み込み
		fread(&mHeader, sizeof(VMDHeader), 1, hFP);

		fread(&mKeyNum, sizeof(mKeyNum), 1, hFP);
		mKeyFrame = new VMDKeyFrame[mKeyNum];
		fread(mKeyFrame, sizeof(VMDKeyFrame), mKeyNum, hFP);

		fclose(hFP);
		mLoadResult = true;
	}
	else{
		//MessageBox(NULL, "VMD fopen", "Err", MB_ICONSTOP);
		mLoadResult = false;
	}
}

//デストラクタ
vmd::~vmd(){
	if (!mLoadResult)return;
	delete[] mKeyFrame;
}