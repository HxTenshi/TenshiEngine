#pragma once


#include <stack>
#include <fstream>
#include "Library/picojson.h"

class InputHelper;
class OutputHelper;

//jsonì«Ç›èëÇ´
class I_ioHelper{
protected:
	I_ioHelper(InputHelper* ic, OutputHelper* oc) :Ic(ic), Oc(oc), error(false){}
	InputHelper* Ic;
	OutputHelper* Oc;
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


//ì«Ç›çûÇ›èàóù
class InputHelper : public I_ioHelper{
public:
	InputHelper(const std::string& fileName) :I_ioHelper(this, NULL), jsonfile(fileName)
	{
		if (jsonfile.fail()){
			error = true;
			return;
		}
		jsonfile >> json;
		pushObject(json.get<picojson::object>());
	}
	~InputHelper(){
	}

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
	std::ifstream jsonfile;
};

//èëÇ´çûÇ›èàóù
class OutputHelper : public I_ioHelper{
public:
	OutputHelper(const std::string& fileName) :I_ioHelper(NULL, this), jsonfile(fileName)
	{
		if (jsonfile.fail()){
			error = true;
			return;
		}
		pushObject();
	}
	~OutputHelper(){

		json = picojson::value(popObject());
		jsonfile << json;
	}
	template<class T>
	void _func(const T& out, const char* name);
private:
	std::ofstream jsonfile;
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