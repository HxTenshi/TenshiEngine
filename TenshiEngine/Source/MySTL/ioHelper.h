#pragma once


#include <stack>
#include <fstream>
#include "Library/picojson.h"

class I_InputHelper;
class I_OutputHelper;

//json読み書き
class I_ioHelper{
protected:
	I_ioHelper(I_InputHelper* ic, I_OutputHelper* oc) :Ic(ic), Oc(oc), error(false){}
	I_InputHelper* Ic;
	I_OutputHelper* Oc;
public:
	virtual ~I_ioHelper(){}

	void pushObject(){
		objects.push(picojson::object());
		o = &objects.top();
	}
	void pushObject(picojson::object& object){
		objects.push(object);
		o = &objects.top();
	}
	picojson::object popObject(){
		auto temp = objects.top();
		objects.pop();
		if (!objects.empty())
			o = &objects.top();
		else
			o = NULL;
		return temp;
	}

	template<class T>
	void func(T&& out, const char* name);

protected:
	std::stack<picojson::object> objects;
	picojson::object* o;
	picojson::value json;
public:
	bool error;
};


//読み込み処理
class I_InputHelper : public I_ioHelper{
protected:
	I_InputHelper() :I_ioHelper(this, NULL)
	{
	}
	virtual ~I_InputHelper(){
	}

public:
	template<class T>
	void _func(T& out, const char* name){
		auto v = o->find(name);
		if (v == o->end())return;
		out = get<T>(v->second);
	}
private:
	template<class T>
	T get(const picojson::value& value);

private:
};
//ファイル読み込み処理
class FileInputHelper : public I_InputHelper{
public:
	FileInputHelper(const std::string& fileName) :I_InputHelper(), jsonfile(fileName)
	{
		if (jsonfile.fail()){
			error = true;
			return;
		}
		jsonfile >> json;
		pushObject(json.get<picojson::object>());
	}
	~FileInputHelper(){
	}
private:
	std::ifstream jsonfile;
};
//メモリ読み込み処理
class MemoryInputHelper : public I_InputHelper{
public:
	MemoryInputHelper(picojson::value& _json) :I_InputHelper()
	{
		pushObject(_json.get<picojson::object>());
	}
	~MemoryInputHelper(){
	}
private:
};

//書き込み処理
class I_OutputHelper : public I_ioHelper{
protected:
	I_OutputHelper() :I_ioHelper(NULL, this)
	{
	}
	virtual ~I_OutputHelper(){
	}
public:
	template<class T>
	void _func(const T& out, const char* name);
private:
};

//ファイル書き込み処理
class FileOutputHelper : public I_OutputHelper{
public:
	FileOutputHelper(const std::string& fileName) :I_OutputHelper(), jsonfile(fileName)
	{
		if (jsonfile.fail()){
			error = true;
			return;
		}
		pushObject();
	}
	~FileOutputHelper(){

		json = picojson::value(popObject());
		jsonfile << json;
	}
private:
	std::ofstream jsonfile;
};
//メモリ書き込み処理
class MemoryOutputHelper : public I_OutputHelper{
public:
	MemoryOutputHelper(picojson::value& _json) :I_OutputHelper(), mOut_json(_json)
	{
		pushObject();
	}
	~MemoryOutputHelper(){

		mOut_json = picojson::value(popObject());
	}
private:
	void operator =(const MemoryOutputHelper&);
	MemoryOutputHelper(const MemoryOutputHelper&);
	picojson::value& mOut_json;
};


template<class T>
void I_ioHelper::func(T&& value, const char* name){
	if (Ic){
		Ic->_func(value, name);
	}
	if (Oc){
		Oc->_func(value, name);
	}
}