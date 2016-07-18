/******************************************************************************
* File Name : GSmovie.h                          Ver : 1.00  Date : 2007-10-04
*
* Description :
*
*		���[�r�[�Đ� �w�b�_�t�@�C���D�D
*
* Author : Moriharu Ohzu.
*
******************************************************************************/
#ifndef _GSMOVIE_H_
#define _GSMOVIE_H_

/****** �֐��v���g�^�C�v�錾 *************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/*=============================================================================
*
* Purpose : ���[�r�[�̃t�@�C���̓ǂݍ��݁D
*
* Return  : �ǂݍ��݂���������ΐ^�C���s����΋U��Ԃ��D
*
*============================================================================*/
extern bool
gsLoadMovie
(
	const char* fileName		/* �t�@�C���� */
);

/*=============================================================================
*
* Purpose : ���[�r�[�̍Đ��D
*
* Return  : �Ȃ��D
*
*============================================================================*/
extern void
gsPlayMovie
(
	void
);

/*=============================================================================
*
* Purpose : ���[�r�[�̍X�V�D
*
* Return  : �Ȃ��D
*
*============================================================================*/
extern void
gsUpdateMovie
(
	void
);

/*=============================================================================
*
* Purpose : ���[�r�[�̕`��D
*
* Return  : �Ȃ��D
*
*============================================================================*/
extern void
gsDrawMovie
(
	void
);

/*=============================================================================
*
* Purpose : ���[�r�[�̒�~�D
*
* Return  : �Ȃ��D
*
*============================================================================*/
extern void
gsStopMovie
(
	void
);

/*=============================================================================
*
* Purpose : �{�����[���̐ݒ�D
*
* Return  : �Ȃ��D
*
*============================================================================*/
extern void
gsSetMovieVolume
(
	float volume		/* �{�����[�� (0.0�i�~���[�g�j�`1.0 �i�ő�j) */
);

/*=============================================================================
*
* Purpose : �{�����[���̎擾�D
*
* Return  : �{�����[����Ԃ��D (0.0�i�~���[�g�j�`1.0 �i�ő�j)
*
*============================================================================*/
extern float
gsGetMovieVolume
(
	void
);

/*=============================================================================
*
* Purpose : ���[�r�[�̍������擾�D
*
* Return  : ���[�r�[�̍�����Ԃ��D
*
*============================================================================*/
extern unsigned int
gsGetMovieHeight
(
	void
);

/*=============================================================================
*
* Purpose : ���[�r�[�̕����擾�D
*
* Return  : ���[�r�[�̕���Ԃ��D
*
*============================================================================*/
extern unsigned int
gsGetMovieWidth
(
	void
);

/*=============================================================================
*
* Purpose : ���[�r�[�̍Đ��ʒu��ݒ�D
*
* Return  : �Ȃ��D
*
*============================================================================*/
extern void
gsSetMovieTime
(
	float time				/* �Đ��ʒu */
);

/*=============================================================================
*
* Purpose : ���[�r�[�̍Đ��ʒu���擾�D
*
* Return  : ���݂̍Đ��ʒu���擾�D
*
*============================================================================*/
extern float
gsGetMovieTime
(
	void 
);

/*=============================================================================
*
* Purpose : ���[�r�[�̏I�����Ԃ��擾�D
*
* Return  : �Ȃ��D
*
*============================================================================*/
extern float
gsGetMovieEndTime
(
	void
);

/*=============================================================================
*
* Purpose : ���[�r�[�̍폜�D
*
* Return  : �Ȃ��D
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