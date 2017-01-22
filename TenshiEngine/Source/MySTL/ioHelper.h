#pragma once


#include <stack>
#include <fstream>
#include "Library/picojson.h"
#include "MySTL/Utility.h"

#include "Engine/IAsset.h"
#include "Game/Script/GameObject.h"

class I_InputHelper;
class I_OutputHelper;

class I_ioHelper;
class Actor;
namespace ioGameObjectHelper {
	void func(wp<Actor>* target, const char* name, I_ioHelper* io, wp<Actor>* This);
	void func(std::vector<wp<Actor>>* target, const char* name, I_ioHelper* io, wp<Actor>* This);
};

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
	bool _func(T* out, const char* name){
		auto v = o->find(name);
		if (v == o->end())return false;

		T temp;
		if (prefab){
			if (!prefab->func(temp, name)){
				return false;
			}
		}

		get<T>(v->second, out);
		return true;
	}
	//template<class T>
	//bool _func(Asset<T>* out, const char* name){
	//	auto v = o->find(name);
	//	if (v == o->end())return false;
	//	
	//	IAsset temp;
	//	if (prefab){
	//		if (!prefab->func((IAsset)temp, name)){
	//			return false;
	//		}
	//	}
	//	get<T>(v->second, (IAsset*)out);
	//	return true;
	//}
	template<>
	bool _func(XMVECTOR* out, const char* name) {
		auto v = o->find(name);
		if (v != o->end()) {

			MD5::MD5HashCode temp;
			if (prefab) {
				if (!prefab->func((MD5::MD5HashCode)temp, name)) {
					return false;
				}
			}
			get<MD5::MD5HashCode>(v->second, (MD5::MD5HashCode*)out);
			return true;
		}

		bool f = true;
		f = f&&_func(&out->x, (std::string(name) + ".x").c_str());
		f = f&&_func(&out->y, (std::string(name) + ".y").c_str());
		f = f&&_func(&out->z, (std::string(name) + ".z").c_str());
		f = f&&_func(&out->w, (std::string(name) + ".w").c_str());
		return f;
	}
	template<>
	bool _func(XMFLOAT2* out, const char* name) {
		auto v = o->find(name);
		if (v != o->end()) {

			MD5::MD5HashCode temp;
			if (prefab) {
				if (!prefab->func((MD5::MD5HashCode)temp, name)) {
					return false;
				}
			}
			get<MD5::MD5HashCode>(v->second, (MD5::MD5HashCode*)out);
			return true;
		}

		bool f = true;
		f = f&&_func(&out->x, (std::string(name) + ".x").c_str());
		f = f&&_func(&out->y, (std::string(name) + ".y").c_str());
		return f;
	}
	template<>
	bool _func(XMFLOAT4* out, const char* name) {
		auto v = o->find(name);
		if (v != o->end()) {

			MD5::MD5HashCode temp;
			if (prefab) {
				if (!prefab->func((MD5::MD5HashCode)temp, name)) {
					return false;
				}
			}
			get<MD5::MD5HashCode>(v->second, (MD5::MD5HashCode*)out);
			return true;
		}

		bool f = true;
		f = f&&_func(&out->x, (std::string(name) + ".x").c_str());
		f = f&&_func(&out->y, (std::string(name) + ".y").c_str());
		f = f&&_func(&out->z, (std::string(name) + ".z").c_str());
		f = f&&_func(&out->w, (std::string(name) + ".w").c_str());
		return f;
	}
	template<>
	bool _func(MD5::MD5HashCode* out, const char* name) {
		//auto v = o->find(name);
		//if (v != o->end()) {

		//	MD5::MD5HashCode temp;
		//	if (prefab) {
		//		if (!prefab->func((MD5::MD5HashCode)temp, name)) {
		//			return false;
		//		}
		//	}
		//	get<MD5::MD5HashCode>(v->second, (MD5::MD5HashCode*)out);
		//	return true;
		//}

		bool f = true;
		f = f&&_func(&out->key_i[0], (std::string(name) + "0").c_str());
		f = f&&_func(&out->key_i[1], (std::string(name) + "1").c_str());
		f = f&&_func(&out->key_i[2], (std::string(name) + "2").c_str());
		f = f&&_func(&out->key_i[3], (std::string(name) + "3").c_str());
		return f;
	}
	template<>
	bool _func(IAsset* out, const char* name) {

		bool f = true;
		f = f&&_func(&out->m_Hash.key_i[0], (std::string(name) + "0").c_str());
		f = f&&_func(&out->m_Hash.key_i[1], (std::string(name) + "1").c_str());
		f = f&&_func(&out->m_Hash.key_i[2], (std::string(name) + "2").c_str());
		f = f&&_func(&out->m_Hash.key_i[3], (std::string(name) + "3").c_str());
		return f;
	}
	template<class T>
	bool _func(Asset<T>* out, const char* name) {
		//auto v = o->find(name);
		//if (v != o->end()) {

		//	IAsset temp;
		//	if (prefab) {
		//		if (!prefab->func((IAsset)temp, name)) {
		//			return false;
		//		}
		//	}
		//	get<T>(v->second, (IAsset*)out);
		//	return true;
		//}

		bool f = true;
		f = f&&_func(&out->m_Hash.key_i[0], (std::string(name) + "0").c_str());
		f = f&&_func(&out->m_Hash.key_i[1], (std::string(name) + "1").c_str());
		f = f&&_func(&out->m_Hash.key_i[2], (std::string(name) + "2").c_str());
		f = f&&_func(&out->m_Hash.key_i[3], (std::string(name) + "3").c_str());
		return f;
	}

	template<class T>
	bool _func(std::vector<T>* out, const char* name) {
		auto v = o->find(name);
		if (v == o->end())return false;

		//std::vector<T> temp;
		//if (prefab) {
		//	if (!prefab->func(temp, name)) {
		//		return false;
		//	}
		//}

		picojson::array ar;

		get<picojson::array>(v->second, &ar);
		if (ar.size() == 0)return false;
		auto back = o;
		out->resize(ar.size());
		int i = 0;
		for (auto& value : ar) {
			
			o = &value.get<picojson::object>();
			//T b;
			//_func(&b, "a");
			//(*out)[i] = b;
			_func(&(*out).data()[i], "a");
			i++;
		}
		o = back;
		return true;
	}

	template<>
	bool _func(std::vector<bool>* out, const char* name) {
		auto v = o->find(name);
		if (v == o->end())return false;

		//std::vector<bool> temp;
		//if (prefab) {
		//	if (!prefab->func(temp, name)) {
		//		return false;
		//	}
		//}

		picojson::array ar;

		get<picojson::array>(v->second, &ar);
		if (ar.size() == 0)return false;
		auto back = o;
		out->resize(ar.size());
		int i = 0;
		for (auto& value : ar) {

			o = &value.get<picojson::object>();
			bool b;
			_func(&b, "a");
			(*out)[i] = b;
			i++;
		}
		o = back;
		return true;
	}


