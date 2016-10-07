#pragma once

#ifdef _ENGINE_MODE

struct Float {
	Float(float* value)
		:value(value)
	{}

	Float &operator=(const float &o) {
		*value = o;
		return *this;
	}
	operator float() {
		return *value;
	}
	float* value;
};

struct Vector2 {
	Vector2(XMVECTOR& v)
		: x(&v.x)
		, y(&v.y)
	{}

	Vector2(XMFLOAT2& v)
		: x(&v.x)
		, y(&v.y)
	{}
	Vector2(XMFLOAT4& v)
		: x(&v.x)
		, y(&v.y)
	{}
	Vector2(const Float& x, const  Float& y)
		: x(x)
		, y(y)
	{}

	Float x;
	Float y;
};
struct Vector3 {
	Vector3(XMVECTOR& v)
		: x(&v.x)
		, y(&v.y)
		, z(&v.z)
	{}

	Vector3(XMFLOAT4& v)
		: x(&v.x)
		, y(&v.y)
		, z(&v.z)
	{}
	Vector3(const Float& x, const Float& y, const Float& z)
		: x(x)
		, y(y)
		, z(z)
	{}
	Float x;
	Float y;
	Float z;
};
struct Color {
	Color(XMVECTOR& v)
		: r(&v.x)
		, g(&v.y)
		, b(&v.z)
		, a(&v.w)
	{}

	Color(XMFLOAT4& v)
		: r(&v.x)
		, g(&v.y)
		, b(&v.z)
		, a(&v.w)
	{}
	Color(const Float& r, const  Float& g, const  Float& b, const  Float& a)
		: r(r)
		, g(g)
		, b(b)
		, a(a)
	{}
	Float r;
	Float g;
	Float b;
	Float a;
};

#include <functional>
#include <vector>
#include "Game/Script/GameObject.h"
struct InspectorDataSet;
class Enabled;
class Component;

#include "IAsset.h"
#include "Engine/AssetLoad.h"


class IActor;
class Inspector{
public:
	Inspector(const std::string& name, Component* target);
	~Inspector();

	void AddEnableButton(Enabled* enable);

	template<typename T, typename Func>
	void Add(const std::string& text, T* data, Func collback){
		Add(text, data, std::function<void(T)>(collback));
	}

	template<typename T>
	void Add(const std::string& text, T* data, const std::function<void(T)>& collback);
	template<typename T, typename Func>
	void Add(const std::string& text, Asset<T>* data, Func collback){
		Add<T>(text, (IAsset*)data, std::function<void()>(collback));
	}
	template<class T>
	void Add(const std::string& text, IAsset* data, const std::function<void(void)>& collback) {

		std::function<void(std::string)> loadcoll = [data, collback](std::string path) {
			MD5::MD5HashCoord hash;
			if (AssetDataBase::FilePath2Hash(path.c_str(), hash)) {
				AssetLoad::Instance<T>(hash, (Asset<T>*)data);
			}
			else {
				data->Free();
			}
			collback();
		};
		auto dataset = new TemplateInspectorDataSet<std::string>(text, &data->m_Name, loadcoll);
		m_DataSet.push_back(InspectorDataSet(InspectorDataFormat::String, dataset));
	}

	template<typename Func>
	void Add(const std::string& text, GameObject* data, Func collback) {
		Add(text, (wp<IActor>*)data, std::function<void()>(collback));
	}
	void Add(const std::string& text, wp<IActor>* data, const std::function<void(void)>& collback);

	void AddLabel(const std::string& text);
	void AddSlideBar(const std::string& text, float min, float max, float* data, const std::function<void(float)>& collback);
	void AddButton(const std::string& text, const std::function<void()>& collback);
	void AddSelect(const std::string& text, int* data, std::vector<std::string> selects, const std::function<void(int)>& collback);

	void Complete();
private:


	std::string m_Name;
	Component* m_Target;
	std::vector<InspectorDataSet> m_DataSet;
	void* m_EnableButton;
};

#endif