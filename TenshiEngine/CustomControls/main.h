
#pragma once

#include "../CppWPFdll/Using.h"
//#include "../CppWPFdll/Stdafx.h"

#include <vcclr.h>

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
		Person(String ^name, MyList ^child) :mName(name), mChildren(child), mDataPtr(NULL), mParent(nullptr), mThisIntPtr(NULL), mIsSelect(false)
		{
			mThisIntPtr = new gcroot<Person^>(this);
		}
	private:
		~Person(){
			this->!Person();
		}
		!Person(){
			delete mThisIntPtr;
		}
	public:
		property String^ Name{ String^ get(){ return mName; } void set(String^ s){ mName = s; NotifyPropertyChanged("Name"); } }
		property ObservableCollection<Person^>^ Children{ ObservableCollection<Person^>^ get(){ return mChildren->list; } void set(ObservableCollection<Person^>^ s){ mChildren->list = s; NotifyPropertyChanged("Children"); } }
		property IntPtr DataPtr{ IntPtr get(){ return mDataPtr; } void set(IntPtr ptr){ mDataPtr = ptr; } }
		property IntPtr ThisIntPtr{ IntPtr get(){ return (IntPtr)mThisIntPtr; } }
		property Person^ Parent{ Person^ get(){ return mParent; } }
		property bool IsSelected{ bool get(){ return mIsSelect; } void set(bool v){ mIsSelect = v; NotifyPropertyChanged("IsSelected"); }}
		void Add(Person^ item){
			if (item->mParent)
				item->mParent->Children->Remove(item);
			Children->Add(item);
			item->mParent = this;
			//NotifyPropertyChanged("Children");
		}
		void RemoveSelf(){
			if (mParent)
				mParent->Children->Remove(this);
			DataPtr = (IntPtr)NULL;
			Name = "sinderu";
			delete this;
		}
	private:
		String ^mName;
		MyList ^mChildren;
		IntPtr mDataPtr;
		Person ^mParent;
		bool mIsSelect;
		gcroot<Person^> *mThisIntPtr;
	};
}