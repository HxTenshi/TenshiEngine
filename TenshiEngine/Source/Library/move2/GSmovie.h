/******************************************************************************
* File Name : GSmovie.h                          Ver : 1.00  Date : 2007-10-04
*
* Description :
*
*		ムービー再生 ヘッダファイル．．
*
* Author : Moriharu Ohzu.
*
******************************************************************************/
#ifndef _GSMOVIE_H_
#define _GSMOVIE_H_

/****** 関数プロトタイプ宣言 *************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/*=============================================================================
*
* Purpose : ムービーのファイルの読み込み．
*
* Return  : 読み込みが成功すれば真，失敗すれば偽を返す．
*
*============================================================================*/
extern bool
gsLoadMovie
(
	const char* fileName		/* ファイル名 */
);

/*=============================================================================
*
* Purpose : ムービーの再生．
*
* Return  : なし．
*
*============================================================================*/
extern void
gsPlayMovie
(
	void
);

/*=============================================================================
*
* Purpose : ムービーの更新．
*
* Return  : なし．
*
*============================================================================*/
extern void
gsUpdateMovie
(
	void
);

/*=============================================================================
*
* Purpose : ムービーの描画．
*
* Return  : なし．
*
*============================================================================*/
extern void
gsDrawMovie
(
	void
);

/*=============================================================================
*
* Purpose : ムービーの停止．
*
* Return  : なし．
*
*============================================================================*/
extern void
gsStopMovie
(
	void
);

/*=============================================================================
*
* Purpose : ボリュームの設定．
*
* Return  : なし．
*
*============================================================================*/
extern void
gsSetMovieVolume
(
	float volume		/* ボリューム (0.0（ミュート）〜1.0 （最大）) */
);

/*=============================================================================
*
* Purpose : ボリュームの取得．
*
* Return  : ボリュームを返す． (0.0（ミュート）〜1.0 （最大）)
*
*============================================================================*/
extern float
gsGetMovieVolume
(
	void
);

/*=============================================================================
*
* Purpose : ムービーの高さを取得．
*
* Return  : ムービーの高さを返す．
*
*============================================================================*/
extern unsigned int
gsGetMovieHeight
(
	void
);

/*=============================================================================
*
* Purpose : ムービーの幅を取得．
*
* Return  : ムービーの幅を返す．
*
*============================================================================*/
extern unsigned int
gsGetMovieWidth
(
	void
);

/*=============================================================================
*
* Purpose : ムービーの再生位置を設定．
*
* Return  : なし．
*
*============================================================================*/
extern void
gsSetMovieTime
(
	float time				/* 再生位置 */
);

/*=============================================================================
*
* Purpose : ムービーの再生位置を取得．
*
* Return  : 現在の再生位置を取得．
*
*============================================================================*/
extern float
gsGetMovieTime
(
	void 
);

/*=============================================================================
*
* Purpose : ムービーの終了時間を取得．
*
* Return  : なし．
*
*============================================================================*/
extern float
gsGetMovieEndTime
(
	void
);

/*=============================================================================
*
* Purpose : ムービーの削除．
*
* Return  : なし．
*
*============================================================================*/
extern void
gsDeleteMovie
(
	void
);

#ifdef __cplusplus
}
#endif

//#ifdef	NDEBUG
//# ifdef _MT
//#  ifdef _DLL
//#   pragma comment( lib, "lib/gslibEXT/gslibEXT_md.lib" )
//#  else
//#   pragma comment( lib, "lib/gslibEXT/gslibEXT_mt.lib" )
//#  endif
//# endif
//#else
//# ifdef _MT
//#  ifdef _DLL
//#   pragma comment( lib, "lib/gslibEXT/gslibEXT_mdd.lib" )
//#  else
//#   pragma comment( lib, "lib/gslibEXT/gslibEXT_mtd.lib" )
//#  endif
//# endif
//#endif

#endif

/********** End of File ******************************************************/
