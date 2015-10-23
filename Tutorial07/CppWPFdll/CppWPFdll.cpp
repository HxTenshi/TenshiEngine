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


//リソースからXAMLを作成する
FrameworkElement ^LoadContentsFromResource(int resourceId) {
	// モジュールハンドルの取得
	HMODULE module = GetModuleHandle(L"CppWPFdll");
	// リソースを見つける
	HRSRC resource = FindResource(module, MAKEINTRESOURCE(resourceId), RT_HTML);
	// リソースのデータサイズを取得する
	DWORD dataSize = SizeofResource(module, resource);
	// リソースデータのグローバルハンドルを取得する
	HGLOBAL dataHandle = LoadResource(module, resource);
	// リソースのデータを取得する
	const char *data = static_cast<const char *>(LockResource(dataHandle));

	// リソースを最後がヌル文字の文字列にするためにバッファーにコピーする
	std::vector<char> xaml(dataSize + 1);
	std::copy(data, data + dataSize, xaml.begin());

	// リソースの解放
	UnlockResource(dataHandle);
	FreeResource(dataHandle);

	// XAMLからオブジェクト化
	StringReader ^reader = gcnew StringReader(gcnew String(xaml.data()));
	XamlXmlReader ^xamlXmlReader = gcnew XamlXmlReader(reader);
	XamlObjectWriter ^xamlObjectWriter = gcnew XamlObjectWriter(xamlXmlReader->SchemaContext);
	while (xamlXmlReader->Read()) {
		xamlObjectWriter->WriteNode(xamlXmlReader);
	}
	return (FrameworkElement ^)xamlObjectWriter->Result;
}


#include "MainWindow.h"

