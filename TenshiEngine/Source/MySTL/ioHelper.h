#pragma once


#include <stack>
#include <fstream>
#include "Library/picojson.h"
#include "MySTL/Utility.h"

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

	virtual void pushObject(const std::string& key){ (void)key; }
	virtual void popObject(){}

	template<class T>
	bool func(T&& out, const char* name, bool compel = false);

	bool isInput(){ return Ic != NULL; }

protected:
	std::stack<std::pair<std::string,picojson::object>> objects;
	picojson::object* o;
	picojson::value json;
public:
	bool error;
};


//読み込み処理
class I_InputHelper : public I_ioHelper{

public:
	virtual ~I_InputHelper(){
	}

	void pushObject(const std::string& key) override{
		picojson::object temp;
		func(temp, key.c_str());
		objects.push(std::make_pair(key, temp));
		o = &objects.top().second;

		if (prefab){
			prefab->pushObject(key);
		}

	}
	void popObject() override{
		objects.pop();
		if (!objects.empty())
			o = &objects.top().second;
		else
			o = NULL;

		if (prefab){
			prefab->popObject();
		}
	}

protected:
	I_InputHelper(I_InputHelper* _prefab) :I_ioHelper(this, NULL), prefab(_prefab)
	{
	}

public:
	template<class T>
	bool _func(T& out, const char* name){
		auto v = o->find(name);
		if (v == o->end())return false;

		T temp;
		if (prefab){
			if (!prefab->func(temp, name)){
				return false;
			}
		}

		out = get<T>(v->second);
		return true;
	}
private:
	template<class T>
	T get(const picojson::value& value);

private:

	I_InputHelper* prefab;
};
//ファイル読み込み処理
class FileInputHelper : public I_InputHelper{
public:
	FileInputHelper(const std::string& fileName, I_InputHelper* _prefab) :I_InputHelper(_prefab), jsonfile(fileName)
	{
		if (jsonfile.fail()){
			error = true;
			return;
		}
		jsonfile >> json;

		objects.push(std::make_pair("", json.get<picojson::object>()));
		o = &objects.top().second;
	}
	~FileInputHelper(){
	}
private:
	std::ifstream jsonfile;
};
//メモリ読み込み処理
class MemoryInputHelper : public I_InputHelper{
public:
	MemoryInputHelper(picojson::value& _json, I_InputHelper* _prefab) :I_InputHelper(_prefab)
	{
		json = _json;
		objects.push(std::make_pair("", _json.get<picojson::object>()));
		o = &objects.top().second;
	}
	~MemoryInputHelper(){
	}
private:
};

//書き込み処理
class I_OutputHelper : public I_ioHelper{
public:
	virtual ~I_OutputHelper(){
	}

	void pushObject(const std::string& key) override{
		objects.push(std::make_pair(key, picojson::object()));
		o = &objects.top().second;

		if (prefab){
			prefab->pushObject(key);
		}

	}
	void popObject() override{
		auto temp = objects.top();
		objects.pop();
		if (!objects.empty())
			o = &objects.top().second;
		else
			o = NULL;

		func(temp.second, temp.first.c_str());

		if (prefab){
			prefab->popObject();
		}
	}
protected:
	I_OutputHelper(I_InputHelper* _prefab, bool OutputFilterRebirth = false) :I_ioHelper(NULL, this), prefab(_prefab), mOutputFilterRebirth(OutputFilterRebirth)
	{
	}
public:
	template<class T>
	void _func(const T& out, const char* name, bool compel){

		T temp;
		if (mOutputFilterRebirth){
			if (compel)return;
			if (prefab && prefab->func(temp, name)){
				if (temp != out)return;
			}
		}
		else{
			if (!compel && prefab && prefab->func(temp, name)){
				if (temp == out)return;
			}
		}
		_func_out(out,name);
	}
	template<class T>
	void _func_out(const T& out, const char* name);
private:

	I_InputHelper* prefab;

	bool mOutputFilterRebirth;
};

//ファイル書き込み処理
class FileOutputHelper : public I_OutputHelper{
public:
	FileOutputHelper(const std::string& fileName, I_InputHelper* _prefab) :I_OutputHelper(_prefab), jsonfile(fileName)
	{
		if (jsonfile.fail()){
			//ディレクトリの作成
			{
				std::string path = "";
				auto fullpath = fileName;
				bool roop = true;
				while (roop){
					auto dir = forward_than_find_first_of(fullpath, "/");
					if (dir == "")break;
					if (path != "")path += "\\";
					path += dir;
					fullpath.erase(0, dir.length() + 1);

					if (CreateDirectory(path.c_str(), NULL)){
					}
				}
			}
			jsonfile.open(fileName);
			if (jsonfile.fail()){
				error = true;
				return;
			}
		}
		pushObject("");
		if (_prefab)_prefab->popObject();
	}
	~FileOutputHelper(){

		json = picojson::value(objects.top().second);
		jsonfile << json;
	}
private:
	std::ofstream jsonfile;
};
//メモリ書き込み処理
class MemoryOutputHelper : public I_OutputHelper{
public:
	MemoryOutputHelper(picojson::value& _json, I_InputHelper* _prefab, bool OutputFilterRebirth = false) :I_OutputHelper(_prefab, OutputFilterRebirth), mOut_json(_json)
	{
		pushObject("");
		if (_prefab)_prefab->popObject();
	}
	~MemoryOutputHelper(){

		mOut_json = picojson::value(objects.top().second);
	}
private:
	void operator =(const MemoryOutputHelper&);
	MemoryOutputHelper(const MemoryOutputHelper&);
	picojson::value& mOut_json;
};


template<class T>
bool I_ioHelper::func(T&& value, const char* name, bool compel){
	if (Ic){
		return Ic->_func(value, name);
	}
	if (Oc){
		Oc->_func(value, name, compel);
		return true;
	}
	return false;
}