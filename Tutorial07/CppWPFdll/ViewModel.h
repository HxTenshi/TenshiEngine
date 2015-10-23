#pragma once

#include "Using.h"
#include <string>

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
	T get() {
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
ref class ViewModel : public IViewModel{
public:

	ViewModel(T* pf,Func collback)
		: _dataModel(new DataModel<T, Func>(pf, collback))
	{
	}

	virtual ~ViewModel() {
		this->!ViewModel();
	}

	!ViewModel() {
		delete _dataModel;
	}

protected:
	DataModel<T, Func> *_dataModel;
};

ref class FloatViewModel : public ViewModel<float,FloatCollback> {
public:

	FloatViewModel(float* fp, FloatCollback collback)
		:ViewModel(fp, collback){
	}
	property String^ Value {

		String^ get() {
			return System::Convert::ToString(_dataModel->get());
		}

		void set(String^ value) {
			if (_dataModel->set(System::Convert::ToSingle(value)))
				NotifyPropertyChanged("Value");
		}

	}
};

#include < vcclr.h >
ref class StringViewModel : public ViewModel<std::string, StringCollback> {
public:

	StringViewModel(std::string *fp, StringCollback collback)
		:ViewModel(fp, collback){
	}
	property String^ Value {

		String^ get() {
			return gcnew String(_dataModel->get().c_str());
		}

		void set(String^ value) {
			pin_ptr<const wchar_t> wch = PtrToStringChars(value);
			size_t convertedChars = 0;
			size_t  sizeInBytes = ((value->Length + 1) * 2);
			char    *ch = (char *)malloc(sizeInBytes);
			wcstombs_s(&convertedChars,
				ch, sizeInBytes,
				wch, sizeInBytes);
			if (_dataModel->set(ch))
				NotifyPropertyChanged("Value");

			free(ch);
		}

	}
};

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
