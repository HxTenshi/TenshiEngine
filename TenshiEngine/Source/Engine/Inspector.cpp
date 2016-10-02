#include "Inspector.h"

#ifdef _ENGINE_MODE

#include"Game/Parts/Enabled.h"
#include "../../CppWPFdll/InspectorDataSet.h"

#include "Window/Window.h"

#include "AssetDataBase.h"

#pragma push_macro("new")
#undef new

Inspector::Inspector(const std::string& name, Component* target)
	:m_Name(name)
	, m_Target(target)
	,m_EnableButton(NULL)
{
}
Inspector::~Inspector(){

	//if (m_EnableButton){
	//	delete ((TemplateInspectorDataSet<bool>*)m_EnableButton);
	//}
	//
	//for (auto& i : m_DataSet){
	//	switch (i.format)
	//	{
	//	case InspectorDataFormat::Float:
	//		delete ((TemplateInspectorDataSet<float>*)i.data);
	//		break;
	//
	//	case InspectorDataFormat::Int:
	//		delete ((TemplateInspectorDataSet<int>*)i.data);
	//		break;
	//
	//	case InspectorDataFormat::Bool:
	//		delete ((TemplateInspectorDataSet<bool>*)i.data);
	//		break;
	//
	//	case InspectorDataFormat::String:
	//		delete ((TemplateInspectorDataSet<std::string>*)i.data);
	//		break;
	//
	//	case InspectorDataFormat::Vector2:
	//		delete ((TemplateInspectorDataSet<Vector2>*)i.data);
	//		break;
	//
	//	case InspectorDataFormat::Vector3:
	//		delete ((TemplateInspectorDataSet<Vector3>*)i.data);
	//		break;
	//
	//	case InspectorDataFormat::Color:
	//		delete ((TemplateInspectorDataSet<Color>*)i.data);
	//		break;
	//
	//	case InspectorDataFormat::Button:
	//		delete ((InspectorButtonDataSet*)i.data);
	//		break;
	//
	//	case InspectorDataFormat::Select:
	//		delete ((InspectorSelectDataSet*)i.data);
	//		break;
	//
	//	case InspectorDataFormat::SlideBar:
	//		delete ((InspectorSlideBarDataSet*)i.data);
	//		break;
	//
	//	case InspectorDataFormat::Label:
	//		delete ((InspectorLabelDataSet*)i.data);
	//		break;
	//
	//	default:
	//		break;
	//	}
	//
	//}
}
void Inspector::AddEnableButton(Enabled* enable){
	if (m_EnableButton)return;
	m_EnableButton = new TemplateInspectorDataSet<bool>(m_Name, &enable->m_IsEnabled, [&, enable](bool f){
		f ? enable->Enable() : enable->Disable(); });
}

template<>
void Inspector::Add(const std::string& text, float* data,const std::function<void(float)>& collback){

	auto dataset = new TemplateInspectorDataSet<float>(text, data, collback);
	m_DataSet.push_back(InspectorDataSet(InspectorDataFormat::Float, dataset));
}
template<>
void Inspector::Add(const std::string& text, int* data, const std::function<void(int)>& collback){

	auto dataset = new TemplateInspectorDataSet<int>(text, data, collback);
	m_DataSet.push_back(InspectorDataSet(InspectorDataFormat::Int, dataset));
}
template<>
void Inspector::Add(const std::string& text, bool* data, const std::function<void(bool)>& collback){

	auto dataset = new TemplateInspectorDataSet<bool>(text, data, collback);
	m_DataSet.push_back(InspectorDataSet(InspectorDataFormat::Bool, dataset));
}
template<>
void Inspector::Add(const std::string& text, std::string* data, const std::function<void(std::string)>& collback){

	auto dataset = new TemplateInspectorDataSet<std::string>(text, data, collback);
	m_DataSet.push_back(InspectorDataSet(InspectorDataFormat::String, dataset));
}

void Inspector::Add(const std::string& text, IAsset* data, const std::function<void()>& collback){

	std::function<void(std::string)> loadcoll = [data, collback](std::string path){
		MD5::MD5HashCoord hash;
		if (AssetDataBase::FilePath2Hash(path.c_str(), hash)){
			data->Load(hash);
		}
		else{
			data->Free();
		}
		collback();
	};
	auto dataset = new TemplateInspectorDataSet<std::string>(text, &data->m_Name, loadcoll);
	m_DataSet.push_back(InspectorDataSet(InspectorDataFormat::String, dataset));
}