private:
	template<class T>
	void get(const picojson::value& value, T* out);

	template<>
	void I_InputHelper::get(const picojson::value& value, MD5::MD5HashCode* out) {
		auto c = (std::string)value.get<std::string>();
		*out = MD5::MD5HashCode(c.c_str());
	}
	template<class T>
	void get(const picojson::value& value, IAsset* out){
		auto c = (std::string)value.get<std::string>();
		out->m_Hash = MD5::MD5HashCode(c.c_str());
	}

private:

	I_InputHelper* prefab;
	template<>
	void get(const picojson::value & value, picojson::array * out) {
		*out = (picojson::array)value.get<picojson::array>();
	}
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
	MD5::MD5HashCode GetUniqueID(const wp<Actor>* out);
	void _func(const GameObject* out, const char* name, bool compel) {
		func(GetUniqueID(out), name, compel);
	}
	template<class T>
	void _func(const T* out, const char* name, bool compel){

		T temp;
		if (mOutputFilterRebirth){
			if (compel)return;
			if (prefab && prefab->func(temp, name)){
				if (temp != *out)return;
			}
		}
		else{
			if (!compel && prefab && prefab->func(temp, name)){
				if (temp == *out)return;
			}
		}
		_func_out(out,name);
	}
	template<>
	void _func(const XMVECTOR* out, const char* name, bool compel) {

		XMVECTOR temp;
		if (mOutputFilterRebirth) {
			if (compel)return;
			if (prefab && prefab->func(temp, name)) {
				if (!(temp.x == out->x&&
					temp.y == out->y&&
					temp.z == out->z&&
					temp.w == out->w))return;
			}
		}
		else {
			if (!compel && prefab && prefab->func(temp, name)) {
				if ((temp.x == out->x&&
					temp.y == out->y&&
					temp.z == out->z&&
					temp.w == out->w))return;
			}
		}
		o->insert(std::make_pair((std::string(name) + ".x").c_str(), picojson::value((double)out->x)));
		o->insert(std::make_pair((std::string(name) + ".y").c_str(), picojson::value((double)out->y)));
		o->insert(std::make_pair((std::string(name) + ".z").c_str(), picojson::value((double)out->z)));
		o->insert(std::make_pair((std::string(name) + ".w").c_str(), picojson::value((double)out->w)));

	}
	template<>
	void _func(const XMFLOAT2* out, const char* name, bool compel) {

		XMFLOAT2 temp;
		if (mOutputFilterRebirth) {
			if (compel)return;
			if (prefab && prefab->func(temp, name)) {
				if (!(temp.x == out->x&&
					temp.y == out->y))return;
			}
		}
		else {
			if (!compel && prefab && prefab->func(temp, name)) {
				if ((temp.x == out->x&&
					temp.y == out->y))return;
			}
		}
		o->insert(std::make_pair((std::string(name) + ".x").c_str(), picojson::value((double)out->x)));
		o->insert(std::make_pair((std::string(name) + ".y").c_str(), picojson::value((double)out->y)));

	}
	template<>
	void _func(const XMFLOAT4* out, const char* name, bool compel) {

		XMFLOAT4 temp;
		if (mOutputFilterRebirth) {
			if (compel)return;
			if (prefab && prefab->func(temp, name)) {
				if (!(temp.x == out->x&&
					temp.y == out->y&&
					temp.z == out->z&&
					temp.w == out->w))return;
			}
		}
		else {
			if (!compel && prefab && prefab->func(temp, name)) {
				if ((temp.x == out->x&&
					temp.y == out->y&&
					temp.z == out->z&&
					temp.w == out->w))return;
			}
		}
		o->insert(std::make_pair((std::string(name) + ".x").c_str(), picojson::value((double)out->x)));
		o->insert(std::make_pair((std::string(name) + ".y").c_str(), picojson::value((double)out->y)));
		o->insert(std::make_pair((std::string(name) + ".z").c_str(), picojson::value((double)out->z)));
		o->insert(std::make_pair((std::string(name) + ".w").c_str(), picojson::value((double)out->w)));

	}
	template<>
	void _func(const MD5::MD5HashCode* out, const char* name, bool compel) {

		MD5::MD5HashCode temp;
		if (mOutputFilterRebirth) {
			if (compel)return;
			if (prefab && prefab->func(temp, name)) {
				if (!(temp == *out))return;
			}
		}
		else {
			if (!compel && prefab && prefab->func(temp, name)) {
				if (temp == *out)return;
			}
		}
		o->insert(std::make_pair((std::string(name) + "0").c_str(), picojson::value((double)out->key_i[0])));
		o->insert(std::make_pair((std::string(name) + "1").c_str(), picojson::value((double)out->key_i[1])));
		o->insert(std::make_pair((std::string(name) + "2").c_str(), picojson::value((double)out->key_i[2])));
		o->insert(std::make_pair((std::string(name) + "3").c_str(), picojson::value((double)out->key_i[3])));

	}
	template<>
	void _func(const IAsset* out, const char* name, bool compel){

		IAsset temp;
		if (mOutputFilterRebirth){
			if (compel)return;
			if (prefab && prefab->func(temp, name)){
				if (!(temp.m_Hash == out->m_Hash))return;
			}
		}
		else{
			if (!compel && prefab && prefab->func(temp, name)){
				if (temp.m_Hash == out->m_Hash)return;
			}
		}
		//_func_out((IAsset*)out, name);

		o->insert(std::make_pair((std::string(name) + "0").c_str(), picojson::value((double)out->m_Hash.key_i[0])));
		o->insert(std::make_pair((std::string(name) + "1").c_str(), picojson::value((double)out->m_Hash.key_i[1])));
		o->insert(std::make_pair((std::string(name) + "2").c_str(), picojson::value((double)out->m_Hash.key_i[2])));
		o->insert(std::make_pair((std::string(name) + "3").c_str(), picojson::value((double)out->m_Hash.key_i[3])));
	}
	template<class T>
	void _func(const Asset<T>* out, const char* name, bool compel){

		IAsset temp;
		if (mOutputFilterRebirth){
			if (compel)return;
			if (prefab && prefab->func(temp, name)){
				if (!(temp.m_Hash == out->m_Hash))return;
			}
		}
		else{
			if (!compel && prefab && prefab->func(temp, name)){
				if (temp.m_Hash == out->m_Hash)return;
			}
		}
		//_func_out((IAsset*)out, name);
		//o->insert(std::make_pair(name, picojson::value((std::string)out->m_Hash.GetString())));
		o->insert(std::make_pair((std::string(name) + "0").c_str(), picojson::value((double)out->m_Hash.key_i[0])));
		o->insert(std::make_pair((std::string(name) + "1").c_str(), picojson::value((double)out->m_Hash.key_i[1])));
		o->insert(std::make_pair((std::string(name) + "2").c_str(), picojson::value((double)out->m_Hash.key_i[2])));
		o->insert(std::make_pair((std::string(name) + "3").c_str(), picojson::value((double)out->m_Hash.key_i[3])));
	}
	template<class T>
	void _func(const std::vector<T>* out, const char* name, bool compel) {
		//std::vector<T> temp;
		//if (mOutputFilterRebirth) {
		//	if (compel)return;
		//	if (prefab && prefab->func(temp, name)) {
		//		if (temp != *out)return;
		//	}
		//}
		//else {
		//	if (!compel && prefab && prefab->func(temp, name)) {
		//		if (temp == *out)return;
		//	}
		//}
		if (out->size() == 0)return;
		auto back = o;
		size_t s = out->size();
		picojson::array ar(s);
		int i = 0;
		for (int i = 0; i < out->size(); i++) {
			picojson::object newO;
			o = &newO;
			
			_func(&(*out)[i], "a", compel);
			ar[i] = picojson::value(newO);
		}
		//for (auto value : *out) {
		//	picojson::object newO;
		//	o = &newO;
		//	_func(&value, "a", compel);
		//	ar[i] = picojson::value(newO);
		//	i++;
		//}
		o = back;
		_func_out(&ar, name);
	}

	template<>
	void _func(const std::vector<bool>* out, const char* name, bool compel) {
		//std::vector<bool> temp;
		//if (mOutputFilterRebirth) {
		//	if (compel)return;
		//	if (prefab && prefab->func(temp, name)) {
		//		if (temp != *out)return;
		//	}
		//}
		//else {
		//	if (!compel && prefab && prefab->func(temp, name)) {
		//		if (temp == *out)return;
		//	}
		//}
		if (out->size() == 0)return;
		auto back = o;
		picojson::array ar(out->size());
		int i = 0;
		for (int i = 0; i < out->size(); i++) {
			picojson::object newO;
			o = &newO;
			const bool v = (*out)[i];

			_func(&v, "a", compel);
			ar[i] = picojson::value(newO);
		}
		//for (auto value : *out) {
		//	picojson::object newO;
		//	o = &newO;
		//	_func(&value, "a", compel);
		//	ar[i] = picojson::value(newO);
		//	i++;
		//}
		o = back;
		_func_out(&ar, name);
	}
	template<class T>
	void _func_out(const T* out, const char* name);
private:

	I_InputHelper* prefab;

	bool mOutputFilterRebirth;
	template<>
	void _func_out(const picojson::array * value, const char * name) {
		o->insert(std::make_pair(name, (picojson::array)*value));
	}
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
		return Ic->_func(&value, name);
	}
	if (Oc){
		Oc->_func(&value, name, compel);
		return true;
	}
	return false;
}