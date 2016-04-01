// これは メイン DLL ファイルです。

#include "stdafx.h"

#define _EXPORTING
#include "CppWPFdll.h"

#include "resource.h"
#include <Windows.h>
#include <algorithm>
#include <vector>
#include "Using.h"
#include "ViewModel.h"

#include "TemplateFunction.h"


#include <vcclr.h>

#pragma comment(lib, "user32.lib")

#include "../Source/Application/Define.h"


#include "MainWindow.h"
static ref class ViewData{
public:
	static View^ window;
};


void CreateInspectorTextBlock(DockPanel^ dockPanel, String ^text){
	auto com = gcnew TextBlock();
	com->Text = text;
	dockPanel->Children->Add(com);
	DockPanel::SetDock(com, System::Windows::Controls::Dock::Top);
}
template <>
void _CreateInspector(DockPanel^ dockPanel, String^ text, bool* p, std::function<void(bool)> collback){
	FrameworkElement ^com = LoadContentsFromResource(IDR_INS_CHECKBOX);
	dockPanel->Children->Add(com);
	DockPanel::SetDock(com, System::Windows::Controls::Dock::Top);
	auto tb = (TextBlock^)com->FindName("FloatName");
	if (tb)tb->Text = text;
	BindingInspector<bool>((CheckBox^)com->FindName("Value"), p, collback);

}
template <>
void _CreateInspector<std::string>(DockPanel^ dockPanel, String^ text, std::string* p, StringCollback collback){
	FrameworkElement ^com = LoadContentsFromResource(IDR_INS_FLOAT);
	dockPanel->Children->Add(com);
	DockPanel::SetDock(com, System::Windows::Controls::Dock::Top);
	auto tb = (TextBlock^)com->FindName("FloatName");
	if (tb)tb->Text = text;

	auto textbox = (TextBox^)com->FindName("Value");
	BindingInspector<std::string>(textbox, p, collback);

	textbox->AllowDrop = true;

	textbox->AddHandler(TextBox::PreviewDragEnterEvent, gcnew DragEventHandler(ViewData::window, &View::Commponent_ViewModel_OnDragOver), true);
	textbox->AddHandler(TextBox::PreviewDragOverEvent, gcnew DragEventHandler(ViewData::window, &View::Commponent_ViewModel_OnDragOver), true);
	textbox->AddHandler(TextBox::DragEnterEvent, gcnew DragEventHandler(ViewData::window, &View::Commponent_ViewModel_OnDragOver), true);
	textbox->AddHandler(TextBox::DragOverEvent, gcnew DragEventHandler(ViewData::window, &View::Commponent_ViewModel_OnDragOver), true);

	textbox->PreviewDrop += gcnew DragEventHandler(ViewData::window, &View::Commponent_ViewModel_OnDrop);
	textbox->Drop += gcnew DragEventHandler(ViewData::window, &View::Commponent_ViewModel_OnDrop);

}

void CreateInspectorFloatSlideBar(DockPanel^ dockPanel, String^ text,float min, float max, float* p, FloatCollback collback){
	FrameworkElement ^com = LoadContentsFromResource(IDR_INS_SLIDEBAR);
	dockPanel->Children->Add(com);
	DockPanel::SetDock(com, System::Windows::Controls::Dock::Top);
	auto tb = (TextBlock^)com->FindName("FloatName");
	if (tb)tb->Text = text;
	auto t = gcnew TextBlock();
	t->DataContext = com->FindName("Value");
	t->SetBinding(TextBlock::TextProperty, "Value");
	dockPanel->Children->Add(t);
	DockPanel::SetDock(t, System::Windows::Controls::Dock::Top);

	BindingInspector<float>((Slider^)com->FindName("Value"), min, max, p, collback);
}
void CreateInspectorVector3(DockPanel^ dockPanel, float* px, FloatCollback collbackx, float* py, FloatCollback collbacky, float* pz, FloatCollback collbackz){
	FrameworkElement ^com = LoadContentsFromResource(IDR_INS_VEC3);
	dockPanel->Children->Add(com);
	DockPanel::SetDock(com, System::Windows::Controls::Dock::Top);
	BindingInspector<float>((TextBox^)com->FindName("xValue"), px,collbackx);
	BindingInspector<float>((TextBox^)com->FindName("yValue"), py,collbacky);
	BindingInspector<float>((TextBox^)com->FindName("zValue"), pz,collbackz);
}
void CreateInspectorVector2(DockPanel^ dockPanel, float* px, FloatCollback collbackx, float* py, FloatCollback collbacky){
	FrameworkElement ^com = LoadContentsFromResource(IDR_INS_VEC2);
	dockPanel->Children->Add(com);
	DockPanel::SetDock(com, System::Windows::Controls::Dock::Top);
	BindingInspector<float>((TextBox^)com->FindName("xValue"), px, collbackx);
	BindingInspector<float>((TextBox^)com->FindName("yValue"), py, collbacky);
}
void CreateInspectorColor(DockPanel^ dockPanel, String^ text, float* pr, FloatCollback collbackR, float* pg, FloatCollback collbackG, float* pb, FloatCollback collbackB, float* pa, FloatCollback collbackA){
	FrameworkElement ^com = LoadContentsFromResource(IDR_INS_COLOR);
	dockPanel->Children->Add(com);
	DockPanel::SetDock(com, System::Windows::Controls::Dock::Top);
	auto tb = (TextBlock^)com->FindName("FloatName");
	if (tb)tb->Text = text;

	BindingInspector<float>((System::Windows::Shapes::Rectangle^)com->FindName("Value"), pr, collbackR, pg, collbackG, pb, collbackB, pa, collbackA);
}

