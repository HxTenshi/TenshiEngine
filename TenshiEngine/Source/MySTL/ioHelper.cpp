#include "ioHelper.h"
#include "Library/MD5.h"
#include "Engine/AssetDataBase.h"

#include "Game/Game.h"
void ioGameObjectHelper::func(GameObject* target, const char* name, I_ioHelper* io, GameObject* This) {
	if (io->isInput()) {
		UniqueID id;
		io->func(id, name);
		(*This)->SetInitializeStageCollQueue([id, target]() {
			*target = Game::FindUID(id);
		});
	}
	else {
		io->func(((bool)*target)?(*target)->GetUniqueID():"", name);
	}
}

//テンプレートの特殊化
#define _DOUBLE(x) \
template<> \
void I_InputHelper::get(const picojson::value& value, x* out){ \
	*out = (x)value.get<double>(); \
}

_DOUBLE(unsigned int)
_DOUBLE(int)
_DOUBLE(float)
_DOUBLE(double)


#undef _DOUBLE


template<>
void I_InputHelper::get(const picojson::value& value, std::string* out){
	*out = (std::string)value.get<std::string>();
}

template<>
void I_InputHelper::get(const picojson::value& value, bool* out){
	*out = (bool)value.get<bool>();
}
template<>
void I_InputHelper::get(const picojson::value& value, picojson::object* out){
	*out = (picojson::object)value.get<picojson::object>();
}
template<>
void I_InputHelper::get(const picojson::value& value, picojson::value* out){
	(void)value;
	*out = picojson::value();
}
//template<>
//void I_InputHelper::get(const picojson::value& value, MD5::MD5HashCoord* out){
//	auto c = (std::string)value.get<std::string>();
//	*out = MD5::MD5HashCoord(c.c_str());
//}




//テンプレートの特殊化
#define _DOUBLE(x) \
template<> \
void I_OutputHelper::_func_out(const x* value, const char* name){ \
	o->insert(std::make_pair(name, picojson::value((double)*value))); \
}

_DOUBLE(unsigned int)
_DOUBLE(int)
_DOUBLE(float)
_DOUBLE(double)

#undef _DOUBLE

template<>
void I_OutputHelper::_func_out(const std::string* value, const char* name){
	o->insert(std::make_pair(name, picojson::value((std::string)*value)));
}
template<>
void I_OutputHelper::_func_out(const bool* value, const char* name){
	o->insert(std::make_pair(name, picojson::value((bool)*value)));
}
template<>
void I_OutputHelper::_func_out(const picojson::object* value, const char* name){
	o->insert(std::make_pair(name, picojson::value((picojson::object)*value)));
}

template<>
void I_OutputHelper::_func_out(const picojson::value* value, const char* name){
	o->insert(std::make_pair(name, (picojson::value)*value));
}

//template<>
//void I_OutputHelper::_func_out(const MD5::MD5HashCoord* value, const char* name){
//	o->insert(std::make_pair(name, picojson::value((std::string)value->GetString())));
//}
//
//template<>
//void I_OutputHelper::_func_out(const IAsset* value, const char* name){
//	o->insert(std::make_pair(name, picojson::value((std::string)value->m_Hash.GetString())));
//}
