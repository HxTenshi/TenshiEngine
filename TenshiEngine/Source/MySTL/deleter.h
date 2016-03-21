#pragma once

class DeleterBase{
public:
	virtual ~DeleterBase(){}
};
template<typename T>
class Deleter :public DeleterBase{
public:
	Deleter(T* pObj, bool IsArray) :mpObj(pObj), mIsArray(IsArray){}
	~Deleter(){
		if (mIsArray)
			delete[] mpObj;
		else
			delete mpObj;
	}

private:

	T* mpObj;
	bool mIsArray;
};