ref class InspectorLabel : public InspectorData{
public:
	InspectorLabel(InspectorLabelDataSet* data)
		: Text(gcnew String(data->Text.c_str())){
		delete data;
	}
	void CreateInspector(DockPanel^ dockPanel) override;
private:
	String ^Text;
};


ref class InspectorFloatSlideBar : public InspectorData{
public:
	InspectorFloatSlideBar(InspectorSlideBarDataSet* data)
		:m_data(data){
	}
	~InspectorFloatSlideBar(){
		this->!InspectorFloatSlideBar();
	}
	!InspectorFloatSlideBar(){
		delete m_data;
	}

	void CreateInspector(DockPanel^ dockPanel) override;
private:
	InspectorSlideBarDataSet *m_data;
};
//ref class InspectorString : public InspectorData{
//public:
//	InspectorString(InspectorStringDataSet* data)
//		:m_data(data){
//	}
//	~InspectorString(){
//		this->!InspectorString();
//	}
//	!InspectorString(){
//		delete m_data;
//	}
//
//	void CreateInspector(DockPanel^ dockPanel) override;
//private:
//	InspectorStringDataSet *m_data;
//};
//ref class InspectorFloat : public InspectorData{
//public:
//	InspectorFloat(InspectorFloatDataSet* data)
//		:m_data(data){
//	}
//	~InspectorFloat(){
//		this->!InspectorFloat();
//	}
//	!InspectorFloat(){
//		delete m_data;
//	}
//
//	void CreateInspector(DockPanel^ dockPanel) override;
//private:
//	InspectorFloatDataSet *m_data;
//};
//ref class InspectorBool : public InspectorData{
//public:
//	InspectorBool(InspectorBoolDataSet* data)
//		:m_data(data){
//	}
//	~InspectorBool(){
//		this->!InspectorBool();
//	}
//	!InspectorBool(){
//		delete m_data;
//	}
//
//	void CreateInspector(DockPanel^ dockPanel) override;
//private:
//	InspectorBoolDataSet *m_data;
//};
ref class InspectorVector3 : public InspectorData{
public:
	InspectorVector3(InspectorVector3DataSet* data)
		: m_data(data){
	}
	~InspectorVector3(){
		this->!InspectorVector3();
	}
	!InspectorVector3(){
		delete m_data;
	}

	void CreateInspector(DockPanel^ dockPanel) override;
private:
	InspectorVector3DataSet *m_data;
};

ref class InspectorVector2 : public InspectorData{
public:
	InspectorVector2(InspectorVector2DataSet* data)
		: m_data(data){
	}
	~InspectorVector2(){
		this->!InspectorVector2();
	}
	!InspectorVector2(){
		delete m_data;
	}

	void CreateInspector(DockPanel^ dockPanel) override;
private:
	InspectorVector2DataSet *m_data;
};
ref class InspectorColor : public InspectorData{
public:
	InspectorColor(InspectorColorDataSet* data)
		: m_data(data){
	}
	~InspectorColor(){
		this->!InspectorColor();
	}
	!InspectorColor(){
		delete m_data;
	}

	void CreateInspector(DockPanel^ dockPanel) override;
private:
	InspectorColorDataSet *m_data;
};


