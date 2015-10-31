#pragma once


#include "Using.h"
#include "InspectorData.h"

//using namespace System::Windows::Input;
//using namespace System::Windows;
using System::Windows::Controls::MenuItem;
using System::Runtime::InteropServices::Marshal;

ref class GameScreen{
public:
	GameScreen(WindowsFormsHost ^wfh)
		:m_wfh(wfh)
	{
	}

	//void ml(Object^ sender, MouseButtonEventArgs ^e){
	//	*m_ML = (bool)e->LeftButton;
	//}
	//void mr(Object^ sender, MouseButtonEventArgs ^e){
	//	*m_MR = (bool)e->RightButton;
	//}
	void md(Object^ sender, System::Windows::Forms::MouseEventArgs ^e){
		if (e->Button == System::Windows::Forms::MouseButtons::Left){
			*m_ML = true;
		}
		if (e->Button == System::Windows::Forms::MouseButtons::Right){
			*m_MR = true;
		}
	}
	void mu(Object^ sender, System::Windows::Forms::MouseEventArgs ^e){
		if (e->Button == System::Windows::Forms::MouseButtons::Left){
			*m_ML = false;
		}
		if (e->Button == System::Windows::Forms::MouseButtons::Right){
			*m_MR = false;
		}
	}
	void mm(Object^ sender, System::Windows::Forms::MouseEventArgs ^e){
		//auto p = e->P(m_wfh);
		*m_MX = (int)e->X;
		*m_MY = (int)e->Y;
	}
	void SetMouseEvents(bool* l, bool* r, int* x, int* y){
		m_ML = l;
		m_MR = r;
		m_MX = x;
		m_MY = y;
		//m_wfh->Child->MouseMove	+= gcnew MouseButtonEventHandler(this, &GameScreen::ml);
		//m_wfh->Child->MouseLeftButtonUp += gcnew MouseButtonEventHandler(this, &GameScreen::ml);
		//m_wfh->Child->MouseRightButtonDown += gcnew MouseButtonEventHandler(this, &GameScreen::mr);
		//m_wfh->Child->MouseRightButtonUp += gcnew MouseButtonEventHandler(this, &GameScreen::mr);
		m_wfh->Child->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &GameScreen::md);
		m_wfh->Child->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &GameScreen::mu);
		m_wfh->Child->MouseMove += gcnew System::Windows::Forms::MouseEventHandler(this, &GameScreen::mm);
	}

	//OnSourceInitializedイベント以降じゃないと取得できない
	HWND GetHWND(){
		//HWNDを取得
		//auto source = gcnew System::Windows::Interop::WindowInteropHelper(this);
		//HwndSource ^source = (HwndSource^)HwndSource::FromVisual(this);
		System::IntPtr handle = m_wfh->Child->Handle;
		return reinterpret_cast<HWND>(handle.ToPointer());
	}

private:
	//ゲーム画面用のHWNDを取得するためのWindowsFormsHost
	WindowsFormsHost ^m_wfh;
	bool* m_ML;
	bool* m_MR;
	int* m_MX;
	int* m_MY;
	//bool* m_MIn;
};

