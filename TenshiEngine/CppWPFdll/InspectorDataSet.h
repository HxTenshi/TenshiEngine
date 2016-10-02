#pragma once

#include <string>
#include <functional>
typedef std::function<void(float)> FloatCollback;
typedef std::function<void(std::string)> StringCollback;
typedef std::function<void(bool)> BoolCollback;

enum class InspectorDataFormat{
	Label,
	String,
	Float,
	Vector2,
	Vector3,
	Bool,
	Int,
	SlideBar,
	Color,
	Button,
	Select,
	GameObject,
	Count,
};
struct InspectorDataSet{
	InspectorDataSet(InspectorDataFormat f, void* d) :format(f), data(d){}
	InspectorDataFormat format;
	void* data;
private:
	InspectorDataSet& operator =(const InspectorDataSet&);
};
struct InspectorLabelDataSet{
	InspectorLabelDataSet(std::string t) :Text(t){}
	const std::string Text;
private:
	InspectorLabelDataSet& operator =(const InspectorLabelDataSet&);
};
struct InspectorSelectDataSet{
	InspectorSelectDataSet(std::string t, std::vector<std::string> selects, int *d, std::function<void(int)> collback) :Text(t), select(selects), data(d), collBack(collback){}
	const std::string Text;
	std::vector<std::string> select;
	int* data;
	std::function<void(int)> collBack;
private:
	InspectorSelectDataSet& operator =(const InspectorSelectDataSet&);
};

template <class T>
struct TemplateInspectorDataSet{
	TemplateInspectorDataSet(std::string t, T* d, std::function<void(T)> collback) :Text(t), data(d), collBack(collback){}
	const std::string Text;
	T* data;
	std::function<void(T)> collBack;
private:
	TemplateInspectorDataSet& operator =(const TemplateInspectorDataSet&);
};

struct InspectorSlideBarDataSet{
	InspectorSlideBarDataSet(std::string t, float min, float max, float* d, FloatCollback collback) :Text(t), _min(min), _max(max), data(d), collBack(collback){}
	const std::string Text;
	float _min, _max;
	float* data;
	FloatCollback collBack;

private:
	InspectorSlideBarDataSet& operator =(const InspectorSlideBarDataSet&);
};
struct InspectorVector3DataSet{
	InspectorVector3DataSet(std::string t
		, float* x, FloatCollback collbackx
		, float* y, FloatCollback collbacky
		, float* z, FloatCollback collbackz)
		:Text(t)
		, datax(x), collbackX(collbackx)
		, datay(y), collbackY(collbacky)
		, dataz(z), collbackZ(collbackz)
	{}
	const std::string Text;
	float* datax;
	FloatCollback collbackX;
	float* datay;
	FloatCollback collbackY;
	float* dataz;
	FloatCollback collbackZ;
	void* com;

private:
	InspectorVector3DataSet& operator =(const InspectorVector3DataSet&);
};
struct InspectorVector2DataSet{
	InspectorVector2DataSet(std::string t
		, float* x, FloatCollback collbackx
		, float* y, FloatCollback collbacky)
		:Text(t)
		, datax(x), collbackX(collbackx)
		, datay(y), collbackY(collbacky)
	{}
	const std::string Text;
	float* datax;
	FloatCollback collbackX;
	float* datay;
	FloatCollback collbackY;
	void* com;

private:
	InspectorVector2DataSet& operator =(const InspectorVector2DataSet&);
};

struct InspectorColorDataSet{
	InspectorColorDataSet(std::string t
		, float* r, FloatCollback collbackr
		, float* g, FloatCollback collbackg
		, float* b, FloatCollback collbackb
		, float* a, FloatCollback collbacka)
		:Text(t)
		, datar(r), collbackR(collbackr)
		, datag(g), collbackG(collbackg)
		, datab(b), collbackB(collbackb)
		, dataa(a), collbackA(collbacka)
	{}
	const std::string Text;
	float* datar;
	FloatCollback collbackR;
	float* datag;
	FloatCollback collbackG;
	float* datab;
	FloatCollback collbackB;
	float* dataa;
	FloatCollback collbackA;
	void* com;

private:
	InspectorColorDataSet& operator =(const InspectorColorDataSet&);
};

struct InspectorButtonDataSet{
	InspectorButtonDataSet(std::string t, std::function<void()> collback) :Text(t), collBack(collback){}
	const std::string Text;
	std::function<void()> collBack;
private:
	InspectorButtonDataSet& operator =(const InspectorButtonDataSet&);
};

template<class T>
class wp;
class IActor;
struct InspectorGameObjectDataSet {
	InspectorGameObjectDataSet(std::string t, wp<IActor>* d, std::function<void(wp<IActor>)> collback) :Text(t), data(d), collBack(collback) {}
	const std::string Text;
	wp<IActor>* data;
	std::function<void(wp<IActor>)> collBack;
private:
	InspectorGameObjectDataSet& operator =(const InspectorGameObjectDataSet&);
};
