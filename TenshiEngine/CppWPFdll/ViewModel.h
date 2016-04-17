#pragma once

#include "Using.h"
#include <string>
#include < vcclr.h >
#include <functional>

ref class NofityPropertyChanged : public INotifyPropertyChanged
{
public:
	virtual event PropertyChangedEventHandler ^PropertyChanged;
protected:
	void NotifyPropertyChanged(String ^info)
	{
		PropertyChanged(this, gcnew PropertyChangedEventArgs(info));
	}
};


template<class T,class Func>
class DataModel{
public:
	DataModel(T* pf, Func collback)
		:m_x(pf)
		, m_collback(collback){
	}
	~DataModel(){
	}
	T& get() {
		return *m_x;
	}

	bool set(T value) {
		if (*m_x == value)return false;
		auto coll = new std::function<void()>();
		*coll = [=](){ m_collback(value); delete coll; };
		Data::MyPostMessage(MyWindowMessage::ChangeParamComponent, coll);
		return true;
	}

private:
	T* m_x;
	Func m_collback;
	//std::function<void()>* coll;
};

ref class IViewModel : public NofityPropertyChanged {
public:
	void UpdateView(){
		if (!m_UpdateView)return;
		NotifyPropertyChanged("Value");
	}
	IViewModel()
		: m_UpdateView(true){

	}
	property bool UpdateViewFlag {

		bool get() {
			return m_UpdateView;
		}

		void set(bool value) {
			m_UpdateView = value;
		}

	}

	virtual ~IViewModel() {}
protected:
	bool m_UpdateView;
};
template<class T,class Func>
ref class TemlateViewModel : public IViewModel{
public:

	TemlateViewModel(T* pf, Func collback)
		: _dataModel(new DataModel<T, Func>(pf, collback))
	{
	}

	virtual ~TemlateViewModel() {
		this->!TemlateViewModel();
	}

	!TemlateViewModel() {
		delete _dataModel;
	}

protected:
	DataModel<T, Func> *_dataModel;
};
template<class T, class Func>
ref class TemlateViewModel3 : public IViewModel{
public:

	TemlateViewModel3(T* pf1, Func collback1, T* pf2, Func collback2, T* pf3, Func collback3)
		: _dataModel1(new DataModel<T, Func>(pf1, collback1))
		, _dataModel2(new DataModel<T, Func>(pf2, collback2))
		, _dataModel3(new DataModel<T, Func>(pf3, collback3))
	{
	}

	virtual ~TemlateViewModel3() {
		this->!TemlateViewModel3();
	}

	!TemlateViewModel3() {
		delete _dataModel1;
		delete _dataModel2;
		delete _dataModel3;
	}

protected:
	DataModel<T, Func> *_dataModel1;
	DataModel<T, Func> *_dataModel2;
	DataModel<T, Func> *_dataModel3;
};
template<class T, class Func>
ref class TemlateViewModel4 : public IViewModel{
public:

	TemlateViewModel4(T* pf1, Func collback1, T* pf2, Func collback2, T* pf3, Func collback3, T* pf4, Func collback4)
		: _dataModel1(new DataModel<T, Func>(pf1, collback1))
		, _dataModel2(new DataModel<T, Func>(pf2, collback2))
		, _dataModel3(new DataModel<T, Func>(pf3, collback3))
		, _dataModel4(new DataModel<T, Func>(pf4, collback4))
	{
	}

	virtual ~TemlateViewModel4() {
		this->!TemlateViewModel4();
	}

	!TemlateViewModel4() {
		delete _dataModel1;
		delete _dataModel2;
		delete _dataModel3;
		delete _dataModel4;
	}

protected:
	DataModel<T, Func> *_dataModel1;
	DataModel<T, Func> *_dataModel2;
	DataModel<T, Func> *_dataModel3;
	DataModel<T, Func> *_dataModel4;
};

//template <class R, class T>
//R^ lexical_cast(const T* p){
//	return (R)p;
//}
//template <>
//String^ lexical_cast(const char* p){
//	return gcnew String(p);
//}

template <class T>
String^ lexical_cast_(T& p);

template <class R, class T>
R lexical_cast(T^ p);

template <class T>
ref class ViewModel : public TemlateViewModel<T, std::function<void(T)>> {
public:

	ViewModel(T* fp, std::function<void(T)> collback)
		:TemlateViewModel(fp, collback){
	}
	property String^ Value {

		String^ get() {
			return lexical_cast_(_dataModel->get());
		}

		void set(String^ value) {
			if (_dataModel->set(lexical_cast<T,String>(value)))
				NotifyPropertyChanged("Value");
		}

	}
};

