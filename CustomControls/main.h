
#pragma once

#include "../Tutorial07/CppWPFdll/Using.h"
//#include "../Tutorial07/CppWPFdll/Stdafx.h"

#define _EXPORTING
#ifdef _EXPORTING
#define CLASS_DECLSPEC    __declspec(dllexport)
#else
#define CLASS_DECLSPEC    __declspec(dllimport)
#endif

#define NULL 0

public ref class _NofityPropertyChanged : public INotifyPropertyChanged
{
public:
	virtual event PropertyChangedEventHandler ^PropertyChanged;
protected:
	void NotifyPropertyChanged(String ^info)
	{
		PropertyChanged(this, gcnew PropertyChangedEventArgs(info));
	}
};

using System::Windows::Data::CollectionView;
namespace TestContent{
	CLASS_DECLSPEC public ref class Contents : public TreeView{
	public:

		virtual event PropertyChangedEventHandler ^PropertyChanged;
	protected:
		void NotifyPropertyChanged(String ^info)
		{
			PropertyChanged(this, gcnew PropertyChangedEventArgs(info));
		}

		void change(Object ^sender, System::Windows::RoutedPropertyChangedEventArgs<Object^> ^e){
			System::Windows::MessageBox::Show(((TextBlock^)sender)->Text);
			//auto t = (TextBlock^)Items->CurrentItem;
			NotifyPropertyChanged("SelectItemText");
		}
	public:
		Contents(){
			SelectedItemChanged += gcnew System::Windows::RoutedPropertyChangedEventHandler<Object^>(this, &Contents::change);
			auto t = gcnew TextBlock();
			t->Text = "u";
			Items->Add(t);
		}

		property ItemCollection^ ItemList{
			ItemCollection^ get(){
				return Items;
			}
		}
		property String^ SelectItemText{
			String^ get(){
				return SelectedItem ? ((TextBlock^)SelectedItem)->Text : "NULL";
			}

			void set(String^ text){
				System::Windows::MessageBox::Show("set");
			}
		}
	};

	ref class Person;
	CLASS_DECLSPEC public ref class MyList
	{
	public:
		MyList(...array<Person^> ^child)
		{
			mChildren = gcnew ObservableCollection<Person^>;
			int num = child->GetLength(0);
			for (int i = 0; i < num; i++)
				mChildren->Add(child[i]);
		}
		property ObservableCollection<Person^>^ list{ ObservableCollection<Person^>^ get(){ return mChildren; } void set(ObservableCollection<Person^>^ s){ mChildren = s; } }
	private:
		ObservableCollection<Person^> ^mChildren;
		//List<Person^>^ mChildren;
	};

	CLASS_DECLSPEC public ref class Person : public _NofityPropertyChanged
	{
	public:
		Person(String ^name, MyList ^child) :mName(name), mChildren(child), mDataPtr(NULL){}
		property String^ Name{ String^ get(){ return mName; } void set(String^ s){ mName = s; NotifyPropertyChanged("Name"); } }
		property ObservableCollection<Person^>^ Children{ ObservableCollection<Person^>^ get(){ return mChildren->list; } void set(ObservableCollection<Person^>^ s){ mChildren->list = s; NotifyPropertyChanged("Children"); } }
		property IntPtr DataPtr{ IntPtr get(){ return mDataPtr; } void set(IntPtr ptr){ mDataPtr = ptr; } }

		void Add(Person^ item){
			Children->Add(item);
			//NotifyPropertyChanged("Children");
		}
	private:
		String ^mName;
		MyList ^mChildren;
		IntPtr mDataPtr;
	};
}