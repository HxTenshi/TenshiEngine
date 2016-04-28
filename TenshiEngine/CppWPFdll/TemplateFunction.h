#pragma once

#include "InspectorDataSet.h"

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


template <class T>
void BindingInspector(TextBox^ textbox, T* pf, std::function<void(T)> collback){
	auto vm = gcnew ViewModel<T>(pf, collback);
	textbox->DataContext = vm;
	textbox->SetBinding(TextBox::TextProperty, "Value");
	textbox->IsKeyboardFocusedChanged += gcnew System::Windows::DependencyPropertyChangedEventHandler(&TextBoxFocused);
	textbox->KeyDown += gcnew System::Windows::Input::KeyEventHandler(&OnKeyDownHandler);
}

template <class T>
void BindingInspector(CheckBox^ textbox, T* pf, std::function<void(T)> collback){
	auto vm = gcnew ViewModel<T>(pf, collback);
	textbox->DataContext = vm;
	textbox->SetBinding(CheckBox::IsCheckedProperty, "Value");
}

template <class T>
void BindingInspector(Slider^ slide, T min, T max, T* pf, std::function<void(T)> collback){
	auto vm = gcnew ViewModel<T>(pf, collback);
	slide->DataContext = vm;
	slide->SetBinding(Slider::ValueProperty, "Value");
	slide->Minimum = min;
	slide->Maximum = max;
}
template <class T>
void BindingInspector(System::Windows::Shapes::Rectangle^ rectangle, T* pf1, std::function<void(T)> collback1, T* pf2, std::function<void(T)> collback2, T* pf3, std::function<void(T)> collback3, T* pf4, std::function<void(T)> collback4){
	//rectangle->SetBinding(Slider::ValueProperty, "Value");
	rectangle->Fill = gcnew SolidColorBrush(Color::FromArgb(255, 255, 255, 255));
	auto scb = (SolidColorBrush^)rectangle->Fill;
	//rectangle->SetBinding(SolidColorBrush::ColorProperty, "Value");
	auto binding = gcnew System::Windows::Data::Binding("Value");

	if (pf4 == NULL){

		auto vm = gcnew Color3ViewModel<T>(pf1, collback1, pf2, collback2, pf3, collback3);
		rectangle->DataContext = vm;
	}
	else{
		auto vm = gcnew ColorViewModel<T>(pf1, collback1, pf2, collback2, pf3, collback3, pf4, collback4);
		rectangle->DataContext = vm;
	}


	binding->Mode = BindingMode::TwoWay;
	System::Windows::Data::BindingOperations::SetBinding(scb, SolidColorBrush::ColorProperty, binding);

	rectangle->AddHandler(System::Windows::Controls::Canvas::MouseLeftButtonDownEvent, gcnew System::Windows::Input::MouseButtonEventHandler(ViewData::window, &View::CreateColorPickerWindow), true);
}

ref class XAMLResource{
public:
	static System::Collections::Generic::Dictionary<int, String^> ^DataBase = gcnew System::Collections::Generic::Dictionary<int, String^>();
};

FrameworkElement ^CreateFrom(String ^xaml) {

	auto reader = gcnew StringReader(xaml);

	XamlXmlReader ^xamlXmlReader = gcnew XamlXmlReader(reader);

	XamlObjectWriter ^xamlObjectWriter = gcnew XamlObjectWriter(xamlXmlReader->SchemaContext);
	while (xamlXmlReader->Read()) {
		xamlObjectWriter->WriteNode(xamlXmlReader);
	}

	return (FrameworkElement ^)xamlObjectWriter->Result;
}
//リソースからXAMLを作成する
FrameworkElement ^LoadContentsFromResource(int resourceId) {
	auto data = XAMLResource::DataBase[resourceId];
	if (!data)return nullptr;
	return CreateFrom(data);
}

void InitializeLoadContentsFromResource(int startID, int endID) {
	// モジュールハンドルの取得
	HMODULE module = GetModuleHandle(L"CppWPFdll");

	if (module == NULL)return;

	for (int i = startID; i <= endID; i++){

		// リソースを見つける
		HRSRC resource = FindResource(module, MAKEINTRESOURCE(i), RT_HTML);
		if (resource == NULL)return;
		// リソースのデータサイズを取得する
		DWORD dataSize = SizeofResource(module, resource);
		// リソースデータのグローバルハンドルを取得する
		HGLOBAL dataHandle = LoadResource(module, resource);
		if (dataHandle == NULL)return;
		// リソースのデータを取得する
		const char *data = static_cast<const char *>(LockResource(dataHandle));

		// リソースを最後がヌル文字の文字列にするためにバッファーにコピーする
		std::vector<char> xaml(dataSize + 1);
		std::copy(data, data + dataSize, xaml.begin());
		xaml[dataSize] = '\0';


		// リソースの解放
		UnlockResource(dataHandle);
		FreeResource(dataHandle);

		// XAMLからオブジェクト化
		XAMLResource::DataBase->Add(i, gcnew String(xaml.data()));
	}

}

//template <class T>
//FrameworkElement^ _GetFrom(){
//	return LoadContentsFromResource(IDR_INS_FLOAT);
//}
//template <>
//FrameworkElement^ _GetFrom<bool>(){
//	return LoadContentsFromResource(IDR_INS_CHECKBOX);
//}


template <class T>
void _CreateInspector(DockPanel^ dockPanel, String^ text, T* p, std::function<void(T)> collback){
	FrameworkElement ^com = LoadContentsFromResource(IDR_INS_FLOAT);
	dockPanel->Children->Add(com);
	DockPanel::SetDock(com, System::Windows::Controls::Dock::Top);
	auto tb = (TextBlock^)com->FindName("FloatName");
	if (tb)tb->Text = text;
	BindingInspector<T>((TextBox^)com->FindName("Value"), p, collback);

}




template <class T>
ref class Inspector : public InspectorData{
public:
	Inspector(TemplateInspectorDataSet<T>* data)
		:m_data(data){
	}
	~Inspector(){
		this->!Inspector();
	}
	!Inspector(){
		delete m_data;
	}

	void CreateInspector(DockPanel^ dockPanel) override{
		_CreateInspector<T>(dockPanel, gcnew String(m_data->Text.c_str()), m_data->data, m_data->collBack);
	}
private:
	TemplateInspectorDataSet<T> *m_data;
};