void InspectorLabel::CreateInspector(DockPanel^ dockPanel){
	CreateInspectorTextBlock(dockPanel, Text);
}
void InspectorFloatSlideBar::CreateInspector(DockPanel^ dockPanel){
	CreateInspectorFloatSlideBar(dockPanel, gcnew String(m_data->Text.c_str()), m_data->_min, m_data->_max, m_data->data, m_data->collBack);
}
void InspectorVector3::CreateInspector(DockPanel^ dockPanel) {
	CreateInspectorTextBlock(dockPanel, gcnew String(m_data->Text.c_str()));
	CreateInspectorVector3(dockPanel, m_data->datax, m_data->collbackX, m_data->datay, m_data->collbackY, m_data->dataz, m_data->collbackZ);
}
void InspectorVector2::CreateInspector(DockPanel^ dockPanel) {
	CreateInspectorTextBlock(dockPanel, gcnew String(m_data->Text.c_str()));
	CreateInspectorVector2(dockPanel, m_data->datax, m_data->collbackX, m_data->datay, m_data->collbackY);
}
void InspectorColor::CreateInspector(DockPanel^ dockPanel) {
	CreateInspectorColor(dockPanel, gcnew String(m_data->Text.c_str()), m_data->datar, m_data->collbackR, m_data->datag, m_data->collbackG, m_data->datab, m_data->collbackB, m_data->dataa, m_data->collbackA);
}


HWND Data::hWnd = NULL;
void Data::MyPostMessage(MyWindowMessage wm, void* p){
	Data::hWnd = FindWindow(L"TutorialWindowClass", NULL);
	PostMessage(hWnd, WM_MYWMCOLLBACK, (int)wm, (LPARAM)p);
}
void Data::_SendMessage(UINT wm, WPARAM p1, LPARAM p2){
	SendMessage(hWnd, wm, p1,p2);
}

void WindowMainThred(){
	Application a;
	ViewData::window = gcnew View();
	a.Run(ViewData::window);
	delete ViewData::window;
	Data::hWnd = FindWindow(L"TutorialWindowClass", NULL);
	//DestroyWindow(hwnd);
	Data::_SendMessage(WM_DESTROY);
}

namespace Test {

	///////////Frac1///////////

	class Frac1 {

		gcroot<System::Threading::Thread ^>UIthread = nullptr;

	public:

		Frac1(){

			UIthread = gcnew System::Threading::Thread(gcnew System::Threading::ThreadStart(WindowMainThred));

			UIthread->SetApartmentState(System::Threading::ApartmentState::STA);
			UIthread->Start();
		}

		void _CreateWindow(){
		}

		~Frac1(){ //デストラクタで.NETのメッセージボックスに分数表示
			//System::Windows::MessageBox::Show(frac->GetMol() + "/" + frac->GetDen());
		}
	};


	///////////NativeFractionの実装///////////

	NativeFraction::NativeFraction(){
	}

	NativeFraction::~NativeFraction(){
	}

	void NativeFraction::Initialize(){
		frac = new Frac1();
	}
	void NativeFraction::Release(){
		delete frac;
	}

