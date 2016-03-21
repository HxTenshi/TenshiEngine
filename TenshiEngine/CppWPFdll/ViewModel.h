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
