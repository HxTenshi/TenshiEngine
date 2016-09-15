#pragma once
#include <functional>
#include <vector>
class InspectorDataSet;
class Enabled;
class Component;

struct Vector2{
	Vector2(XMVECTOR& v)
		: x(v.x)
		, y(v.y)
	{}

	Vector2(XMFLOAT2& v)
		: x(v.x)
		, y(v.y)
	{}
	Vector2(XMFLOAT4& v)
		: x(v.x)
		, y(v.y)
	{}
	Vector2(float& x, float& y)
		: x(x)
		, y(y)
	{}
	float& x;
	float& y;
};
struct Vector3{
	Vector3(XMVECTOR& v)
		: x(v.x)
		, y(v.y)
		, z(v.z)
	{}

	Vector3(XMFLOAT4& v)
		: x(v.x)
		, y(v.y)
		, z(v.z)
	{}
	Vector3(float& x, float& y, float& z)
		: x(x)
		, y(y)
		, z(z)
	{}
	float& x;
	float& y;
	float& z;
};
struct Color{
	Color(XMVECTOR& v)
		: r(v.x)
		, g(v.y)
		, b(v.z)
		, a(v.z)
	{}

	Color(XMFLOAT4& v)
		: r(v.x)
		, g(v.y)
		, b(v.z)
		, a(v.z)
	{}
	Color(float& r, float& g, float& b, float& a)
		: r(r)
		, g(g)
		, b(b)
		, a(a)
	{}
	float& r;
	float& g;
	float& b;
	float& a;
};


class Inspector{
public:
	Inspector(const std::string& name, Component* target);
	~Inspector();

	void AddEnableButton(Enabled* enable);

	template<class T>
	void Add(const std::string& text, T* data, std::function<void(T)> collback);
	void AddLabel(const std::string& text);
	void AddSlideBar(const std::string& text, float min, float max, float* data, std::function<void(float)> collback);
	void AddButton(const std::string& text, std::function<void()> collback);
	void AddSelect(const std::string& text, int* data, std::vector<std::string> selects, std::function<void(int)> collback);

	void Complete();
private:
	std::string m_Name;
	Component* m_Target;
	std::vector<InspectorDataSet> m_DataSet;
	void* m_EnableButton;
};