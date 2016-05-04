#pragma once

#include "Device/DirectX11Device.h"
#include "MySTL\ptr.h"
#include <functional>
#include "MySTL/Deleter.h"

template<typename T>
class ConstantBuffer;
template<typename T>
class ConstantBufferArray;


class ConstantBufferPtr{
private:

	ConstantBufferPtr()
		: mpBuffer(NULL)
		, mDeleter(NULL)
	{
	}
public:
	~ConstantBufferPtr()
	{
		if (mpBuffer) mpBuffer->Release();
		if (mDeleter) delete mDeleter;
	}

	typedef shared_ptr<ConstantBufferPtr> create_type;
	static
	create_type create(UINT ByteWidth)
	{

		if (ByteWidth%16){
			_ASSERT("16‚Ì”{”‚¶‚á‚È‚¢‚Æƒ_ƒ");
		}

		create_type temp(new ConstantBufferPtr());

		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));

		// Create the constant buffers
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = ByteWidth;//sizeof(T);
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = 0;

		HRESULT hr = Device::mpd3dDevice->CreateBuffer(&bd, NULL, &temp->mpBuffer);
		if (FAILED(hr))
			return create_type();

		return temp;
	}

	static
	create_type create(UINT ByteWidth, DeleterBase* Deleter)
	{
		
		create_type temp = create(ByteWidth);

		temp->mDeleter = Deleter;

		return temp;
	}

	ID3D11Buffer* GetBufferPtr(){
		return mpBuffer;
	}

private:
	ID3D11Buffer*   mpBuffer;

	template<typename T>
	friend class ConstantBuffer;
	template<typename T>
	friend class ConstantBufferArray;


	DeleterBase* mDeleter;
};

//class ConstantBufferInterface{
//	virtual ~ConstantBufferInterface();
//	virtual void VSSetConstantBuffers();
//	virtual void PSSetConstantBuffers();
//};

template<typename T>
class ConstantBuffer{
public:
	ConstantBuffer<T>()
		: mSlot(UINT_MAX)
		, mBuffer(shared_ptr<ConstantBufferPtr>())
	{
	}
private:
	ConstantBuffer<T>(UINT Slot, shared_ptr<ConstantBufferPtr> const& pbuf)
		: mSlot(Slot)
		, mBuffer(pbuf)
	{
	}
public:
	virtual ~ConstantBuffer()
	{
	}

	typedef ConstantBuffer<T> create_type;
	static
	create_type create(UINT Slot)
	{

		create_type temp(Slot, ConstantBufferPtr::create(sizeof(T)));

		if (!temp.mBuffer)
			return create_type();

		return temp;
	}

	create_type Duplicate(UINT Slot = UINT_MAX) const
	{
		if (Slot == UINT_MAX)Slot = mSlot;
		create_type temp(Slot, mBuffer);

		return temp;
	}

	void UpdateSubresource(ID3D11DeviceContext* context) const{

		if (mBuffer->mpBuffer)context->UpdateSubresource(mBuffer->mpBuffer, 0, NULL, &mParam, 0, 0);
	}

	void VSSetConstantBuffers(ID3D11DeviceContext* context) const{

		if (mBuffer->mpBuffer)context->VSSetConstantBuffers(mSlot, 1, &mBuffer->mpBuffer);
	}
	void GSSetConstantBuffers(ID3D11DeviceContext* context) const{

		if (mBuffer->mpBuffer)context->GSSetConstantBuffers(mSlot, 1, &mBuffer->mpBuffer);
	}
	void CSSetConstantBuffers(ID3D11DeviceContext* context) const{

		if (mBuffer->mpBuffer)context->CSSetConstantBuffers(mSlot, 1, &mBuffer->mpBuffer);
	}
	void PSSetConstantBuffers(ID3D11DeviceContext* context) const{

		if (mBuffer->mpBuffer)context->PSSetConstantBuffers(mSlot, 1, &mBuffer->mpBuffer);
	}

public:
	UINT mSlot;
	T mParam;
	shared_ptr<ConstantBufferPtr> mBuffer;
private:
};

template<typename T>
class ConstantBufferArray{
public:
	ConstantBufferArray<T>()
		: mSlot(UINT_MAX)
		, mParam(NULL)
		, mArraySize(0)
		, mBuffer(shared_ptr<ConstantBufferPtr>())
	{
	}
private:
	ConstantBufferArray<T>(UINT Slot, UINT ArrayNum)
		: mSlot(Slot)
		, mArraySize(ArrayNum)
	{
		mParam = new T[mArraySize];
		mBuffer = ConstantBufferPtr::create((UINT)(sizeof(T)*mArraySize), new Deleter<T>(mParam, true));
	}

public:

	void DeleteParam(){
		delete[] mParam;
	}

	virtual ~ConstantBufferArray()
	{
	}

	typedef ConstantBufferArray<T> create_type;
	static
	create_type create(UINT Slot, UINT ArrayNum)
	{
		create_type temp(Slot, ArrayNum);

		if (!temp.mBuffer)
			return create_type();

		return temp;
	}

	//create_type Duplicate(UINT Slot = UINT_MAX) const
	//{
	//	if (Slot == UINT_MAX)Slot = mSlot;
	//	create_type temp(Slot, mBuffer);
	//
	//	return temp;
	//}

	void UpdateSubresource(ID3D11DeviceContext* context) const{
		if (mBuffer->mpBuffer)context->UpdateSubresource(mBuffer->mpBuffer, 0, NULL, mParam, 0, 0);
	}

	void VSSetConstantBuffers(ID3D11DeviceContext* context) const{
		if (mBuffer->mpBuffer)context->VSSetConstantBuffers(mSlot, 1, &mBuffer->mpBuffer);
	}
	void PSSetConstantBuffers(ID3D11DeviceContext* context) const{
		if (mBuffer->mpBuffer)context->PSSetConstantBuffers(mSlot, 1, &mBuffer->mpBuffer);
	}

public:
	UINT mSlot;
	T* mParam;
	UINT mArraySize;
	shared_ptr<ConstantBufferPtr> mBuffer;
private:
};