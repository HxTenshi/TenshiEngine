#include "Inspector.h"

#include"Game/Parts/Enabled.h"
#include "../../CppWPFdll/InspectorDataSet.h"

#include "Window/Window.h"

Inspector::Inspector(const std::string& name, Component* target)
	:m_Name(name)
	, m_Target(target)
	,m_EnableButton(NULL)
{
}
Inspector::~Inspector(){

	if (m_EnableButton){
		delete ((TemplateInspectorDataSet<bool>*)m_EnableButton);
	}

	for (auto& i : m_DataSet){
		switch (i.format)
		{
		case InspectorDataFormat::Float:
			delete ((TemplateInspectorDataSet<float>*)i.data);
			break;

		case InspectorDataFormat::Int:
			delete ((TemplateInspectorDataSet<int>*)i.data);
			break;

		case InspectorDataFormat::Bool:
			delete ((TemplateInspectorDataSet<bool>*)i.data);
			break;

		case InspectorDataFormat::String:
			delete ((TemplateInspectorDataSet<std::string>*)i.data);
			break;

		case InspectorDataFormat::Vector2:
			delete ((TemplateInspectorDataSet<Vector2>*)i.data);
			break;

		case InspectorDataFormat::Vector3:
			delete ((TemplateInspectorDataSet<Vector3>*)i.data);
			break;

		case InspectorDataFormat::Color:
			delete ((TemplateInspectorDataSet<Color>*)i.data);
			break;

		case InspectorDataFormat::Button:
			delete ((InspectorButtonDataSet*)i.data);
			break;

		case InspectorDataFormat::Select:
			delete ((InspectorSelectDataSet*)i.data);
			break;

		case InspectorDataFormat::SlideBar:
			delete ((InspectorSlideBarDataSet*)i.data);
			break;

		case InspectorDataFormat::Label:
			delete ((InspectorLabelDataSet*)i.data);
			break;

		default:
			break;
		}

	}
}
void Inspector::AddEnableButton(Enabled* enable){
	if (m_EnableButton)return;
	m_EnableButton = new TemplateInspectorDataSet<bool>(m_Name, &enable->m_IsEnabled, [&, enable](bool f){
		f ? enable->Enable() : enable->Disable(); });
}

template<>
void Inspector::Add(const std::string& text, float* data, std::function<void(float)> collback){

	auto dataset = new TemplateInspectorDataSet<float>(text, data, collback);
	m_DataSet.push_back(InspectorDataSet(InspectorDataFormat::Float, dataset));
}
template<>
void Inspector::Add(const std::string& text, int* data, std::function<void(int)> collback){

	auto dataset = new TemplateInspectorDataSet<int>(text, data, collback);
	m_DataSet.push_back(InspectorDataSet(InspectorDataFormat::Int, dataset));
}
template<>
void Inspector::Add(const std::string& text, bool* data, std::function<void(bool)> collback){

	auto dataset = new TemplateInspectorDataSet<bool>(text, data, collback);
	m_DataSet.push_back(InspectorDataSet(InspectorDataFormat::Bool, dataset));
}
template<>
void Inspector::Add(const std::string& text, std::string* data, std::function<void(std::string)> collback){

	auto dataset = new TemplateInspectorDataSet<std::string>(text, data, collback);
	m_DataSet.push_back(InspectorDataSet(InspectorDataFormat::String, dataset));
}
template<>
void Inspector::Add(const std::string& text, Vector2* data, std::function<void(Vector2)> collback){

	auto x = [data, collback](float _x){
		float _y = data->y;
		collback(Vector2(_x, _y));
	};
	auto y = [data, collback](float _y){
		float _x = data->x;
		collback(Vector2(_x, _y));
	};
	auto dataset = new InspectorVector2DataSet(text, &data->x, x, &data->y, y);
	m_DataSet.push_back(InspectorDataSet(InspectorDataFormat::Vector2, dataset));
}
template<>
void Inspector::Add(const std::string& text, Vector3* data, std::function<void(Vector3)> collback){

	auto x = [data, collback](float _x){
		float _y = data->y;
		float _z = data->z;
		collback(Vector3(_x, _y, _z));
	};
	auto y = [data, collback](float _y){
		float _x = data->x;
		float _z = data->z;
		collback(Vector3(_x, _y, _z));
	};
	auto z = [data, collback](float _z){
		float _x = data->x;
		float _y = data->y;
		collback(Vector3(_x, _y, _z));
	};
	auto dataset = new InspectorVector3DataSet(text, &data->x, x, &data->y, y, &data->z, z);
	m_DataSet.push_back(InspectorDataSet(InspectorDataFormat::Vector3, dataset));
}

template<>
void Inspector::Add(const std::string& text, Color* data, std::function<void(Color)> collback){

	auto r = [data, collback](float _r){
		float _g = data->g;
		float _b = data->b;
		float _a = data->a;
		collback(Color(_r, _g, _b, _a));
	};
	auto g = [data, collback](float _g){
		float _r = data->r;
		float _b = data->b;
		float _a = data->a;
		collback(Color(_r, _g, _b, _a));
	};
	auto b = [data, collback](float _b){
		float _r = data->r;
		float _g = data->g;
		float _a = data->a;
		collback(Color(_r, _g, _b, _a));
	};
	auto a = [data, collback](float _a){
		float _r = data->r;
		float _g = data->g;
		float _b = data->b;
		collback(Color(_r, _g, _b, _a));
	};
	auto dataset = new InspectorColorDataSet(text, &data->r, r, &data->g, g, &data->b, b, &data->a, a);
	m_DataSet.push_back(InspectorDataSet(InspectorDataFormat::Color, dataset));
}

void Inspector::AddLabel(const std::string& text){
	auto dataset = new InspectorLabelDataSet(text);
	m_DataSet.push_back(InspectorDataSet(InspectorDataFormat::Label, dataset));
}
void Inspector::AddSlideBar(const std::string& text, float min, float max, float* data, std::function<void(float)> collback){

	auto dataset = new InspectorSlideBarDataSet(text,min,max,data,collback);
	m_DataSet.push_back(InspectorDataSet(InspectorDataFormat::SlideBar, dataset));
}
void Inspector::AddButton(const std::string& text, std::function<void()> collback){

	auto dataset = new InspectorButtonDataSet(text,collback);
	m_DataSet.push_back(InspectorDataSet(InspectorDataFormat::Button, dataset));
}

void Inspector::AddSelect(const std::string& text, int* data, std::vector<std::string> selects, std::function<void(int)> collback){

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