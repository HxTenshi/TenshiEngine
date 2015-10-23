#ifndef _NXMEMORYSTREAM_H_
#define _NXMEMORYSTREAM_H_
#include <NxPhysics.h>
#include <NxStream.h>
#include <queue>
// メモリストリームクラス
class NxMemoryStream : public NxStream
{
public:
	// １バイトデータの読み込み
	NxU8 readByte() const;
	// ２バイトデータの読み込み
	NxU16 readWord() const;
	// ４バイトデータの読み込み
	NxU32 readDword() const;
	// ｆｌｏａｔデータの読み込み
	float readFloat() const;
	// ｄｏｕｂｌｅバイトデータの読み込み
	double readDouble() const;
	// バッファデータの読み込み
	void readBuffer(void* buffer, NxU32 size) const;
	// １バイトデータの書き込み
	NxStream& storeByte(NxU8 b);
	// ２バイトデータの書き込み
	NxStream& storeWord(NxU16 w);
	// ４バイトデータの書き込み
	NxStream& storeDword(NxU32 d);
	// ｆｌｏａｔデータの書き込み
	NxStream& storeFloat(NxReal f);
	// ｄｏｕｂｌｅバイトデータの書き込み
	NxStream& storeDouble(NxF64 df);
	// バッファデータの書き込み
	NxStream& storeBuffer(const void* buffer, NxU32 size);
private:
	// バッファデータ
	mutable std::queue<NxU8> mBuffer;
};
#endif