template <class T>
ref class ColorViewModel : public TemlateViewModel4<T, std::function<void(T)>> {
public:

	ColorViewModel(T* fp1, std::function<void(T)> collback1, T* fp2, std::function<void(T)> collback2, T* fp3, std::function<void(T)> collback3, T* fp4, std::function<void(T)> collback4)
		:TemlateViewModel4(fp1, collback1, fp2, collback2, fp3, collback3, fp4, collback4){
	}
	property Color Value {

		Color get() {
			return Color::FromArgb(_dataModel4->get() * 255, _dataModel1->get() * 255, _dataModel2->get() * 255, _dataModel3->get() * 255);
		}

		void set(Color value) {
			_dataModel1->set(lexical_cast<T, String>((value.R / 255.0f).ToString()));
			_dataModel2->set(lexical_cast<T, String>((value.G / 255.0f).ToString()));
			_dataModel3->set(lexical_cast<T, String>((value.B / 255.0f).ToString()));
			_dataModel4->set(lexical_cast<T, String>((value.A / 255.0f).ToString()));

			NotifyPropertyChanged("Value");
		}

	}
};
template <class T>
ref class Color3ViewModel : public TemlateViewModel3<T, std::function<void(T)>> {
public:

	Color3ViewModel(T* fp1, std::function<void(T)> collback1, T* fp2, std::function<void(T)> collback2, T* fp3, std::function<void(T)> collback3)
		:TemlateViewModel3(fp1, collback1, fp2, collback2, fp3, collback3){
	}
	property Color Value {

		Color get() {
			return Color::FromRgb(_dataModel1->get() * 255, _dataModel2->get() * 255, _dataModel3->get() * 255);
		}

		void set(Color value) {
			_dataModel1->set(lexical_cast<T, String>((value.R / 255.0f).ToString()));
			_dataModel2->set(lexical_cast<T, String>((value.G / 255.0f).ToString()));
			_dataModel3->set(lexical_cast<T, String>((value.B / 255.0f).ToString()));

			NotifyPropertyChanged("Value");
		}

	}
};

//ref class FloatViewModel : public ViewModel<float,std::function<void(float)>> {
//public:
//
//	FloatViewModel(float* fp, std::function<void(float)> collback)
//		:ViewModel(fp, collback){
//	}
//	property String^ Value {
//
//		String^ get() {
//			return lexical_cast(_dataModel->get());
//		}
//
//		void set(String^ value) {
//			if (_dataModel->set(lexical_cast<float>(value)))
//				NotifyPropertyChanged("Value");
//		}
//
//	}
//};
//ref class BoolViewModel : public ViewModel<bool, BoolCollback> {
//public:
//
//	BoolViewModel(bool *fp, BoolCollback collback)
//		:ViewModel(fp, collback){
//	}
//	property String^ Value {
//
//		String^ get() {
//			return lexical_cast(_dataModel->get());
//		}
//
//		void set(String^ value) {
//			if (_dataModel->set())
//				NotifyPropertyChanged("Value");
//		}
//
//	}
//};
//
//#include < vcclr.h >
//ref class StringViewModel : public ViewModel<std::string, StringCollback> {
//public:
//
//	StringViewModel(std::string *fp, StringCollback collback)
//		:ViewModel(fp, collback){
//	}
//	property String^ Value {
//
//		String^ get() {
//			return gcnew String(_dataModel->get());
//		}
//
//		void set(String^ value) {
//			pin_ptr<const wchar_t> wch = PtrToStringChars(value);
//			size_t convertedChars = 0;
//			size_t  sizeInBytes = ((value->Length + 1) * 2);
//			char    *ch = (char *)malloc(sizeInBytes);
//			wcstombs_s(&convertedChars,
//				ch, sizeInBytes,
//				wch, sizeInBytes);
//			if (_dataModel->set(ch))
//				NotifyPropertyChanged("Value");
//
//			free(ch);
//		}
//
//	}
//};

//ref class IntViewModel : public ViewModel<int,void*> {
//public:
//
//	IntViewModel(int* ip,void* null)
//		:ViewModel(ip,null){
//	}
//	property String^ Value {
//
//		String^ get() {
//			return System::Convert::ToString(_dataModel->get());
//		}
//
//		void set(String^ value) {
//			if (_dataModel->set(System::Convert::ToInt32(value)))
//				NotifyPropertyChanged("Value");
//		}
//
//	}
//};


class CallBackDataModel{
public:
	CallBackDataModel(std::function<void()> collback)
		: m_collback(collback){
	}
	~CallBackDataModel(){
	}

	void Call(System::Object ^sender, System::Windows::RoutedEventArgs ^e) {
		auto coll = new std::function<void()>();
		*coll = [=](){ m_collback(); delete coll; };
		Data::MyPostMessage(MyWindowMessage::ChangeParamComponent, coll);
		e->Handled = true;
	}

private:
	std::function<void()> m_collback;
};
ref class CallBackViewModel{
public:

	CallBackViewModel(std::function<void()> collback)
		: _dataModel(new CallBackDataModel(collback))
	{
	}

	virtual ~CallBackViewModel() {
		this->!CallBackViewModel();
	}

	!CallBackViewModel() {
		delete _dataModel;
	}

	void Call(System::Object ^sender, System::Windows::RoutedEventArgs ^e) {
		_dataModel->Call(sender, e);
	}

protected:
	CallBackDataModel *_dataModel;
};