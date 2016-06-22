
#pragma once

#include "Common.h"
#include "AnimationLinear.h"
#include "MovieTexture.h"


class	SampleMovie
{
// メンバ変数
private:
	// モデルの頂点バッファ
	LPDIRECT3DVERTEXBUFFER9	m_vb;

	// テクスチャ画像バッファ
	LPDIRECT3DTEXTURE9		m_tex;

	// 頂点バッファの構造体（テクスチャ座標つき）
	struct  MY_VERTEX_VDT{
		D3DXVECTOR3 p;      // 位置
		DWORD       color;  // 色
		D3DXVECTOR2 t;      // テクスチャー座標
	};
	 
	// 頂点バッファーのフォーマットの定義
	enum{ MY_VERTEX_VDT_FVF = (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1) };
	
	// アニメーション
	AnimationLinear		m_anim_rot;		// 液晶回転

	// ◆動画テクスチャ
	MovieTexture		m_movie_tex;


// コンストラクタ・デストラクタ
public:
	SampleMovie();
	~SampleMovie();


// メンバ関数
public:
	HRESULT	Create( LPDIRECT3DDEVICE9 d3d_dev );	// シーンの生成
	HRESULT	Destroy( void );						// シーンの破棄
	HRESULT	Transform( LPDIRECT3DDEVICE9 d3d_dev );	// 座標変換処理
	HRESULT	Draw( LPDIRECT3DDEVICE9 d3d_dev );		// シーンの描画
};