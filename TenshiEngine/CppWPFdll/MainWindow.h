#pragma once


#include "Using.h"
#include "InspectorData.h"

#using <system.dll>


std::string* string_cast(String^ str){
	//String^ name = (String^)t->Header;
	pin_ptr<const wchar_t> wch = PtrToStringChars(str);
	size_t convertedChars = 0;
	size_t  sizeInBytes = ((str->Length + 1) * 2);
	char    *ch = (char *)malloc(sizeInBytes);
	wcstombs_s(&convertedChars,
		ch, sizeInBytes,
		wch, sizeInBytes);
	std::string* temp = new std::string(ch);
	free(ch);

	return temp;

}

//using namespace System::Windows::Input;
//using namespace System::Windows;
using System::Windows::Controls::MenuItem;
using System::Runtime::InteropServices::Marshal;
static int cre = 0;
ref class GameScreenPanel : public System::Windows::Forms::Panel{
public:
	property System::Windows::Forms::CreateParams^ CreateParams
	{
		virtual System::Windows::Forms::CreateParams^ get() override
		{
			System::Windows::Forms::CreateParams ^cp = __super::CreateParams;// System::Windows::Forms::Panel::CreateParams;
			//cp->ExStyle |= WS_EX_TRANSPARENT;// 0x00000020;//クリック透過
			//cp->Style == NULL;
			//cp->Caption = "aiuto";
			//cp->ExStyle == NULL;
			return cp;
		}
	}
	//virtual void OnPaintBackground(System::Windows::Forms::PaintEventArgs pevent) override
	//{
	//}
	//virtual void OnPaint(System::Windows::Forms::PaintEventArgs pevent) override
	//{
	//}
public:
	void UpdateStyle(){
		SetStyle(System::Windows::Forms::ControlStyles::SupportsTransparentBackColor, true);
		UpdateStyles();
	}
};

ref class GameScreen{
public:

	template<class T>
	T^ HandleDragEnter(System::Windows::Forms::DragEventArgs ^e)
	{
		System::Reflection::FieldInfo ^info;

		System::Object ^obj;

		info = e->Data->GetType()->GetField("innerData", System::Reflection::BindingFlags::NonPublic | System::Reflection::BindingFlags::Instance);

		obj = info->GetValue(e->Data);

		info = obj->GetType()->GetField("innerData", System::Reflection::BindingFlags::NonPublic | System::Reflection::BindingFlags::Instance);

		System::Windows::DataObject ^dataObj = (System::Windows::DataObject^)info->GetValue(obj);

		T ^item = (T^)dataObj->GetData(T::typeid);
		return item;

	}

	GameScreen(WindowsFormsHost ^wfh)
		:m_wfh(wfh)
	{
		auto panel = gcnew GameScreenPanel();
		m_wfh->Child = panel;
		auto par = panel->Parent;
		//auto par2 = par->Parent;
		//par->Visible = false;
		//par->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &GameScreen::mu);
		//panel->BackColor = System::Drawing::Color::Transparent;
		//panel->Image = nullptr;
		//panel->UpdateStyle();

		//panel->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &GameScreen::mu);
		//auto par3 = par2->Parent;
		//par->BackColor = System::Drawing::Color::AliceBlue;
		//par->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &GameScreen::mu);
		//panel->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &GameScreen::md);
		//panel->UpdateStyle();
		//panel->BackColor = System::Drawing::Color::Transparent;

		//wfh->AllowDrop = true;
		//wfh->DragEnter += gcnew DragEventHandler(this, &GameScreen::OnDragOver2);
		//wfh->DragOver += gcnew DragEventHandler(this, &GameScreen::OnDragOver2);
		//wfh->MouseDown += gcnew MouseButtonEventHandler(this, &GameScreen::md2);
		wfh->Child->AllowDrop = true;
		wfh->Child->DragDrop += gcnew System::Windows::Forms::DragEventHandler(this, &GameScreen::OnDrop);
		wfh->Child->DragEnter += gcnew System::Windows::Forms::DragEventHandler(this, &GameScreen::OnDragOver);
		wfh->Child->DragOver += gcnew System::Windows::Forms::DragEventHandler(this, &GameScreen::OnDragOver);

		//auto image = gcnew System::Windows::Controls::Image();
		//mD3DImageEx = gcnew System::Windows::Interop::D3DImageEx();
		//image->Source = mD3DImageEx;
		//Gamepanel->Children->Add(image);
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

		auto scr = (System::Windows::Forms::Panel^)m_wfh->Child;
		*m_WX = scr->Width;
		*m_WY = scr->Height;
	}
	//void keypressnull(Object^ sender, System::Windows::Forms::KeyEventArgs ^e){
	//	e->Handled = true;
	//}
	//void keynull(Object^ sender, System::Windows::Forms::KeyEventArgs ^e){
	//	e->Handled = true;
	//}
	void SetMouseEvents(bool* l, bool* r, int* x, int* y, int* wx, int* wy){
		m_ML = l;
		m_MR = r;
		m_MX = x;
		m_MY = y;
		m_WX = wx;
		m_WY = wy;


		//mD3DImageEx->SetBackBufferEx(System::Windows::Interop::D3DResourceTypeEx::ID3D11Texture2D, IntPtr(pRenderTarget));

		//m_wfh->Child->MouseMove	+= gcnew MouseButtonEventHandler(this, &GameScreen::ml);
		//m_wfh->Child->MouseLeftButtonUp += gcnew MouseButtonEventHandler(this, &GameScreen::ml);
		//m_wfh->Child->MouseRightButtonDown += gcnew MouseButtonEventHandler(this, &GameScreen::mr);
		//m_wfh->Child->MouseRightButtonUp += gcnew MouseButtonEventHandler(this, &GameScreen::mr);

		m_wfh->Child->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &GameScreen::md);
		m_wfh->Child->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &GameScreen::mu);
		m_wfh->Child->MouseMove += gcnew System::Windows::Forms::MouseEventHandler(this, &GameScreen::mm);

		//m_wfh->Child->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &GameScreen::keynull);
	}

	//OnSourceInitializedイベント以降じゃないと取得できない
	HWND GetHWND(){
		//HWNDを取得
		//auto source = gcnew System::Windows::Interop::WindowInteropHelper(this);
		//HwndSource ^source = (HwndSource^)HwndSource::FromVisual(this);
		//RemoveFrame();
		System::IntPtr handle = m_wfh->Child->Handle;

		return reinterpret_cast<HWND>(handle.ToPointer());
	}
	//ゲーム画面用のHWNDを取得するためのWindowsFormsHost