// ビュー
ref class View : public Window {
public:

	View()
		: Window()
		, m_GameScreen(nullptr)
		, mGameScreenHWND(NULL)
		, m_ComponentPanel(nullptr)
		, m_TreeViewItemRoot(nullptr)
	{
		//DataContext = gcnew ViewModel();
		Background = Brushes::Black;
		//w:1184, h : 762
		Width = 1605 + 16 + 200;
		Height = 800 + 38;

		FrameworkElement ^contents = LoadContentsFromResource(IDR_VIEW);
		Content = contents;

		auto wfh = (WindowsFormsHost ^)contents->FindName("GameScreenWFH");
		m_GameScreen = gcnew GameScreen(wfh);

		m_ComponentPanel = (StackPanel ^)contents->FindName("MainDock");
		auto TreeViewDec = (Border ^)contents->FindName("TreeView");
		CreateTreeView(TreeViewDec);

		Title = "title";

		SetSelectItemReturnPoint();

		//((TestContent::Contents^)tv->DataContext)->TextChange("dacsdv");

		//System::Windows::Controls::TreeView a; 
		//auto i = a.Items;
		//i->CurrentItem

		//FrameworkElement ^component = LoadContentsFromResource(IDR_COMPONENT);
		//mainDock->Children->Add(component);

		//CreateComponent();

		//return;
		//
		//
		//Border ^baseBorder = (Border ^)contents->FindName("BaseBorder");
		//baseBorder->SetBinding(Border::BorderThicknessProperty, "Value");
		//baseBorder->SetBinding(Border::CornerRadiusProperty, "Value");
		////baseBorder->CornerRadius = System::Windows::CornerRadius(10,20,30,40);
		////baseBorder->SetValue(Border::CornerRadiusProperty, System::Windows::CornerRadius(10, 20, 30, 40));
		//
		//Slider ^valueSlider = (Slider ^)contents->FindName("ValueSlider");
		//valueSlider->SetBinding(Slider::ValueProperty, "Value");
		//
		//TextBlock ^valueTextBlock = (TextBlock ^)contents->FindName("ValueTextBlock");
		//valueTextBlock->SetBinding(TextBlock::TextProperty, "Value");
		//
		//
		//return;
		//
		//// Button 用の Style を追加
		//System::Windows::Style ^buttonStyle = gcnew System::Windows::Style(System::Windows::Controls::Button::typeid);
		//buttonStyle->Setters->Add(gcnew System::Windows::Setter(Button::ForegroundProperty, gcnew SolidColorBrush(Color::FromRgb(240, 240, 240))));
		//buttonStyle->Setters->Add(gcnew System::Windows::Setter(Button::BackgroundProperty, gcnew SolidColorBrush(Color::FromRgb(5, 147, 14 * 16 + 2))));
		//buttonStyle->Setters->Add(gcnew System::Windows::Setter(Button::HeightProperty, (double)28));
		//buttonStyle->Setters->Add(gcnew System::Windows::Setter(Button::MarginProperty, gcnew System::Windows::Thickness(1)));
		//Resources->Add(Button::typeid, buttonStyle);
		//
		//// 一番外側の DockPanel を追加
		//DockPanel ^dockPanel = gcnew DockPanel();
		//Content = dockPanel;
		//
		//// [OK][キャンセル] を置く StackPanel を作成
		//StackPanel ^stackPanel = gcnew StackPanel();
		//stackPanel->Orientation = System::Windows::Controls::Orientation::Horizontal;
		//stackPanel->FlowDirection = System::Windows::FlowDirection::RightToLeft;
		//// StackPanel にボタンを追加
		//auto cb = gcnew Button();
		//cb->Content = "キャンセル";
		//cb->Width = 90;
		//stackPanel->Children->Add(cb);
		//auto ok = gcnew Button();
		//ok->Content = "OK";
		//ok->Width = 90;
		//stackPanel->Children->Add(ok);
		//
		//// StackPanel を追加
		//dockPanel->Children->Add(stackPanel);
		//DockPanel::SetDock(stackPanel, System::Windows::Controls::Dock::Bottom);
		//
		//// XAMLで作ったものを追加
		//dockPanel->Children->Add(contents);
		//DockPanel::SetDock(contents, System::Windows::Controls::Dock::Top);


	}

	void UpdateView(){
		if (m_ComponentPanel == nullptr)return;
		for (int i = 0; i < m_ComponentPanel->Children->Count; i++){
			auto b = dynamic_cast<Border^>(m_ComponentPanel->Children[i]);
			if (b == nullptr)continue;
			auto d = dynamic_cast<Panel^>(b->Child);
			if (d == nullptr)continue;
			ForDockPanelChild(d);
		}
	}
	void ForDockPanelChild(Panel^ dock){
		for (int i = 0; i < dock->Children->Count; i++){

			Panel^ d = dynamic_cast<Panel^>(dock->Children[i]);
			if (d != nullptr){
				ForDockPanelChild(d);
				continue;
			}

			CheckObject(dock->Children[i]);
		}
	}
	void CheckObject(UIElement^ ui){
		FrameworkElement^ t = dynamic_cast<FrameworkElement^>(ui);
		if (t == nullptr)return;
		IViewModel^ f = dynamic_cast<IViewModel^>(t->DataContext);
		if (f == nullptr)return;
		f->UpdateView();
	}
	void ClearAllComponent(){
		if (m_ComponentPanel == nullptr)return;
		m_ComponentPanel->Children->Clear();
	}

	void CreateComponent(array<InspectorData^>^ data){
		if (m_ComponentPanel == nullptr)return;
		FrameworkElement ^com = LoadContentsFromResource(IDR_COMPONENT);
		m_ComponentPanel->Children->Add(com);
		//DockPanel::SetDock(com, System::Windows::Controls::Dock::Top);
		auto dock = (DockPanel^)com->FindName("MainDock");
		int num = data->GetLength(0);
		for (int i = 0; i < num; i++){
			data[i]->CreateInspector(dock);
		}
	}

	void AddItem(String ^Name, IntPtr ptr){
		if (m_TreeViewItemRoot == nullptr)return;
		auto item = gcnew TestContent::Person(Name, gcnew TestContent::MyList());
		item->DataPtr = ptr;
		m_TreeViewItemRoot->Add(item);
		Data::MyPostMessage(MyWindowMessage::StackIntPtr, (void*)item->DataPtr);
		Data::MyPostMessage(MyWindowMessage::ReturnTreeViewIntPtr, (void*)item->ThisIntPtr);
	}

	void SetParent(IntPtr parent, IntPtr child){
		auto p = (gcroot<TestContent::Person^>*)(void*)parent;
		auto c = (gcroot<TestContent::Person^>*)(void*)child;
		(*p)->Add(*c);
	}

	//更新イベント
	void ActorIntPtr_TargetUpdated(Object ^sender, System::Windows::Data::DataTransferEventArgs ^e){
		auto text = ((TextBlock^)sender);
		if (text->Text == ""){
			Data::MyPostMessage(MyWindowMessage::SelectActor, NULL);
			return;
		}
		//((IntViewModel^)text->DataContext)->Value = text->Text;
		//auto item = dynamic_cast<TestContent::Person^>(m_TreeView->SelectedItem);
		Data::MyPostMessage(MyWindowMessage::SelectActor, (void*)int::Parse(text->Text));

	}
	void SetSelectItemReturnPoint(){
		if (!m_ActorIntPtrDataBox)return;

		m_ActorIntPtrDataBox->Text = "0";

		auto bind = gcnew System::Windows::Data::Binding("SelectedValue.DataPtr");
		bind->Source = m_TreeView;
		//更新イベントを呼ぶ
		bind->NotifyOnTargetUpdated = true;
		m_ActorIntPtrDataBox->SetBinding(TextBlock::TextProperty, bind);
		//更新イベント
		m_ActorIntPtrDataBox->TargetUpdated += gcnew System::EventHandler<System::Windows::Data::DataTransferEventArgs^>(this, &View::ActorIntPtr_TargetUpdated);
		//アクターのIntPtrViewModelをバインド
		//m_ActorIntPtrDataBox->DataContext = vm;


	}
	void ChangeTreeViewName(String^ name, IntPtr treeviewptr){
		auto i = (gcroot<TestContent::Person^>*)(void*)treeviewptr;
		(*i)->Name = name;
	}

private:
	//ツリービュー作成
	void CreateTreeView(System::Windows::Controls::Decorator ^dec){
		//ツリービュー作成
		auto sp = (StackPanel^)LoadContentsFromResource(IDR_TREEVIEW);
		dec->Child = sp;
		auto treeView = (TreeView^)sp->FindName("treeView1");
		m_TreeView = treeView;

		//アイテムリスト作成
		auto list = gcnew TestContent::MyList();//gcnew List<TestContent::Person^>();
		//list->list->Add(gcnew TestContent::Person
		//	("U", gcnew TestContent::MyList(
		//			gcnew TestContent::Person("UNK", gcnew TestContent::MyList())
		//	))
		//);
		//アイテムリストのルートを作成
		auto root = gcnew TestContent::Person("root", list);
		treeView->DataContext = root;
		auto source = gcnew System::Windows::Data::Binding("Children");
		source->Mode = System::Windows::Data::BindingMode::TwoWay;
		treeView->SetBinding(TreeView::ItemsSourceProperty, source);
		m_TreeViewItemRoot = root;


		//アイテムリストのデータ構造
		auto datatemp = gcnew System::Windows::HierarchicalDataTemplate();
		//アイテムのリストバインド
		auto datasource = gcnew System::Windows::Data::Binding("Children");
		datasource->Mode = System::Windows::Data::BindingMode::TwoWay;
		datatemp->ItemsSource = datasource;

		//追加するアイテムのコントロール
		auto fact = gcnew System::Windows::FrameworkElementFactory();
		fact->Type = TextBlock::typeid;
		auto itembind = gcnew System::Windows::Data::Binding("Name");
		itembind->Mode = BindingMode::TwoWay;
		fact->SetBinding(TextBlock::TextProperty, itembind);
		datatemp->VisualTree = fact;

		treeView->ItemTemplate = datatemp;

		//選択中のアイテム表示
		auto tblock = (TextBlock ^)sp->FindName("textBlock1");
		m_ActorIntPtrDataBox = tblock;


		//コンテキストメニュー作成
		auto cm = gcnew System::Windows::Controls::ContextMenu();
		{//コンテキストメニューの要素作成
			auto mitem = gcnew System::Windows::Controls::MenuItem();
			mitem->Header = "しぬ";
			mitem->Click += gcnew System::Windows::RoutedEventHandler(this, &View::MenuItem_Click);
			cm->Items->Add(mitem);
		}
		{//コンテキストメニューの要素作成
			auto mitem = gcnew System::Windows::Controls::MenuItem();
			mitem->Header = "AddScriptComponent";
			mitem->Click += gcnew System::Windows::RoutedEventHandler(this, &View::MenuItem_Click_AddScriptComponent);
			cm->Items->Add(mitem);
		}
		{//コンテキストメニューの要素作成
			auto mitem = gcnew System::Windows::Controls::MenuItem();
			mitem->Header = "RemoveScriptComponent";
			mitem->Click += gcnew System::Windows::RoutedEventHandler(this, &View::MenuItem_Click_RemoveScriptComponent);
			cm->Items->Add(mitem);
		}
		{//コンテキストメニューの要素作成
			auto mitem = gcnew System::Windows::Controls::MenuItem();
			mitem->Header = "AddPhysXComponent";
			mitem->Click += gcnew System::Windows::RoutedEventHandler(this, &View::MenuItem_Click_AddPhysXComponent);
			cm->Items->Add(mitem);
		}
		{//コンテキストメニューの要素作成
			auto mitem = gcnew System::Windows::Controls::MenuItem();
			mitem->Header = "RemovePhysXComponent";
			mitem->Click += gcnew System::Windows::RoutedEventHandler(this, &View::MenuItem_Click_RemovePhysXComponent);
			cm->Items->Add(mitem);
		}
		{//コンテキストメニューの要素作成
			auto mitem = gcnew System::Windows::Controls::MenuItem();
			mitem->Header = "AddPhysXColliderComponent";
			mitem->Click += gcnew System::Windows::RoutedEventHandler(this, &View::MenuItem_Click_AddPhysXColliderComponent);
			cm->Items->Add(mitem);
		}
		{//コンテキストメニューの要素作成
			auto mitem = gcnew System::Windows::Controls::MenuItem();
			mitem->Header = "RemovePhysXColliderComponent";
			mitem->Click += gcnew System::Windows::RoutedEventHandler(this, &View::MenuItem_Click_RemovePhysXColliderComponent);
			cm->Items->Add(mitem);
		}
		//ツリービューに反映
		m_TreeView->ContextMenu = cm;


		//m_TreeView->AddHandler(TextBlock::MouseDownEvent, gcnew System::Windows::RoutedEventHandler(this, &View::OnDrag));

		//m_TreeView->Resources->Add(System::Windows::Controls::ContextMenu::typeid, cm);

		//auto treestyle = gcnew System::Windows::Style(TreeViewItem::typeid);
		//
		//auto cmbind = gcnew System::Windows::Data::Binding("TreeItemContextMenu");
		//treestyle->Setters->Add(gcnew System::Windows::Setter(TreeViewItem::ContextMenuProperty, cmbind));


		//fact->SetBinding(TextBlock::ContextMenuProperty, cmbind);

		//m_TreeView->ItemContainerStyle = treestyle;

	}

	void OnDrag(Object ^sender, System::Windows::RoutedEventArgs ^e){
		//auto item = (TestContent::Person^)e->Source;
		//System::Windows::MessageBox::Show("had");
		auto tv = (TreeView^)sender;
		//tv->Background = Brush
		System::Windows::MessageBox::Show(e->Source->ToString());
		e->Handled = true;
		//tv->Cursor = gcnew System::Windows::Input::Cursor("Hand");
		//System::Windows::MessageBox::Show("had");
	}
	void MenuItem_Click(Object ^sender, System::Windows::RoutedEventArgs ^e){
			{
				if (m_TreeView->SelectedItem == nullptr)return;
				auto i = (TestContent::Person^)m_TreeView->SelectedItem;
				//i->Name = "死んだ";
				//m_TreeViewItemRoot->Children->Remove(i);
				Data::MyPostMessage(MyWindowMessage::ActorDestroy, (void*)i->DataPtr);
				//((IntViewModel^)m_ActorIntPtrDataBox->DataContext)->Value = "0";
				//m_ActorIntPtrDataBox->Text = "0";
				//((TestContent::Person^)m_TreeView->SelectedItem)->DataPtr = (IntPtr)0;

				i->RemoveSelf();

				//要素があるときのみ
				if (m_TreeViewItemRoot->Children->Count != 0){
					//セレクトし直す
					ActorIntPtr_TargetUpdated(m_ActorIntPtrDataBox, nullptr);
				}

				e->Handled = true;
			}
	}

#pragma region MenuContext
	void PostMessageRemoveComponent(const char* t){
		if (m_TreeView->SelectedItem == nullptr)return;
		Data::MyPostMessage(MyWindowMessage::RemoveComponent, (void*)t);
	}
	void PostMessageAddComponent(const char* t){
		if (m_TreeView->SelectedItem == nullptr)return;
		Data::MyPostMessage(MyWindowMessage::AddComponent, (void*)t);
	}
	void MenuItem_Click_AddScriptComponent(Object ^sender, System::Windows::RoutedEventArgs ^e){
		PostMessageAddComponent("Script");
		e->Handled = true;
	}
	void MenuItem_Click_RemoveScriptComponent(Object ^sender, System::Windows::RoutedEventArgs ^e){
		PostMessageRemoveComponent("Script");
		e->Handled = true;
	}

	void MenuItem_Click_AddPhysXComponent(Object ^sender, System::Windows::RoutedEventArgs ^e){
		PostMessageAddComponent("PhysX");
		e->Handled = true;
	}
	void MenuItem_Click_RemovePhysXComponent(Object ^sender, System::Windows::RoutedEventArgs ^e){
		PostMessageRemoveComponent("PhysX");
		e->Handled = true;
	}

	void MenuItem_Click_AddPhysXColliderComponent(Object ^sender, System::Windows::RoutedEventArgs ^e){
		PostMessageAddComponent("Collider");
		e->Handled = true;
	}
	void MenuItem_Click_RemovePhysXColliderComponent(Object ^sender, System::Windows::RoutedEventArgs ^e){
		PostMessageRemoveComponent("Collider");
		e->Handled = true;
	}
//MenuContext
#pragma endregion 

protected:
	void OnSourceInitialized(System::EventArgs ^e) override
	{
		Window::OnSourceInitialized(e);

		mGameScreenHWND = m_GameScreen->GetHWND();

	}

public:
	property GameScreen^ GameScreenData{
		GameScreen^ get(){
			return m_GameScreen;
		}
	}
private:

	GameScreen ^m_GameScreen;
	HWND mGameScreenHWND;

	StackPanel ^m_ComponentPanel;

	TreeView ^ m_TreeView;
	TestContent::Person ^m_TreeViewItemRoot;
	TextBlock ^m_ActorIntPtrDataBox;

public:
	property HWND GameScreenHWND{
		HWND get(){
			return mGameScreenHWND;
		}
	}

};