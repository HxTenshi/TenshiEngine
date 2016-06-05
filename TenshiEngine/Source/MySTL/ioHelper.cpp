#include "ioHelper.h"

//テンプレートの特殊化
#define _DOUBLE(x) \
template<> \
x I_InputHelper::get(const picojson::value& value){ \
	return (x)value.get<double>(); \
}

_DOUBLE(unsigned int)
_DOUBLE(int)
_DOUBLE(float)
_DOUBLE(double)


#undef _DOUBLE


template<>
std::string I_InputHelper::get(const picojson::value& value){
	return (std::string)value.get<std::string>();
}

template<>
bool I_InputHelper::get(const picojson::value& value){
	return (bool)value.get<bool>();
}
template<>
picojson::object I_InputHelper::get(const picojson::value& value){
	return (picojson::object)value.get<picojson::object>();
}
template<>
picojson::value I_InputHelper::get(const picojson::value& value){
	return picojson::value();
}



//テンプレートの特殊化
#define _DOUBLE(x) \
template<> \
void I_OutputHelper::_func_out(const x& value, const char* name){ \
	o->insert(std::make_pair(name, picojson::value((double)value))); \
}

_DOUBLE(unsigned int)
_DOUBLE(int)
_DOUBLE(float)
_DOUBLE(double)

#undef _DOUBLE

template<>
void I_OutputHelper::_func_out(const std::string& value, const char* name){
	o->insert(std::make_pair(name, picojson::value((std::string)value)));
}
template<>
void I_OutputHelper::_func_out(const bool& value, const char* name){
	o->insert(std::make_pair(name, picojson::value((bool)value)));
}
template<>
void I_OutputHelper::_func_out(const picojson::object& value, const char* name){
	o->insert(std::make_pair(name, picojson::value((picojson::object)value)));
}

template<>
void I_OutputHelper::_func_out(const picojson::value& value, const char* name){
	o->insert(std::make_pair(name, (picojson::value)value));
}