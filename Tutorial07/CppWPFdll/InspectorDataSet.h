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
	Vector3,
	Bool,
	SlideBar,
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
struct InspectorFloatDataSet{
	InspectorFloatDataSet(std::string t, float* d, FloatCollback collback) :Text(t), data(d), collBack(collback){}
	const std::string Text;
	float* data;
	FloatCollback collBack;
private:
	InspectorFloatDataSet& operator =(const InspectorFloatDataSet&);
};
struct InspectorBoolDataSet{
	InspectorBoolDataSet(std::string t, bool* d, BoolCollback collback) :Text(t), data(d), collBack(collback){}
	const std::string Text;
	bool* data;
	BoolCollback collBack;
private:
	InspectorBoolDataSet& operator =(const InspectorBoolDataSet&);
};
struct InspectorStringDataSet{
	InspectorStringDataSet(std::string t, std::string* d, StringCollback collback) :Text(t), data(d), collBack(collback){}
	const std::string Text;
	std::string* data;
	StringCollback collBack;
private:
	InspectorStringDataSet& operator =(const InspectorStringDataSet&);
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