private:
	String^ GetFilePath(TestContent::Person^ item){
		if (item->Name == "__Assets__root__")return "Assets";
		return  GetFilePath(item->Parent) + "/" + item->Name;
	}
	void OnDrop(Object ^s, System::Windows::Forms::DragEventArgs ^e)
	{
		TreeViewItem ^t = HandleDragEnter<TreeViewItem>(e);

		auto name = GetFilePath((TestContent::Person^)t->DataContext);
		std::string* str = string_cast(name);
		Data::MyPostMessage(MyWindowMessage::CreatePrefabToActor, (void*)str);

	}

	void OnDragOver(Object ^sender, System::Windows::Forms::DragEventArgs ^e)
	{
		TreeViewItem ^t = HandleDragEnter<TreeViewItem>(e);
		if (((String^)t->Header)->Contains(".prefab")){
			e->Effect = System::Windows::Forms::DragDropEffects::Copy;
			return;
		}
		e->Effect = System::Windows::Forms::DragDropEffects::None;
	}

	WindowsFormsHost ^m_wfh;
	bool* m_ML;
	bool* m_MR;
	int* m_MX;
	int* m_MY;
	int* m_WX;
	int* m_WY;
};
// ビュー
ref class ColorPickerWindow : public Window {
public:
	static ColorPickerWindow ^mUnique = nullptr;

	ColorPickerWindow(Window^ owner, SolidColorBrush^ brush)
	{
		if (mUnique){
			mUnique->Close();
		}
		mUnique = this;

		mTargetBrush = brush;
		Owner = owner;
		Title = "ColorPicker";
		Width = 400;
		Height = 420;
		Show();
		Visibility = System::Windows::Visibility::Visible;




		FrameworkElement ^colwnd = LoadContentsFromResource(IDR_INSWND_COLOR);
		Content = colwnd;

#pragma push_macro("FindResource")
#undef FindResource
#pragma pop_macro("FindResource")
		auto colbrush = (VisualBrush ^)colwnd->TryFindResource("LevelSaturationBrush");

		auto vis = colbrush->Visual;
		auto can = (System::Windows::Controls::Canvas ^)colbrush->Visual;
		auto rect = (System::Windows::Shapes::Rectangle^) can->Children[0];
		auto lb = (LinearGradientBrush^)rect->Fill;
		auto gs = (GradientStop^)lb->GradientStops[1];

		mTargetColor = gs;

		//can->AddHandler(System::Windows::Controls::Canvas::MouseLeftButtonUpEvent, gcnew System::Windows::Input::MouseButtonEventHandler(this, &View::canvas_MouseUp),true);
		//rect->AddHandler(System::Windows::Controls::Canvas::MouseLeftButtonUpEvent, gcnew System::Windows::Input::MouseButtonEventHandler(this, &View::canvas_MouseUp),true);


		auto colpickPanel = (System::Windows::Shapes::Rectangle ^)colwnd->FindName("ColorPickerPanel");
		colpickPanel->AddHandler(System::Windows::Controls::Canvas::MouseMoveEvent, gcnew System::Windows::Input::MouseEventHandler(this, &ColorPickerWindow::canvas_MouseMove), true);
		colpickPanel->AddHandler(System::Windows::Controls::Canvas::MouseLeftButtonDownEvent, gcnew System::Windows::Input::MouseButtonEventHandler(this, &ColorPickerWindow::canvas_MouseDown), true);

		auto hpickPanel = (System::Windows::Shapes::Rectangle ^)colwnd->FindName("HPickerPanel");
		hpickPanel->AddHandler(System::Windows::Controls::Canvas::MouseMoveEvent, gcnew System::Windows::Input::MouseEventHandler(this, &ColorPickerWindow::color_MouseMove), true);
		hpickPanel->AddHandler(System::Windows::Controls::Canvas::MouseLeftButtonDownEvent, gcnew System::Windows::Input::MouseButtonEventHandler(this, &ColorPickerWindow::color_MouseDown), true);

		mViewColor = (System::Windows::Shapes::Rectangle ^)colwnd->FindName("ColorViewPanel");
		mCursor = (System::Windows::Controls::Canvas ^)colwnd->FindName("ColorPickerCursor");
		mHCursor = (System::Windows::Controls::Canvas ^)colwnd->FindName("HColorPickerCursor");


		double h, s, v;
		rgb2hsv(mTargetBrush->Color,&h,&s,&v);
		
		auto rgb = ToRgb(h * 2 * System::Math::PI / 360, 1, 1);

		mTargetColor->Color = rgb;
		h_CursorUpdate(h);
		
		double x = 0;
		if (v != 0.0){
			double pow = 1.0 / v;
			double cr = mTargetBrush->Color.R * pow;
			double cg = mTargetBrush->Color.G * pow;
			double cb = mTargetBrush->Color.B * pow;


			double sr = rgb.R;
			double sg = rgb.G;
			double sb = rgb.B;

			double hr = sr - cr;
			double hg = sg - cg;
			double hb = sb - cb;

			if (rgb.R == 0){
				x = hr;
			}else if(rgb.G == 0){
				x = hg;
			}else if (rgb.B == 0){
				x = hb;
			}

		}

		canvas_CursorUpdate(255+x, 255-v*255);
		color_Update();

	}
private:
	void rgb2hsv(Color in, double* ph, double* ps, double* pv)
	{
		double h, s, v;
		double      min, max, delta;
		double R = in.R / 255.0;
		double G = in.G / 255.0;
		double B = in.B / 255.0;

		min = R < G ? R : G;
		min = min  < B ? min : B;

		max = R > G ? R : G;
		max = max  > B ? max : B;

		v = max;                                // v
		delta = max - min;
		if (delta < 0.00001)
		{
			s = 0;
			h = 0; // undefined, maybe nan?

			*ph = h;
			*ps = s;
			*pv = v;

			return;
		}
		if (max > 0.0) { // NOTE: if Max is == 0, this divide would cause a crash
			s = (delta / max);                  // s
		}
		else {
			// if max is 0, then r = g = b = 0              
			// s = 0, v is undefined
			s = 0.0;
			h = NAN;                            // its now undefined

			*ph = h;
			*ps = s;
			*pv = v;
			return;
		}
		if (R >= max)                           // > is bogus, just keeps compilor happy
			h = (G - B) / delta;        // between yellow & magenta
		else
			if (G >= max)
				h = 2.0 + (B - R) / delta;  // between cyan & yellow
			else
				h = 4.0 + (R - G) / delta;  // between magenta & cyan

		h *= 60.0;                              // degrees

		if (h < 0.0)
			h += 360.0;


		*ph = h;
		*ps = s;
		*pv = v;

	}


	byte ToByte(double d)
	{
		if (d < 0)
		{
			return 0;
		}
		else if (d > 255)
		{
			return 255;
		}
		return (byte)System::Math::Round(d);
	}

	Color ToRgb(double H, double S, double V)
	{
		auto v = ToByte(255 * V);
		if (S == 0)
		{
			return Color::FromRgb(v, v, v);
		}
		auto hr = H / (2 * System::Math::PI);
		auto hd = (float)(6 * (hr - System::Math::Floor(hr)));
		auto h = (int)System::Math::Floor(hd);
		auto p = ToByte(v * (1 - S));
		auto q = ToByte(v * (1 - S * (hd - h)));
		auto t = ToByte(v * (1 - S * (1 - hd + h)));

		switch (h)
		{
		case 0:
			return Color::FromRgb(v, t, p);
		case 1:
			return Color::FromRgb(q, v, p);
		case 2:
			return Color::FromRgb(p, v, t);
		case 3:
			return Color::FromRgb(p, q, v);
		case 4:
			return Color::FromRgb(t, p, v);
		default:
			return Color::FromRgb(v, p, q);
		}
	}

	void h_CursorUpdate(double y){
		auto path = (System::Windows::Shapes::Path^)mHCursor->Children[0];
		auto trns = gcnew TranslateTransform();
		trns->Y = y - 2;

		path->RenderTransform = trns;
	}

	void color_MouseMove(Object ^sender, System::Windows::Input::MouseEventArgs ^e)
	{
		if (e->LeftButton == MouseButtonState::Released)return;

		// 対象Rectangleの取得
		auto r = (System::Windows::Shapes::Rectangle^)e->OriginalSource;
		// 色の表示

		// 位置計算
		auto cp = e->GetPosition(r);
		auto v = 0;
		auto h = (int)System::Math::Round(cp.Y);

		//if (0 <= h && h < 256 && 0 <= v && v < 256)
		{


			auto rgb = ToRgb(h * 2 * System::Math::PI / 360, 1, 1 - v / 255.0);

			mTargetColor->Color = rgb;


		}
		{
			h_CursorUpdate(cp.Y);
		}

		color_Update();
	}

	void color_MouseDown(Object ^sender, System::Windows::Input::MouseButtonEventArgs ^e)
	{
		// 対象Rectangleの取得
		auto r = (System::Windows::Shapes::Rectangle^)e->OriginalSource;
		// 色の表示

		// 位置計算
		auto cp = e->GetPosition(r);
		auto v = 1;
		auto h = (int)System::Math::Round(cp.Y);

		//if (0 <= h && h < 256 && 0 <= v && v < 256)
		{


			auto rgb = ToRgb(h * 2 * System::Math::PI / 360, 1, 1 - v / 255.0);

			mTargetColor->Color = rgb;


		}
		{
			h_CursorUpdate(cp.Y);
		}

		color_Update();
	}

	void color_Update(){
		// 色の表示
		auto h = (int)System::Math::Round(mCursorPoint.X);
		auto v = (int)System::Math::Round(mCursorPoint.Y);

		//if (0 <= h && h < 256 && 0 <= v && v < 256)
		{

			auto col = mTargetColor->Color;
			auto r = 255 - col.R;
			auto g = 255 - col.G;
			auto b = 255 - col.B;
			float fh = (255 - h) / 255.0f;
			float fv = (255 - v) / 255.0f;
			col.R += (unsigned char)r*fh;
			col.G += (unsigned char)g*fh;
			col.B += (unsigned char)b*fh;
			col.R = (unsigned char)(col.R * fv);
			col.G = (unsigned char)(col.G * fv);
			col.B = (unsigned char)(col.B * fv);


			mViewColor->Fill = gcnew SolidColorBrush(col);
			mTargetBrush->Color = col;

		}
	}

	void canvas_CursorUpdate(double x,double y){

		mCursorPoint.X = x;
		mCursorPoint.Y = y;

		auto path = (System::Windows::Shapes::Path^)mCursor->Children[0];
		auto trns = gcnew TranslateTransform();
		trns->X = mCursorPoint.X + 10 - 18 + 1;
		trns->Y = mCursorPoint.Y + 10 - 18 + 1;


		path->RenderTransform = trns;
	}

	void canvas_MouseMove(Object ^sender, System::Windows::Input::MouseEventArgs ^e)
	{
		if (e->LeftButton == MouseButtonState::Released)return;


		// 対象Rectangleの取得
		auto r = (System::Windows::Shapes::Rectangle^)e->OriginalSource;

		auto pos = e->GetPosition(r);

		canvas_CursorUpdate(pos.X, pos.Y);
		color_Update();
	}

	void canvas_MouseDown(Object ^sender, System::Windows::Input::MouseButtonEventArgs ^e)
	{
		if (e->LeftButton == MouseButtonState::Released)return;


		// 対象Rectangleの取得
		auto r = (System::Windows::Shapes::Rectangle^)e->OriginalSource;

		// 位置計算
		auto pos = e->GetPosition(r);

		canvas_CursorUpdate(pos.X, pos.Y);
		color_Update();
	}

	SolidColorBrush^ mTargetBrush;
	GradientStop^ mTargetColor;
	System::Windows::Point mCursorPoint;
	System::Windows::Shapes::Rectangle^ mViewColor;
	System::Windows::Controls::Canvas^ mCursor;
	System::Windows::Controls::Canvas^ mHCursor;
	
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
		//Width = 1605 + 16 + 200;
		//Height = 800 + 38;
		this->WindowState = System::Windows::WindowState::Maximized;

		FrameworkElement ^contents = LoadContentsFromResource(IDR_VIEW);
		Content = contents;

		auto wfh = (WindowsFormsHost ^)contents->FindName("GameScreenWFH");
		m_GameScreen = gcnew GameScreen(wfh);

		m_ComponentPanel = (StackPanel ^)contents->FindName("MainDock");
		auto cm = gcnew System::Windows::Controls::ContextMenu();
		m_ComponentPanel->ContextMenu = cm;

		auto TreeViewDec = (Border ^)contents->FindName("TreeView");
		CreateTreeView(TreeViewDec);
		auto AssetTreeViewDec = (Border ^)contents->FindName("AssetTreeView");
		CreateAssetTreeView(AssetTreeViewDec);

		auto commandBarPanel = (StackPanel ^)contents->FindName("CommandBarPanel");
		auto commandBar = LoadContentsFromResource(IDR_COMMAND_BAR);
		commandBarPanel->Children->Add(commandBar);


		//メニューバー作成
		{
			auto pbtn = (Button^)commandBar->FindName("PlayGameButton");
			pbtn->Click += gcnew System::Windows::RoutedEventHandler(this, &View::PlayButton_Click);
			auto sbtn = (Button^)commandBar->FindName("StopGameButton");
			sbtn->Click += gcnew System::Windows::RoutedEventHandler(this, &View::StopButton_Click);
			auto cbtn = (Button^)commandBar->FindName("ScriptCompile");
			cbtn->Click += gcnew System::Windows::RoutedEventHandler(this, &View::CompileButton_Click);
			auto savebtn = (Button^)commandBar->FindName("SaveButton");
			savebtn->Click += gcnew System::Windows::RoutedEventHandler(this, &View::SaveButton_Click);
		}

		Title = "TenshiEngine";
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


#pragma region コンポーネントウィンドウ

public:
	//コンポーネントの値更新
	void UpdateView(){
		if (m_ComponentPanel == nullptr)return;
		for (int i = 0; i < m_ComponentPanel->Children->Count; i++){
			auto b = dynamic_cast<Border^>(m_ComponentPanel->Children[i]);
			if (b == nullptr)continue;
			auto e = dynamic_cast<Expander^>(b->Child);
			if (e == nullptr)continue;
			auto d = dynamic_cast<Panel^>(e->Content);
			if (d == nullptr)continue;
			ForDockPanelChild(d);
		}
	}

	//表示しているコンポーネントを全て非表示（削除）にする
	void ClearAllComponent(){
		if (m_ComponentPanel == nullptr)return;
		m_ComponentPanel->Children->Clear();
	}

	//コンポーネントの作成
	void CreateComponent(String^ headerName, IntPtr comptr, array<InspectorData^>^ data){
		if (m_ComponentPanel == nullptr)return;

		FrameworkElement ^com = LoadContentsFromResource(IDR_COMPONENT);
		m_ComponentPanel->Children->Add(com);

		auto header = (Expander^)com->FindName("ComponentHeaderName");
		header->Header = headerName;
		auto dock = (DockPanel^)com->FindName("MainDock");
		int num = data->GetLength(0);
		for (int i = 0; i < num; i++){
			data[i]->CreateInspector(dock);
		}

		auto cm = gcnew System::Windows::Controls::ContextMenu();
		{//コンテキストメニューの要素作成
			auto mitem = gcnew System::Windows::Controls::MenuItem();
			mitem->Header = "Remove";
			mitem->Tag = comptr;
			mitem->Click += gcnew System::Windows::RoutedEventHandler(this, &View::MenuItem_Click_RemoveComponent);
			cm->Items->Add(mitem);
		}
		com->ContextMenu = cm;
	}

	//ドラッグ中の判定
	void Commponent_ViewModel_OnDragOver(Object ^sender, DragEventArgs ^e)
	{
		// ドラッグされたいるデータがTreeVireItemか？
		if (e->Data->GetDataPresent(TreeViewItem::typeid))
		{
			e->Effects = DragDropEffects::Copy;
			return;
		}

		e->Effects = DragDropEffects::None;
	}

	//コンポーネントのViewMdeolにドロップ処理
	void Commponent_ViewModel_OnDrop(Object ^s, DragEventArgs ^e)
	{

		auto dragtreeitem = (TreeViewItem^)e->Data->GetData(TreeViewItem::typeid);
		auto dragitem = dynamic_cast<TestContent::Person^>(dragtreeitem->DataContext);
		//アクターをD&Dしているか
		if (dragitem->DataPtr != (IntPtr)NULL){
		}
		else{
			Commponent_ViewModel_AssetDrop(s, dragitem);
		}
		e->Handled = true;
	}

	void CreateColorPickerWindow(Object ^sender, System::Windows::Input::MouseButtonEventArgs ^e){
		auto r = (System::Windows::Shapes::Rectangle^)sender;
		auto scb = (SolidColorBrush^)r->Fill;

		Window^ w = gcnew ColorPickerWindow(this,scb);
	}

private:
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

	//コンテキストメニューのコンポーネント削除処理
	void MenuItem_Click_RemoveComponent(Object ^sender, System::Windows::RoutedEventArgs ^e){
		auto m = (MenuItem^)sender;
		auto ptr = (IntPtr)m->Tag;
		Data::MyPostMessage(MyWindowMessage::RemoveComponent, (void*)ptr);
		e->Handled = true;
	}

	//ゲームオブジェクト以外をドロップした処理
	void Commponent_ViewModel_AssetDrop(Object ^s, TestContent::Person^ dragitem){

		auto name = GetFilePath(dragitem);

		auto targettextbox = dynamic_cast<TextBox^>(s);
		//TextBoxに対してDropしたか
		if (targettextbox){
			auto viewmodel = (ViewModel<std::string>^)targettextbox->DataContext;
			viewmodel->Value = name;
		}
	}


//コンポーネントウィンドウ
#pragma endregion 

#pragma region シーンウィンドウ

public:

	//ツリービューにゲームオブジェクトを追加
	void AddItem(String ^Name, IntPtr ptr){
		if (m_TreeViewItemRoot == nullptr)return;
		auto item = gcnew TestContent::Person(Name, gcnew TestContent::MyList());
		item->DataPtr = ptr;
		m_TreeViewItemRoot->Add(item);
		Data::MyPostMessage(MyWindowMessage::StackIntPtr, (void*)item->DataPtr);
		Data::MyPostMessage(MyWindowMessage::ReturnTreeViewIntPtr, (void*)item->ThisIntPtr);
	}

	//ツリービューで親子関係の登録
	void SetParent(IntPtr parent, IntPtr child){
		auto p = (gcroot<TestContent::Person^>*)(void*)parent;
		auto c = (gcroot<TestContent::Person^>*)(void*)child;
		(*p)->Add(*c);
	}

	//ツリービューアイテムの名前変更
	void ChangeTreeViewName(String^ name, IntPtr treeviewptr){
		auto i = (gcroot<TestContent::Person^>*)(void*)treeviewptr;
		(*i)->Name = name;
	}
	//ツリービューアイテムの削除
	void ClearTreeViewItem(IntPtr treeviewptr){
		auto i = (gcroot<TestContent::Person^>*)(void*)treeviewptr;
		(*i)->RemoveSelf();
	}

	//コンテキストメニューの要素作成
	void CreateContextMenu_AddComponent(String^ componentname){
		CreateContextMenu_AddComponent(componentname, m_TreeView->ContextMenu);
		CreateContextMenu_AddComponent(componentname, m_ComponentPanel->ContextMenu);
	}

private:

	//シーンツリービュー作成
	void CreateTreeView(System::Windows::Controls::Decorator ^dec){
		//ツリービュー作成
			{
				auto sp = (Panel^)LoadContentsFromResource(IDR_TREEVIEW);

				dec->Child = sp;
				auto treeView = (TreeView^)sp->FindName("treeView1");
				m_TreeView = treeView;

				//選択中のアイテム表示
				auto tblock = (TextBlock ^)sp->FindName("textBlock1");
				m_ActorIntPtrDataBox = tblock;
			}

		//アイテムリスト作成
		{
			auto list = gcnew TestContent::MyList();
			//アイテムリストのルートを作成
			auto root = gcnew TestContent::Person("root", list);
			m_TreeView->DataContext = root;
			auto source = gcnew System::Windows::Data::Binding("Children");
			source->Mode = System::Windows::Data::BindingMode::TwoWay;
			m_TreeView->SetBinding(TreeView::ItemsSourceProperty, source);
			m_TreeViewItemRoot = root;
		}

		//アイテムリストのデータ構造
		{
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
			fact->SetValue(TextBlock::ForegroundProperty, gcnew SolidColorBrush(Color::FromRgb(24, 24, 24)));

			//fact->AddHandler(TreeViewItem::LostFocusEvent, gcnew System::Windows::RoutedEventHandler(this, &View::TreeViewItem_ForcusLost));
			//fact->AddHandler(TreeViewItem::MouseLeftButtonDownEvent, gcnew MouseButtonEventHandler(this, &View::OnMouseDown), true);
			//fact->AddHandler(TreeViewItem::DropEvent, gcnew DragEventHandler(this, &View::ActorTreeView_OnDrop));
			//fact->AddHandler(TreeViewItem::MouseRightButtonDownEvent, gcnew MouseButtonEventHandler(this, &View::OnMouseDown));
			//fact->AddHandler(TreeViewItem::MouseLeaveEvent, gcnew System::Windows::Input::MouseEventHandler(this, &View::TreeViewItem_OnMouseLeave));
			//fact->AddHandler(TreeViewItem::DragEnterEvent, gcnew DragEventHandler(this, &View::ActorTreeView_OnDragOver));
			//fact->AddHandler(TreeViewItem::DragOverEvent, gcnew DragEventHandler(this, &View::ActorTreeView_OnDragOver));
			datatemp->VisualTree = fact;

			m_TreeView->ItemTemplate = datatemp;
		}

		//styleのセット
		{
			//今のところ上で設定しているので意味なし?
			auto s = gcnew System::Windows::Style(TreeViewItem::typeid);
			auto setter = s->Setters;
			//setter->Add(gcnew System::Windows::Setter(TreeViewItem::ForegroundProperty, gcnew SolidColorBrush(Color::FromRgb(240, 240, 240))));
			//setter->Add(gcnew System::Windows::Setter(TreeViewItem::BackgroundProperty, gcnew SolidColorBrush(Color::FromRgb(5, 147, 14 * 16 + 2))));
			setter->Add(gcnew System::Windows::EventSetter(TreeViewItem::DropEvent, gcnew DragEventHandler(this, &View::ActorTreeView_OnDrop)));
			setter->Add(gcnew System::Windows::EventSetter(TreeViewItem::MouseLeaveEvent, gcnew System::Windows::Input::MouseEventHandler(this, &View::TreeViewItem_OnMouseLeave)));
			setter->Add(gcnew System::Windows::EventSetter(TreeViewItem::DragEnterEvent, gcnew DragEventHandler(this, &View::ActorTreeView_OnDragOver)));
			setter->Add(gcnew System::Windows::EventSetter(TreeViewItem::DragOverEvent, gcnew DragEventHandler(this, &View::ActorTreeView_OnDragOver)));
			m_TreeView->ItemContainerStyle = s;


			m_TreeView->AllowDrop = true;
			m_TreeView->Drop += gcnew DragEventHandler(this, &View::ActorTreeView_OnDrop);
			m_TreeView->DragEnter += gcnew DragEventHandler(this, &View::ActorTreeView_OnDragOver);
			m_TreeView->DragOver += gcnew DragEventHandler(this, &View::ActorTreeView_OnDragOver);
		}


		//コンテキストメニュー作成
		{
			auto cm = gcnew System::Windows::Controls::ContextMenu();
			{//コンテキストメニューの要素作成
				auto mitem = gcnew System::Windows::Controls::MenuItem();
				mitem->Header = "CreateObject";
				mitem->Click += gcnew System::Windows::RoutedEventHandler(this, &View::MenuItem_Click_CreateObject);
				cm->Items->Add(mitem);
			}
			{//コンテキストメニューの要素作成
				auto mitem = gcnew System::Windows::Controls::MenuItem();
				mitem->Header = "Remove";
				mitem->Click += gcnew System::Windows::RoutedEventHandler(this, &View::MenuItem_Click_Remove);
				cm->Items->Add(mitem);
			}
			//ツリービューに反映
			m_TreeView->ContextMenu = cm;
		}

	}

	//シーンビューでゲームオブジェクトを選択した時の処理
	void ActorIntPtr_TargetUpdated(Object ^sender, System::Windows::Data::DataTransferEventArgs ^e){
		auto text = ((TextBlock^)sender);
		if (text->Text == ""){
			Data::MyPostMessage(MyWindowMessage::SelectActor, NULL);
			return;
		}
		//System::Windows::MessageBox::Show("text:" + text->Text + "a:" + ((TestContent::Person^)m_TreeView->SelectedValue)->Name);
		//if (text->Text != ((TestContent::Person^)m_TreeView->SelectedValue)->DataPtr.ToString()){
		//	text->Text = "0";
		//}
		//((IntViewModel^)text->DataContext)->Value = text->Text;
		//auto item = dynamic_cast<TestContent::Person^>(m_TreeView->SelectedItem);
		Data::MyPostMessage(MyWindowMessage::SelectActor, (void*)int::Parse(text->Text));

	}

	//選択しているオブジェクトをテキストで表示処理をバインド
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

	//コンテキストメニューのコンポーネント追加の追加
	void CreateContextMenu_AddComponent(String^ text, System::Windows::Controls::ContextMenu^ cm){
		auto mitem = gcnew System::Windows::Controls::MenuItem();
		mitem->Header = text;
		mitem->Click += gcnew System::Windows::RoutedEventHandler(this, &View::MenuItem_Click_AddComponent);
		cm->Items->Add(mitem);
	}
	//コンテキストメニューのコンポーネント追加処理
	void MenuItem_Click_AddComponent(Object ^sender, System::Windows::RoutedEventArgs ^e){
		auto m = (MenuItem^)sender;
		auto s = (String^)m->Header;
		auto str = string_cast(s);
		PostMessageAddComponent(str);
		e->Handled = true;
	}
	//コンテキストメニューのコンポーネント追加のコールバックを送信
	void PostMessageAddComponent(std::string* str){
		if (m_TreeView->SelectedItem == nullptr)return;
		Data::MyPostMessage(MyWindowMessage::AddComponent, (void*)str);
	}

	//コンテキストメニューのオブジェクト作成
	void MenuItem_Click_CreateObject(Object ^sender, System::Windows::RoutedEventArgs ^e){
		Data::MyPostMessage(MyWindowMessage::CreatePrefabToActor, new std::string("EngineResource/box.prefab"));
		e->Handled = true;
	}
	//コンテキストメニューのゲームオブジェクトの削除処理
	void MenuItem_Click_Remove(Object ^sender, System::Windows::RoutedEventArgs ^e){

		//カーソルを変えないとダメ
		//要素があるときのみ
		//if (m_TreeViewItemRoot->Children->Count != 0){
		//セレクトし直す
		//m_ActorIntPtrDataBox
		ActorIntPtr_TargetUpdated(gcnew TextBlock, nullptr);
		//}

		if (m_TreeView->SelectedItem == nullptr)return;
		auto i = (TestContent::Person^)m_TreeView->SelectedItem;
		Data::MyPostMessage(MyWindowMessage::ActorDestroy, (void*)i->DataPtr);

		//デストロイ時にアクターから消される
		//i->RemoveSelf();

		e->Handled = true;
	}



	//ドラッグ中の判定
	void ActorTreeView_OnDragOver(Object ^sender, DragEventArgs ^e)
	{
		if (e->Data->GetDataPresent(TreeViewItem::typeid))
		{
			e->Effects = DragDropEffects::Copy;
			return;
		}

		e->Effects = DragDropEffects::None;
	}

	//シーンツリービューでドロップ処理
	void ActorTreeView_OnDrop(Object ^s, DragEventArgs ^e)
	{

		auto dragtreeitem = (TreeViewItem^)e->Data->GetData(TreeViewItem::typeid);
		auto dragitem = dynamic_cast<TestContent::Person^>(dragtreeitem->DataContext);

		//アクターをD&Dしているか
		if (dragitem->DataPtr != (IntPtr)NULL){

			ActorDrop(s, dragitem);
		}
		else{
			OtherDrop(s, dragitem);
		}
		e->Handled = true;
	}

	//ゲームオブジェクトをドロップした処理
	void ActorDrop(Object ^s, TestContent::Person^ dragitem){
		auto targettreeitem = dynamic_cast<TreeViewItem^>(s);
		//ツリービューアイテムに対してDropしたか
		if (targettreeitem){
			auto targetitem = dynamic_cast<TestContent::Person^>(targettreeitem->DataContext);
			//アクターに対してDropしたか
			if (targetitem->DataPtr != (IntPtr)NULL){
				//ドロップ先が自分の子ならTRUE
				if (ParentCheck(dragitem, targetitem))return;

				targetitem->Add(dragitem);
				Data::MyPostMessage(MyWindowMessage::StackIntPtr, (void*)targetitem->DataPtr);
				Data::MyPostMessage(MyWindowMessage::SetActorParent, (void*)dragitem->DataPtr);
			}
		}
		//ツリービューアイテム外に対してDropしたなら
		else{
			m_TreeViewItemRoot->Add(dragitem);
			Data::MyPostMessage(MyWindowMessage::StackIntPtr, (void*)NULL);
			Data::MyPostMessage(MyWindowMessage::SetActorParent, (void*)dragitem->DataPtr);
		}
	}
	//ゲームオブジェクト以外をドロップした処理
	void OtherDrop(Object ^s, TestContent::Person^ dragitem){

		auto name = GetFilePath(dragitem);
		if (!name->Contains(".json"))return;
		std::string* str = string_cast(name);
		Data::MyPostMessage(MyWindowMessage::CreatePrefabToActor, (void*)str);
	}

	//ターゲットがペアレントの親ならTRUE
	bool ParentCheck(TestContent::Person^ target, TestContent::Person^ parent){
		if (parent == m_TreeViewItemRoot){
			return false;
		}
		if (target == parent){
			return true;
		}

		return ParentCheck(target, parent->Parent);

	}


//シーンウィンドウ
#pragma endregion

#pragma region アセットツリービュー

public:

private:

	TestContent::Person ^ m_AssetTreeView_RootItem;
	TreeView^ m_AssetTreeView;

	//アセットツリービュー作成
	void CreateAssetTreeView(System::Windows::Controls::Decorator ^dec){
		//ツリービュー作成

		auto sp = (Panel^)LoadContentsFromResource(IDR_TREEVIEW);
		dec->Child = sp;
		auto treeView = (TreeView^)sp->FindName("treeView1");
		m_AssetTreeView = treeView;

		//アイテムリスト作成
		auto list = gcnew TestContent::MyList();
		//アイテムリストのルートを作成
		m_AssetTreeView_RootItem = gcnew TestContent::Person("__Assets__root__", list);
		treeView->DataContext = m_AssetTreeView_RootItem;
		auto source = gcnew System::Windows::Data::Binding("Children");
		source->Mode = System::Windows::Data::BindingMode::TwoWay;
		treeView->SetBinding(TreeView::ItemsSourceProperty, source);


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
		//fact->SetBinding(TreeViewItem::HeaderProperty, itembind);
		//fact->AddHandler(TreeViewItem::LostFocusEvent, gcnew System::Windows::RoutedEventHandler(this, &View::TreeViewItem_ForcusLost));
		//fact->AddHandler(TreeViewItem::MouseLeftButtonDownEvent, gcnew MouseButtonEventHandler(this, &View::OnMouseDown), true);
		//fact->AddHandler(TreeViewItem::MouseRightButtonDownEvent, gcnew MouseButtonEventHandler(this, &View::OnMouseDown));
		//fact->AddHandler(TreeViewItem::MouseLeaveEvent, gcnew System::Windows::Input::MouseEventHandler(this, &View::TreeViewItem_OnMouseLeave));
		//fact->AddHandler(TreeViewItem::ExpandedEvent, gcnew System::Windows::RoutedEventHandler(this, &View::TreeView_Expanded));
		datatemp->VisualTree = fact;

		treeView->ItemTemplate = datatemp;

		//styleのセット
		//今のところ上で設定しているので意味なし?
		auto s = gcnew System::Windows::Style(TreeViewItem::typeid);
		auto setter = s->Setters;
		setter->Add(gcnew System::Windows::EventSetter(TreeViewItem::ExpandedEvent, gcnew System::Windows::RoutedEventHandler(this, &View::TreeView_Expanded)));
		//setter->Add(gcnew System::Windows::EventSetter(TreeViewItem::DropEvent, gcnew DragEventHandler(this, &View::ActorTreeView_OnDrop)));
		setter->Add(gcnew System::Windows::EventSetter(TreeViewItem::MouseLeaveEvent, gcnew System::Windows::Input::MouseEventHandler(this, &View::TreeViewItem_OnMouseLeave)));
		setter->Add(gcnew System::Windows::EventSetter(TreeViewItem::MouseDoubleClickEvent, gcnew System::Windows::Input::MouseButtonEventHandler(this, &View::AssetsTreeViewItem_OnMouseDoubleClick)));
		//setter->Add(gcnew System::Windows::EventSetter(TreeViewItem::DragEnterEvent, gcnew DragEventHandler(this, &View::ActorTreeView_OnDragOver)));
		//setter->Add(gcnew System::Windows::EventSetter(TreeViewItem::DragOverEvent, gcnew DragEventHandler(this, &View::ActorTreeView_OnDragOver)));

		treeView->ItemContainerStyle = s;
		

		treeView->AllowDrop = true;
		treeView->Drop += gcnew DragEventHandler(this, &View::OnDrop);
		treeView->DragEnter += gcnew DragEventHandler(this, &View::OnDragOver2);
		treeView->DragOver += gcnew DragEventHandler(this, &View::OnDragOver2);

		CreateAssetTreeViewItem(m_AssetTreeView_RootItem, "Assets");

		FileSystemEvent();

		////コンテキストメニュー作成
		//{
		//	auto cm = gcnew System::Windows::Controls::ContextMenu();
		//	{//コンテキストメニューの要素作成
		//		auto mitem = gcnew System::Windows::Controls::MenuItem();
		//		mitem->Header = "Remove";
		//		mitem->Click += gcnew System::Windows::RoutedEventHandler(this, &View::test);
		//		cm->Items->Add(mitem);
		//	}
		//	//ツリービューに反映
		//	m_AssetTreeView->ContextMenu = cm;
		//}
	}

	delegate void fsChange(Object^ source, System::IO::FileSystemEventArgs^ e);

	void OnFSEvent(Object^ source, System::IO::FileSystemEventArgs^ e){

		fsChange ^del;
		if (e->ChangeType == System::IO::WatcherChangeTypes::Changed){
			del = gcnew fsChange(this, &View::OnChanged);
		}
		else if (e->ChangeType == System::IO::WatcherChangeTypes::Created){
			del = gcnew fsChange(this, &View::OnCreated);
		}
		else if (e->ChangeType == System::IO::WatcherChangeTypes::Deleted){
			del = gcnew fsChange(this, &View::OnDeleted);

		}

		Dispatcher->Invoke(del, source, e);
	}

	void OnChanged(Object^ source, System::IO::FileSystemEventArgs^ e)
	{
	}
	void OnCreated(Object^ source, System::IO::FileSystemEventArgs^ e)
	{
		auto item = gcnew TestContent::Person(e->Name, gcnew TestContent::MyList());
		item->DataPtr = (IntPtr)NULL;
		m_AssetTreeView_RootItem->Add(item);
	}
	void OnDeleted(Object^ source, System::IO::FileSystemEventArgs^ e)
	{
		int num = m_AssetTreeView_RootItem->Children->Count;
		for (int i = 0; i<num; i++){
			auto item = m_AssetTreeView_RootItem->Children[i];
			if (item->Name == e->Name){
				item->RemoveSelf();
			}
		}
	}
	void OnRenamed(Object^ source, System::IO::RenamedEventArgs^ e)
	{
		int num = m_AssetTreeView_RootItem->Children->Count;
		for (int i = 0; i<num; i++){
			auto item = m_AssetTreeView_RootItem->Children[i];
			if (item->Name == e->OldName){
				item->Name = e->Name;
			}
		}
	}

	void FileSystemEvent()
	{
		
		auto fsWatcher = gcnew System::IO::FileSystemWatcher();
		fsWatcher->Path = "Assets";
		fsWatcher->NotifyFilter = static_cast<System::IO::NotifyFilters>
			(System::IO::NotifyFilters::FileName |
			System::IO::NotifyFilters::Attributes |
			System::IO::NotifyFilters::LastAccess |
			System::IO::NotifyFilters::LastWrite |
			System::IO::NotifyFilters::Security |
			System::IO::NotifyFilters::Size);
		
		fsWatcher->Changed += gcnew System::IO::FileSystemEventHandler(
			this, &View::OnFSEvent);
		fsWatcher->Created += gcnew System::IO::FileSystemEventHandler(
			this, &View::OnFSEvent);
		fsWatcher->Deleted += gcnew System::IO::FileSystemEventHandler(
			this, &View::OnFSEvent);
		fsWatcher->Renamed += gcnew System::IO::RenamedEventHandler(
			this, &View::OnRenamed);

		fsWatcher->EnableRaisingEvents = true;

	}




	//指定フォルダから全てのフォルダとファイルを登録
	void CreateAssetTreeViewItem(TestContent::Person^ parent, String^ Path){
		auto dirs = gcnew System::IO::DirectoryInfo(Path);

		auto folders = dirs->EnumerateDirectories()->GetEnumerator();
		while (folders->MoveNext()){
			auto item = gcnew TestContent::Person(folders->Current->Name, gcnew TestContent::MyList());
			item->DataPtr = (IntPtr)NULL;
			parent->Add(item);

			CreateAssetTreeViewItem(item, folders->Current->FullName);

			//空のフォルダの場合　空のアイテムを追加
			if (item->Children->Count == 0){
				auto nullitem = gcnew TestContent::Person("", gcnew TestContent::MyList());
				nullitem->DataPtr = (IntPtr)NULL;
				parent->Add(nullitem);
			}
		}
		auto files = dirs->EnumerateFiles()->GetEnumerator();
		while (files->MoveNext()){
			auto item = gcnew TestContent::Person(files->Current->Name, gcnew TestContent::MyList());
			item->DataPtr = (IntPtr)NULL;
			parent->Add(item);
		}

	}
	//フォルダのルートからの相対パスを取得
	String^ GetFolderPath(TestContent::Person^ item){
		if (item->Name == "__Assets__root__")return "Assets/";
		return GetFolderPath(item->Parent) + item->Name + "/";
	}
	//ファイルのルートからの相対パスを取得
	String^ GetFilePath(TestContent::Person^ item){
		if (item->Name == "__Assets__root__")return "Assets";
		return GetFilePath(item->Parent) + "/" + item->Name;
	}

	//ツリービューでフォルダを展開する処理
	void TreeView_Expanded(Object ^s, System::Windows::RoutedEventArgs ^e)
	{

		auto treeitem = (TreeViewItem^)s;
		auto par = (TestContent::Person^)treeitem->DataContext;

		par->Children->Clear();

		auto dirPath = GetFolderPath(par);

		CreateAssetTreeViewItem(par, dirPath);

		e->Handled = true;
	}

	//ダブルクリックでファイルを開く処理
	void AssetsTreeViewItem_OnMouseDoubleClick(Object ^s, System::Windows::Input::MouseButtonEventArgs ^e)
	{
		auto t = (TreeViewItem^)s;

		auto name = GetFilePath((TestContent::Person^)t->DataContext);
		pin_ptr<const wchar_t> wch = PtrToStringChars(name);
		std::string* str = string_cast(name);
		Data::MyPostMessage(MyWindowMessage::OpenAsset, (void*)str);

	}

	//ドラッグ中の判定
	void OnDragOver2(Object ^sender, DragEventArgs ^e)
	{
		// ドロップされたデータがTreeVireItemか？
		if (e->Data->GetDataPresent(TreeViewItem::typeid))
		{
			e->Effects = DragDropEffects::Copy;
			return;
		}
		//ドロップされたデータがWindowsファイルか？
		else if (e->Data->GetDataPresent(System::Windows::Forms::DataFormats::FileDrop))
		{
			e->Effects = DragDropEffects::All;
			return;
		}

		e->Effects = DragDropEffects::None;
	}

	//外部ファイルをドロップした時の処理
	void OnDrop(Object ^s, DragEventArgs ^e)
	{
		// ドロップされたデータがTreeVireItemか？
		if (e->Data->GetDataPresent(TreeViewItem::typeid)){

			auto dragtreeitem = (TreeViewItem^)e->Data->GetData(TreeViewItem::typeid);
			auto dragitem = dynamic_cast<TestContent::Person^>(dragtreeitem->DataContext);

			//アクターをD&Dしているか
			if (dragitem->DataPtr != (IntPtr)NULL){

				AssetTreeView_ActorDrop(s, dragitem);
			}
			else{

			}
		}
		//Windowsファイルをドロップしたか？
		else if (e->Data->GetDataPresent(System::Windows::Forms::DataFormats::FileDrop)){
			AssetTreeView_WindowsFileDrop(e);

		}

	}

	void AssetTreeView_ActorDrop(Object ^s, TestContent::Person^ dragitem){

		std::string* str = string_cast(dragitem->Name);

		Data::MyPostMessage(MyWindowMessage::StackIntPtr, (void*)dragitem->DataPtr);
		Data::MyPostMessage(MyWindowMessage::CreateActorToPrefab, (void*)str);
	}

	void AssetTreeView_WindowsFileDrop(DragEventArgs ^e){

		auto t = (array<String^>^)e->Data->GetData(System::Windows::Forms::DataFormats::FileDrop, false);
		if (t[0]->Contains(".pmx")){
			std::string* str = string_cast(t[0]);
			Data::MyPostMessage(MyWindowMessage::CreatePMXtoTEStaticMesh, str);
		}

	}

	void AssetTreeView_OtherDrop(){

	}


//アセットツリービュー
#pragma endregion


	//ツリービュー共通ドラッグ開始処理
	void TreeViewItem_OnMouseLeave(Object ^s, System::Windows::Input::MouseEventArgs ^e)
	{
		auto item = (TreeViewItem^)s;
		if (e->LeftButton == MouseButtonState::Pressed
			&& e->RightButton == MouseButtonState::Released
			&& e->MiddleButton == MouseButtonState::Released)
			DragDrop::DoDragDrop(
			item, // ドラッグされる物
			item, // 渡すデータ
			DragDropEffects::Copy); // D&Dで許可するオペレーション
	}


#pragma region メニューバー

public:

private:
	void PlayButton_Click(Object ^s, System::Windows::RoutedEventArgs ^e)
	{
		Data::MyPostMessage(MyWindowMessage::PlayGame);
	}
	void StopButton_Click(Object ^s, System::Windows::RoutedEventArgs ^e)
	{
		Data::MyPostMessage(MyWindowMessage::StopGame);
	}
	void CompileButton_Click(Object ^s, System::Windows::RoutedEventArgs ^e)
	{
		Data::MyPostMessage(MyWindowMessage::ScriptCompile);
	}
	void SaveButton_Click(Object ^s, System::Windows::RoutedEventArgs ^e)
	{
		Data::MyPostMessage(MyWindowMessage::SaveScene);
	}


//メニューバー
#pragma endregion 

protected:

	//初期化処理
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
	property HWND GameScreenHWND{
		HWND get(){
			return mGameScreenHWND;
		}
	}
private:

	GameScreen ^m_GameScreen;
	HWND mGameScreenHWND;

	StackPanel ^m_ComponentPanel;

	TreeView ^ m_TreeView;
	TestContent::Person ^m_TreeViewItemRoot;
	TextBlock ^m_ActorIntPtrDataBox;


};