
#include "SampleMovie.h"


//-------------------------------------------------------------
// デフォルトコンストラクタ
//-------------------------------------------------------------
SampleMovie::SampleMovie()
	: m_vb( NULL )
	, m_tex( NULL )
{
}



//-------------------------------------------------------------
// デストラクタ
//-------------------------------------------------------------
SampleMovie::~SampleMovie()
{
	Destroy();
}



//----------------------------------------------------------
// シーンの生成
//----------------------------------------------------------
HRESULT SampleMovie::Create( LPDIRECT3DDEVICE9 d3d_dev )
{
	Destroy();
	if( d3d_dev == NULL )	return	E_FAIL;

	//----------------------------------------------------------
	// ■モデルの生成
	//----------------------------------------------------------
	// モデルの頂点バッファを生成
	if( FAILED( d3d_dev->CreateVertexBuffer(
			4 * sizeof(MY_VERTEX_VDT),  // 頂点数
			D3DUSAGE_WRITEONLY,
			MY_VERTEX_VDT_FVF,          // 頂点データ定数値
			D3DPOOL_MANAGED,
			&m_vb,                      // 頂点バッファポインタへのアドレス
			NULL ) ) )
	{
		return  E_FAIL;
	}

	// 頂点バッファ内のデータを設定
	MY_VERTEX_VDT   *v;

	// 設定を開始
	m_vb->Lock( 0, 0, (void**)&v, 0 );
	 
	// 各頂点の位置
	v[0].p = D3DXVECTOR3( -1.5f,  1.5f,  0.0f );
	v[1].p = D3DXVECTOR3(  1.5f,  1.5f,  0.0f );
	v[2].p = D3DXVECTOR3( -1.5f, -1.5f,  0.0f );
	v[3].p = D3DXVECTOR3(  1.5f, -1.5f,  0.0f );
	 
	// 各頂点の色
	v[0].color = v[1].color = v[2].color = v[3].color 
		= D3DXCOLOR( 0.0f, 0.0f, 0.0f, 1.0f );
	 
	// テクスチャ座標の設定
	v[0].t = D3DXVECTOR2( 0.0f, 0.0f );
	v[1].t = D3DXVECTOR2( 1.0f, 0.0f );
	v[2].t = D3DXVECTOR2( 0.0f, 1.0f );
	v[3].t = D3DXVECTOR2( 1.0f, 1.0f );

	// 設定を終了
	m_vb->Unlock();

	//----------------------------------------------------------
	// ■テクスチャの生成
	//----------------------------------------------------------
	// ◆動画テクスチャ画像の読み込み
	m_movie_tex.Create( d3d_dev, L"movie/E_Map_TYPE01a.avi", true, true, true );

	//----------------------------------------------------------
	// ■アニメーションの設定
	//----------------------------------------------------------
	// 液晶回転
	m_anim_rot.CreateAnimation( 3, timeGetTime(), true );
	m_anim_rot.SetKeyValue( 0, 0, -45.0f );
	m_anim_rot.SetKeyValue( 1, 2000, 45.0f );
	m_anim_rot.SetKeyValue( 2, 4000, -45.0f );


	return	S_OK;
}



//-------------------------------------------------------------
// シーンの破棄
//-------------------------------------------------------------
HRESULT	SampleMovie::Destroy()
{
	// 頂点バッファの解放
	SAFE_RELEASE( m_vb );
	SAFE_RELEASE( m_tex );

	return	S_OK;
}



//-------------------------------------------------------------
// 座標変換
//-------------------------------------------------------------
HRESULT	SampleMovie::Transform( LPDIRECT3DDEVICE9 d3d_dev )
{
	// モデルの配置
	D3DXMATRIX  mat_world;
	D3DXMATRIX  mat_trans, mat_scale, mat_rot_y;
	D3DXMATRIX  mat_mul;

	float	angle	= D3DXToRadian( m_anim_rot.GetValue( timeGetTime() ) );

	D3DXMatrixIdentity( &mat_world );
	D3DXMatrixTranslation( &mat_trans, 0.0f, 0.0f, 0.0f );
	D3DXMatrixScaling( &mat_scale, 1.6f, 0.9f, 1.0f );
	D3DXMatrixRotationY( &mat_rot_y, angle );
	D3DXMatrixMultiply( &mat_mul, &mat_scale, &mat_rot_y );
	D3DXMatrixMultiply( &mat_world, &mat_mul, &mat_trans );

	d3d_dev->SetTransform( D3DTS_WORLD, &mat_world );

	return	S_OK;
}



//-------------------------------------------------------------
// オブジェクト等の描画
//-------------------------------------------------------------
HRESULT	SampleMovie::Draw( LPDIRECT3DDEVICE9 d3d_dev )
{
	if( d3d_dev == NULL )	return	E_FAIL;
	if( m_vb == NULL )		return	E_FAIL;

	// テクスチャの設定
	// テクスチャステージの設定 
	d3d_dev->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
	d3d_dev->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    d3d_dev->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );

	// 座標変換
	Transform( d3d_dev );

	// ◆作成したテクスチャをパイプラインにセット
	d3d_dev->SetTexture( 0, m_movie_tex.GetTexture() );

	// 頂点バッファの描画
	d3d_dev->SetStreamSource( 0, m_vb, 0, sizeof(MY_VERTEX_VDT) );
	d3d_dev->SetFVF( MY_VERTEX_VDT_FVF );
	d3d_dev->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );

	// テクスチャ情報をリセット
	d3d_dev->SetTexture( 0, NULL );

	return	S_OK;
}