void OnKeyDownHandler(Object ^sender, System::Windows::Input::KeyEventArgs ^e)
{
	if (e->Key == System::Windows::Input::Key::Return)
	{
		auto t = (TextBox^)sender;
		auto sp = (Panel^)t->Parent;
		sp->Focusable = true;
		sp->Focus();
		sp->Focusable = false;
		//t->MoveFocus(gcnew System::Windows::Input::TraversalRequest(System::Windows::Input::FocusNavigationDirection::Previous));
	}
}
void TextBoxFocused(Object^ sender, System::Windows::DependencyPropertyChangedEventArgs e){
	if ((*(bool^)e.NewValue)){
		((IViewModel^)((TextBox^)sender)->DataContext)->UpdateView();
		((IViewModel^)((TextBox^)sender)->DataContext)->UpdateViewFlag = false;
	}
	else
		((IViewModel^)((TextBox^)sender)->DataContext)->UpdateViewFlag = true;

}
void BindingInspectorString(TextBox^ textbox, std::string* pf, StringCollback collback){
	auto vm = gcnew StringViewModel(pf, collback);
	textbox->DataContext = vm;
	textbox->SetBinding(TextBox::TextProperty, "Value");
	textbox->IsKeyboardFocusedChanged += gcnew System::Windows::DependencyPropertyChangedEventHandler(&TextBoxFocused);
	textbox->KeyDown += gcnew System::Windows::Input::KeyEventHandler(&OnKeyDownHandler);
}
void BindingInspectorFloat(TextBox^ textbox, float* pf, FloatCollback collback){
	auto vm = gcnew FloatViewModel(pf, collback);
	textbox->DataContext = vm;
	textbox->SetBinding(TextBox::TextProperty, "Value");
	textbox->IsKeyboardFocusedChanged += gcnew System::Windows::DependencyPropertyChangedEventHandler(&TextBoxFocused);
	textbox->KeyDown += gcnew System::Windows::Input::KeyEventHandler(&OnKeyDownHandler);
}
void BindingInspectorFloat(Slider^ slide, float min, float max, float* pf, FloatCollback collback){
	auto vm = gcnew FloatViewModel(pf, collback);
	slide->DataContext = vm;
	slide->SetBinding(Slider::ValueProperty, "Value");
	slide->Minimum = min;
	slide->Maximum = max;
}
void CreateInspectorTextBlock(DockPanel^ dockPanel, String ^text){
	auto com = gcnew TextBlock();
	com->Text = text;
	dockPanel->Children->Add(com);
	DockPanel::SetDock(com, System::Windows::Controls::Dock::Top);
}
void CreateInspectorString(DockPanel^ dockPanel, String^ text, std::string* p, StringCollback collback){
	FrameworkElement ^com = LoadContentsFromResource(IDR_INS_FLOAT);
	dockPanel->Children->Add(com);
	DockPanel::SetDock(com, System::Windows::Controls::Dock::Top);
	auto tb = (TextBlock^)com->FindName("FloatName");
	if (tb)tb->Text = text;
	BindingInspectorString((TextBox^)com->FindName("Value"), p, collback);
}
void CreateInspectorFloat(DockPanel^ dockPanel, String^ text, float* p, FloatCollback collback){
	FrameworkElement ^com = LoadContentsFromResource(IDR_INS_FLOAT);
	dockPanel->Children->Add(com);
	DockPanel::SetDock(com, System::Windows::Controls::Dock::Top);
	auto tb = (TextBlock^)com->FindName("FloatName");
	if (tb)tb->Text = text;
	BindingInspectorFloat((TextBox^)com->FindName("Value"), p,collback);
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

	BindingInspectorFloat((Slider^)com->FindName("Value"), min, max, p, collback);
}
void CreateInspectorVector3(DockPanel^ dockPanel, float* px, FloatCollback collbackx, float* py, FloatCollback collbacky, float* pz, FloatCollback collbackz){
	FrameworkElement ^com = LoadContentsFromResource(IDR_INS_VEC3);
	dockPanel->Children->Add(com);
	DockPanel::SetDock(com, System::Windows::Controls::Dock::Top);
	BindingInspectorFloat((TextBox^)com->FindName("xValue"), px,collbackx);
	BindingInspectorFloat((TextBox^)com->FindName("yValue"), py,collbacky);
	BindingInspectorFloat((TextBox^)com->FindName("zValue"), pz,collbackz);
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
ref class InspectorString : public InspectorData{
public:
	InspectorString(InspectorStringDataSet* data)
		:m_data(data){
	}
	~InspectorString(){
		this->!InspectorString();
	}
	!InspectorString(){
		delete m_data;
	}

	void CreateInspector(DockPanel^ dockPanel) override;
private:
	InspectorStringDataSet *m_data;
};
ref class InspectorFloat : public InspectorData{
public:
	InspectorFloat(InspectorFloatDataSet* data)
		:m_data(data){
	}
	~InspectorFloat(){
		this->!InspectorFloat();
	}
	!InspectorFloat(){
		delete m_data;
	}

	void CreateInspector(DockPanel^ dockPanel) override;
private:
	InspectorFloatDataSet *m_data;
};
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

void InspectorLabel::CreateInspector(DockPanel^ dockPanel){
	CreateInspectorTextBlock(dockPanel, Text);
}
void InspectorString::CreateInspector(DockPanel^ dockPanel){
	CreateInspectorString(dockPanel, gcnew String(m_data->Text.c_str()), m_data->data, m_data->collBack);
}
void InspectorFloat::CreateInspector(DockPanel^ dockPanel){
	CreateInspectorFloat(dockPanel, gcnew String(m_data->Text.c_str()), m_data->data, m_data->collBack);
}
void InspectorFloatSlideBar::CreateInspector(DockPanel^ dockPanel){
	CreateInspectorFloatSlideBar(dockPanel, gcnew String(m_data->Text.c_str()), m_data->_min, m_data->_max, m_data->data, m_data->collBack);
}
void InspectorVector3::CreateInspector(DockPanel^ dockPanel) {
	CreateInspectorTextBlock(dockPanel, gcnew String(m_data->Text.c_str()));
	CreateInspectorVector3(dockPanel, m_data->datax, m_data->collbackX, m_data->datay, m_data->collbackY, m_data->dataz, m_data->collbackZ);
}

// データモデル
//class _DataModel {
//public:
//
//	DataModel()
//		: _value(3.0) {
//	}
//
//	double GetValue() const {
//		return _value;
//	}
//
//	bool SetValue(double value) {
//		if (_value == value) {
//			return false;
//		}
//		_value = value;
//		return true;
//	}
//
//private:
//	double _value;
//};
//
//// ビューモデル
//ref class _ViewModel : public NofityPropertyChanged {
//public:
//
//	property double Value {
//
//		double get() {
//			return _dataModel->GetValue();
//		}
//
//		void set(double value) {
//			if (_dataModel->SetValue(value)) {
//				NotifyPropertyChanged("Value");
//			}
//		}
//
//	}
//
//	ViewModel()
//		: _dataModel(new DataModel())
//	{
//	}
//
//	~ViewModel() {
//		this->!ViewModel();
//	}
//
//	!ViewModel() {
//		delete _dataModel;
//	}
//
//private:
//	DataModel *_dataModel;
//};

HWND g_GameScreenHWND = NULL;
HWND g_GameScreenHWND2 = NULL;


#include <vcclr.h>

#pragma comment(lib, "user32.lib")

#include "../Source/Application/Define.h"


static ref class ViewData{
public:
	static View^ window = nullptr;
};
HWND Data::hWnd = NULL;
void Data::MyPostMessage(MyWindowMessage wm, void* p){
	Data::hWnd = FindWindow(L"TutorialWindowClass", NULL);
	PostMessage(hWnd, WM_MYWMCOLLBACK, (int)wm, (LPARAM)p);
}
void Data::_SendMessage(UINT wm, WPARAM p1, LPARAM p2){
	SendMessage(hWnd, wm, p1,p2);
}

void thredtest(){
	Application a;
	ViewData::window = gcnew View();
	a.Run(ViewData::window);
	delete ViewData::window;
	Data::hWnd = FindWindow(L"TutorialWindowClass", NULL);
	//DestroyWindow(hwnd);
	Data::_SendMessage(WM_DESTROY);
}

namespace Test {

	///////////Fraction///////////

	ref class Fraction {

		float m, d;

	internal:

		Fraction(float m_, float d_) : m(m_), d(d_) {}

		float GetMol(){ return m; }
		float GetDen(){ return d; }
		double GetRatio(){ return (double)m / d; }

	};

	///////////Frac1///////////

	class Frac1 {

		gcroot<Fraction^> frac;
		gcroot<System::Threading::Thread ^>UIthread = nullptr;

	public:

		//gcrootの、->のオーバーロード
		double GetRatio() const { return frac->GetRatio(); }

		Frac1(float mol, float den) : frac(gcnew Fraction(mol, den)){
			UIthread = gcnew System::Threading::Thread(gcnew System::Threading::ThreadStart(thredtest));

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
		frac = new Frac1(1, 2);
	}
	void NativeFraction::Release(){
		delete frac;
	}

	delegate void MyDelegate();
	delegate IntPtr MyDelegateR();
	//delegate void MyDelegateI(IntViewModel ^);
	delegate void MyDelegateF(array<InspectorData^>^);
	delegate void MyDelegateITEM(String^, IntPtr);
	void NativeFraction::CreateComponentWindow(std::vector<InspectorDataSet>& data){
		if (ViewData::window!=nullptr){
			auto del = gcnew MyDelegateF(ViewData::window, &View::CreateComponent);
			//System::Array<std::vector<InspectorData>> ^f;
			int num = data.size();
			array<array<InspectorData^>^> ^a = gcnew array<array<InspectorData^>^>(1);
			a[0] = gcnew array<InspectorData^>(num);
			int i = 0;
			for (auto& d : data){
				if (d.format == InspectorDataFormat::Label){
					a[0][i] = gcnew InspectorLabel((InspectorLabelDataSet*)d.data);
				}
				if (d.format == InspectorDataFormat::String){
					a[0][i] = gcnew InspectorString((InspectorStringDataSet*)d.data);
				}
				if (d.format == InspectorDataFormat::Float){
					a[0][i] = gcnew InspectorFloat((InspectorFloatDataSet*)d.data);
				}
				if (d.format == InspectorDataFormat::Vector3){
					a[0][i] = gcnew InspectorVector3((InspectorVector3DataSet*)d.data);
				}
				if (d.format == InspectorDataFormat::SlideBar){
					a[0][i] = gcnew InspectorFloatSlideBar((InspectorSlideBarDataSet*)d.data);
				}
				i++;
			}
			
			ViewData::window->Dispatcher->BeginInvoke(del, a);

		}
	}
	void NativeFraction::ClearAllComponentWindow(){
		if (ViewData::window != nullptr){
			ViewData::window->Dispatcher->BeginInvoke(gcnew MyDelegate(ViewData::window, &View::ClearAllComponent));

		}
	}

	void NativeFraction::UpdateComponentWindow(){
		if (ViewData::window != nullptr){
			auto del = gcnew MyDelegate(ViewData::window, &View::UpdateView);
			ViewData::window->Dispatcher->BeginInvoke(del);
		}
	}

	void NativeFraction::AddTreeViewItem(std::string& Name, void* ptr){
		if (ViewData::window == nullptr)return;

		auto del = gcnew MyDelegateITEM(ViewData::window, &View::AddItem);
		ViewData::window->Dispatcher->BeginInvoke(del, gcnew String(Name.c_str()), (IntPtr)ptr);
	}

	void NativeFraction::SetMouseEvents(bool* l, bool* r, int* x, int* y){
		if (ViewData::window == nullptr)return;
		ViewData::window->GameScreenData->SetMouseEvents(l, r, x, y);
	}
	void* NativeFraction::GetGameScreenHWND() const {
		void* temp = NULL;
		if (ViewData::window != nullptr){
			temp = ViewData::window->GameScreenHWND;
		}
		return temp;
	}


}