	delegate void MyDelegate();
	delegate IntPtr MyDelegateR();
	//delegate void MyDelegateI(IntViewModel ^);
	delegate void MyDelegateF(String^,IntPtr,array<InspectorData^>^);
	delegate void MyDelegateITEM(String^, IntPtr);
	delegate void MyDelegateCOM(String^);
	delegate void MyDelegateI2(IntPtr, IntPtr);
	delegate void MyDelegateI1(IntPtr);
	void NativeFraction::CreateComponentWindow(const std::string& ComponentName, void* comptr, std::vector<InspectorDataSet>& data){
		if (ViewData::window!=nullptr){
			auto del = gcnew MyDelegateF(ViewData::window, &View::CreateComponent);
			//System::Array<std::vector<InspectorData>> ^f;
			int num = data.size();
			array<InspectorData^> ^a = gcnew array<InspectorData^>(num);
			int i = 0;
			for (auto& d : data){
				if (d.format == InspectorDataFormat::Label){
					a[i] = gcnew InspectorLabel((InspectorLabelDataSet*)d.data);
				}
				if (d.format == InspectorDataFormat::String){
					a[i] = gcnew Inspector<std::string>((TemplateInspectorDataSet<std::string>*)d.data);
				}
				if (d.format == InspectorDataFormat::Float){
					a[i] = gcnew Inspector<float>((TemplateInspectorDataSet<float>*)d.data);
				}
				if (d.format == InspectorDataFormat::Bool){
					a[i] = gcnew Inspector<bool>((TemplateInspectorDataSet<bool>*)d.data);
				}
				if (d.format == InspectorDataFormat::Int){
					a[i] = gcnew Inspector<int>((TemplateInspectorDataSet<int>*)d.data);
				}
				if (d.format == InspectorDataFormat::Vector2){
					a[i] = gcnew InspectorVector2((InspectorVector2DataSet*)d.data);
				}
				if (d.format == InspectorDataFormat::Vector3){
					a[i] = gcnew InspectorVector3((InspectorVector3DataSet*)d.data);
				}
				if (d.format == InspectorDataFormat::SlideBar){
					a[i] = gcnew InspectorFloatSlideBar((InspectorSlideBarDataSet*)d.data);
				}
				if (d.format == InspectorDataFormat::Color){
					a[i] = gcnew InspectorColor((InspectorColorDataSet*)d.data);
				}
				i++;
			}
			
			ViewData::window->Dispatcher->BeginInvoke(del, gcnew String(ComponentName.c_str()),(IntPtr)comptr, a);

		}
	}

	void NativeFraction::ChangeTreeViewName(void* ptr, std::string& name){
		if (ViewData::window != nullptr){
			auto del = gcnew MyDelegateITEM(ViewData::window, &View::ChangeTreeViewName);
			ViewData::window->Dispatcher->BeginInvoke(del, gcnew String(name.c_str()), (IntPtr)ptr);
		}
	}

	void NativeFraction::ClearAllComponentWindow(){
		if (ViewData::window != nullptr){
			ViewData::window->Dispatcher->BeginInvoke(gcnew MyDelegate(ViewData::window, &View::ClearAllComponent));

		}
	}

	void NativeFraction::ClearTreeViewItem(void* ptr){
		if (ViewData::window != nullptr){
			auto del = gcnew MyDelegateI1(ViewData::window, &View::ClearTreeViewItem);
			ViewData::window->Dispatcher->BeginInvoke(del, (IntPtr)ptr);
		}
	}

	void NativeFraction::UpdateComponentWindow(){
		if (ViewData::window != nullptr){
			auto del = gcnew MyDelegate(ViewData::window, &View::UpdateView);
			ViewData::window->Dispatcher->BeginInvoke(del);
		}
	}

	void NativeFraction::CreateContextMenu_AddComponent(const std::string& ComponentName){
		if (ViewData::window != nullptr){
			auto del = gcnew MyDelegateCOM(ViewData::window, &View::CreateContextMenu_AddComponent);
			ViewData::window->Dispatcher->BeginInvoke(del, gcnew String(ComponentName.c_str()));
		}
	}

	void NativeFraction::AddTreeViewItem(const std::string& Name, void* ptr){
		if (ViewData::window == nullptr)return;

		auto del = gcnew MyDelegateITEM(ViewData::window, &View::AddItem);
		ViewData::window->Dispatcher->BeginInvoke(del, gcnew String(Name.c_str()), (IntPtr)ptr);
	}
	void NativeFraction::SetParentTreeViewItem(void* parent, void* child){
		if (ViewData::window == nullptr)return;

		auto del = gcnew MyDelegateI2(ViewData::window, &View::SetParent);
		ViewData::window->Dispatcher->BeginInvoke(del, (IntPtr)parent, (IntPtr)child);
	}

	void NativeFraction::SetMouseEvents(bool* l, bool* r, int* x, int* y, int* wx, int* wy){
		if (ViewData::window == nullptr)return;
		ViewData::window->GameScreenData->SetMouseEvents(l, r, x, y, wx, wy);
	}
	void* NativeFraction::GetGameScreenHWND() const {
		void* temp = NULL;
		if (ViewData::window != nullptr){
			temp = ViewData::window->GameScreenHWND;
		}
		return temp;
	}

}