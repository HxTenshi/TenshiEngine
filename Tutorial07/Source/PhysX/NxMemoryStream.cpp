#include "NxMemoryStream.h"
// １バイトデータの読み込み
NxU8 NxMemoryStream::readByte() const
{
	NxU8 buffer;
	readBuffer(&buffer, sizeof(buffer));
	return buffer;
}
// ２バイトデータの読み込み
NxU16 NxMemoryStream::readWord() const
{
	NxU16 buffer;
	readBuffer(&buffer, sizeof(buffer));
	return buffer;
}
// ４バイトデータの読み込み
NxU32 NxMemoryStream::readDword() const
{
	NxU32 buffer;
	readBuffer(&buffer, sizeof(buffer));
	return buffer;
}
// ｆｌｏａｔデータの読み込み
float NxMemoryStream::readFloat() const
{
	float buffer;
	readBuffer(&buffer, sizeof(buffer));
	return buffer;
}
// ｄｏｕｂｌｅバイトデータの読み込み
double NxMemoryStream::readDouble() const
{
	double buffer;
	readBuffer(&buffer, sizeof(buffer));
	return buffer;
}
// バッファデータの読み込み
void NxMemoryStream::readBuffer(void* buffer, NxU32 size) const
{
	for (NxU32 i = 0; i < size; i++) {
		static_cast<NxU8*>(buffer)[i] = mBuffer.front(); // キューの先頭からデータを取り出す
		mBuffer.pop(); // キューの先頭を削除
	}
}
// １バイトデータの書き込み
NxStream& NxMemoryStream::storeByte(NxU8 b)
{
	storeBuffer(&b, sizeof(b));
	return *this;
}
// ２バイトデータの書き込み
NxStream& NxMemoryStream::storeWord(NxU16 w)
{
	storeBuffer(&w, sizeof(w));
	return *this;
}
// ４バイトデータの書き込み
NxStream& NxMemoryStream::storeDword(NxU32 d)
{
	storeBuffer(&d, sizeof(d));
	return *this;
}
// ｆｌｏａｔデータの書き込み
NxStream& NxMemoryStream::storeFloat(NxReal f)
{
	storeBuffer(&f, sizeof(f));
	return *this;
}
// ｄｏｕｂｌｅバイトデータの書き込み
NxStream& NxMemoryStream::storeDouble(NxF64 df)
{
	storeBuffer(&df, sizeof(df));
	return *this;
}
// バッファデータの書き込み
NxStream& NxMemoryStream::storeBuffer(const void* buffer, NxU32 size)
{
	for (NxU32 i = 0; i < size; i++) {
		mBuffer.push(static_cast<const NxU8*>(buffer)[i]); // キューの末尾にデータを追加
	}
	return *this;
}