//#include "Game/Script/GameObject.h"
#include "Game/Actor.h"
void Inspector::Add(const std::string& text, wp<IActor>* data, const std::function<void()>& collback) {

	std::function<void(wp<IActor>)> loadcoll = [data, collback](wp<IActor> act) {
		if (!act)return;
		*data = act;
		collback();
	};

	auto dataset = new InspectorGameObjectDataSet(text, data, loadcoll);
	m_DataSet.push_back(InspectorDataSet(InspectorDataFormat::GameObject, dataset));
}

template<>
void Inspector::Add(const std::string& text, Vector2* data, const std::function<void(Vector2)>& collback){
	Float _x = data->x;
	Float _y = data->y;
	auto x = [_x, _y, collback](float _x){
		auto v = Vector2(Float(&_x), _y);
		collback(v);
	};
	auto y = [_x, _y, collback](float _y){
		auto v = Vector2(_x, Float(&_y));
		collback(v);
	};
	auto dataset = new InspectorVector2DataSet(text, data->x.value, x, data->y.value, y);
	m_DataSet.push_back(InspectorDataSet(InspectorDataFormat::Vector2, dataset));
}
template<>
void Inspector::Add(const std::string& text, Vector3* data, const std::function<void(Vector3)>& collback){
	Float _x = data->x;
	Float _y = data->y;
	Float _z = data->z;
	auto x = [_x,_y,_z, collback](float _x){
		auto v = Vector3(Float(&_x), _y, _z);
		collback(v);
	};
	auto y = [_x, _y, _z, collback](float _y){
		auto v = Vector3(_x, Float(&_y), _z);
		collback(v);
	};
	auto z = [_x, _y, _z, collback](float _z){
		auto v = Vector3(_x, _y, Float(&_z));
		collback(v);
	};
	auto dataset = new InspectorVector3DataSet(text, data->x.value, x, data->y.value, y, data->z.value, z);
	m_DataSet.push_back(InspectorDataSet(InspectorDataFormat::Vector3, dataset));
}

template<>
void Inspector::Add(const std::string& text, Color* data, const std::function<void(Color)>& collback){
	Float _r = data->r;
	Float _g = data->g;
	Float _b = data->b;
	Float _a = data->a;
	auto r = [_r,_g,_b,_a, collback](float _r){
		auto v = Color(Float(&_r), _g, _b, _a);
		collback(v);
	};
	auto g = [_r, _g, _b, _a, collback](float _g){
		auto v = Color(_r, Float(&_g), _b, _a);
		collback(v);
	};
	auto b = [_r, _g, _b, _a, collback](float _b){
		auto v = Color(_r, _g, Float(&_b), _a);
		collback(v);
	};
	auto a = [_r, _g, _b, _a, collback](float _a){
		auto v = Color(_r, _g, _b, Float(&_a));
		collback(v);
	};
	auto dataset = new InspectorColorDataSet(text, data->r.value, r, data->g.value, g, data->b.value, b, data->a.value, a);
	m_DataSet.push_back(InspectorDataSet(InspectorDataFormat::Color, dataset));
}

void Inspector::AddLabel(const std::string& text){
	auto dataset = new InspectorLabelDataSet(text);
	m_DataSet.push_back(InspectorDataSet(InspectorDataFormat::Label, dataset));
}
void Inspector::AddSlideBar(const std::string& text, float min, float max, float* data, const std::function<void(float)>& collback){

	auto dataset = new InspectorSlideBarDataSet(text,min,max,data,collback);
	m_DataSet.push_back(InspectorDataSet(InspectorDataFormat::SlideBar, dataset));
}
void Inspector::AddButton(const std::string& text,const std::function<void()>& collback){

	auto dataset = new InspectorButtonDataSet(text,collback);
	m_DataSet.push_back(InspectorDataSet(InspectorDataFormat::Button, dataset));
}

void Inspector::AddSelect(const std::string& text, int* data, std::vector<std::string> selects, const std::function<void(int)>& collback){

	auto dataset = new InspectorSelectDataSet(text, selects, data, collback);
	m_DataSet.push_back(InspectorDataSet(InspectorDataFormat::Select , dataset));
}

void Inspector::Complete(){

	if (m_EnableButton){
		auto data = (TemplateInspectorDataSet<bool>*)m_EnableButton;
		Window::ViewInspector(m_Name, m_Target, m_DataSet, data);
	}
	else{
		Window::ViewInspector(m_Name, m_Target, m_DataSet);
	}

}


#endif
#pragma pop_macro("new")