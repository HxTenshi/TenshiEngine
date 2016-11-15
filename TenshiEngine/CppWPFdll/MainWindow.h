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


//フォルダのルートからの相対パスを取得
String^ GetFolderPath(TestContent::Person^ item){
	if (item->Name == "$__Root__")return "";
	return GetFolderPath(item->Parent) + item->Name + "/";
}
//ファイルのルートからの相対パスを取得
String^ GetFilePath(TestContent::Person^ item){
	if (item->Name == "$__Root__")return "";
	return GetFolderPath(item->Parent) + item->Name;
}

//using namespace System::Windows::Input;
//using namespace System::Windows;
using System::Windows::Controls::MenuItem;
using System::Runtime::InteropServices::Marshal;
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
		,m_ML(NULL)
		,m_MR(NULL)
		,m_MX(NULL)
		,m_MY(NULL)
		,m_WX(NULL)
		,m_WY(NULL)
		,m_Focus(NULL)
	{
		auto panel = gcnew GameScreenPanel();
		m_wfh->Child = panel;
		auto par = panel->Parent;

		m_wfh->VerticalAlignment = System::Windows::VerticalAlignment::Stretch;
		m_wfh->HorizontalAlignment = System::Windows::HorizontalAlignment::Stretch;
		m_wfh->SizeChanged += gcnew System::Windows::SizeChangedEventHandler(this, &GameScreen::OnSizeChanged);

		//m_wfh->Child->AllowDrop = true;
		//m_wfh->Child->DragDrop += gcnew System::Windows::Forms::DragEventHandler(this, &GameScreen::OnDrop);
		//m_wfh->Child->DragEnter += gcnew System::Windows::Forms::DragEventHandler(this, &GameScreen::OnDragOver);
		//m_wfh->Child->DragOver += gcnew System::Windows::Forms::DragEventHandler(this, &GameScreen::OnDragOver);

		//十字キーやTABでフォーカスをロックする
		m_wfh->PreviewKeyDown += gcnew System::Windows::Input::KeyEventHandler(this, &GameScreen::keyLock);

		m_wfh->IsKeyboardFocusedChanged += gcnew System::Windows::DependencyPropertyChangedEventHandler(this, &GameScreen::focusChanged);
		
	}

	void OnSizeChanged(Object^ sender, System::Windows::SizeChangedEventArgs^ e)
	{
		LockAspect(e->NewSize);
		e->Handled = true;
	}
	void LockAspect(System::Windows::Size^ size){
		auto w = size->Width;
		auto h = size->Height;
		auto as = w / h;
		if (as < 16.0 / 9.0){
			h = w * (9.0 / 16.0);
		}
		else if (as > 16.0 / 9.0){
			w = h * (16.0 / 9.0);
		}
		m_wfh->Child->Height = (int)h;
		m_wfh->Child->Width = (int)w;
	}

	//void ml(Object^ sender, MouseButtonEventArgs ^e){
	//	*m_ML = (bool)e->LeftButton;
	//}
	//void mr(Object^ sender, MouseButtonEventArgs ^e){
	//	*m_MR = (bool)e->RightButton;
	//}

	void focusChanged(Object^ sender, System::Windows::DependencyPropertyChangedEventArgs e){
		if(m_Focus)
			*m_Focus = (bool)e.NewValue;
	}

	//十字キーやTABでフォーカスをロックする
	void keyLock(Object^ sender, System::Windows::Input::KeyEventArgs ^e){
		e->Handled = true;
	}

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
		auto scr = (System::Windows::Forms::Panel^)sender;
		//auto p = e->P(m_wfh);
		*m_MX = (int)e->X;
		*m_MY = (int)e->Y;

		*m_WX = scr->Width;
		*m_WY = scr->Height;
	}
	//void keypressnull(Object^ sender, System::Windows::Forms::KeyEventArgs ^e){
	//	e->Handled = true;
	//}
	//void keynull(Object^ sender, System::Windows::Forms::KeyEventArgs ^e){
	//	e->Handled = true;
	//}
	void SetMouseEvents(bool* focus,bool* l, bool* r, int* x, int* y, int* wx, int* wy){

		m_Focus = focus;
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
		System::IntPtr handle = m_wfh->Child->Handle;

		return reinterpret_cast<HWND>(handle.ToPointer());
	}
	//ゲーム画面用のHWNDを取得するためのWindowsFormsHost

private:

	void OnDrop(Object ^s, System::Windows::Forms::DragEventArgs ^e)
	{
		//TreeViewItem ^t = HandleDragEnter<TreeViewItem>(e);
		//
		//auto name = GetFilePath((TestContent::Person^)t->DataContext);
		//std::string* str = string_cast(name);
		//Data::MyPostMessage(MyWindowMessage::CreatePrefabToActor, (void*)str);

	}

	void OnDragOver(Object ^sender, System::Windows::Forms::DragEventArgs ^e)
	{
		//TreeViewItem ^t = HandleDragEnter<TreeViewItem>(e);
		//if (((String^)t->Header)->Contains(".prefab")){
		//	e->Effect = System::Windows::Forms::DragDropEffects::Copy;
		//	return;
		//}
		//e->Effect = System::Windows::Forms::DragDropEffects::None;
	}

	WindowsFormsHost ^m_wfh;
	bool* m_Focus;
	bool* m_ML;
	bool* m_MR;
	int* m_MX;
	int* m_MY;
	int* m_WX;
	int* m_WY;
};

ref class ColorPickerWindow : public Window {
public:
	static ColorPickerWindow ^mUnique = nullptr;
	static void WindowClose(){
		if (mUnique){
			mUnique->Close();
			mUnique = nullptr;
		}
	}

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

ref class CreateScriptWindow : public Window {
public:
	static CreateScriptWindow ^mUnique = nullptr;

	CreateScriptWindow(Window^ owner)
	{
		if (mUnique){
			mUnique->Close();
		}
		mUnique = this;

		Owner = owner;
		Title = "CreateScritp";
		Width = 400;
		Height = 50 + 36;

		FrameworkElement ^colwnd = LoadContentsFromResource(IDR_POPWND_CREATE);
		Content = colwnd;
		auto createbutton = (Button^)colwnd->FindName("CreateButton");
		createbutton->Click += gcnew System::Windows::RoutedEventHandler(this, &CreateScriptWindow::CreateButton);
		auto cancelbutton = (Button^)colwnd->FindName("CancelButton");
		cancelbutton->Click += gcnew System::Windows::RoutedEventHandler(this, &CreateScriptWindow::CancelButton);


		mTextBox = (TextBox^)colwnd->FindName("CreateTextBox");

		this->ShowActivated = true;
		Show();
		Visibility = System::Windows::Visibility::Visible;
		this->Activate();
	}

protected:
	void OnActivated(System::EventArgs^ e) override{
		Window::OnActivated(e);
		mTextBox->Focus();
	}

private:

	void CreateButton(Object ^s, System::Windows::RoutedEventArgs ^e){
		Data::MyPostMessage(MyWindowMessage::CreateScriptFile, string_cast(mTextBox->Text));
		Close();
	}
	void CancelButton(Object ^s, System::Windows::RoutedEventArgs ^e){
		Close();
	}
	TextBox ^mTextBox;

};

ref class CreateAssetWindow : public Window {
public:
	static CreateAssetWindow ^mUnique = nullptr;

	CreateAssetWindow(Window^ owner,String^ ex)
	{
		if (mUnique){
			mUnique->Close();
		}
		mEx = ex;
		mUnique = this;
		Owner = owner;
		Title = "CreateAsset";
		Width = 400;
		Height = 50 + 36;


FrameworkElement ^colwnd = LoadContentsFromResource(IDR_POPWND_CREATE);
Content = colwnd;
auto createbutton = (Button^)colwnd->FindName("CreateButton");
createbutton->Click += gcnew System::Windows::RoutedEventHandler(this, &CreateAssetWindow::CreateButton);
auto cancelbutton = (Button^)colwnd->FindName("CancelButton");
cancelbutton->Click += gcnew System::Windows::RoutedEventHandler(this, &CreateAssetWindow::CancelButton);


mTextBox = (TextBox^)colwnd->FindName("CreateTextBox");

this->ShowActivated = true;
Show();
Visibility = System::Windows::Visibility::Visible;
this->Activate();
	}

protected:
	void OnActivated(System::EventArgs^ e) override {
		Window::OnActivated(e);
		mTextBox->Focus();
	}

private:

	void CreateButton(Object ^s, System::Windows::RoutedEventArgs ^e) {
		Data::MyPostMessage(MyWindowMessage::CreateAssetFile, string_cast(mTextBox->Text + mEx));
		Close();
	}
	void CancelButton(Object ^s, System::Windows::RoutedEventArgs ^e) {
		Close();
	}
	TextBox ^mTextBox;
	String ^mEx;

};

ref class NewFolderWindow : public Window {
public:
	static NewFolderWindow ^mUnique = nullptr;

	NewFolderWindow(Window^ owner)
	{
		if (mUnique) {
			mUnique->Close();
		}
		mUnique = this;
		Owner = owner;
		Title = "New Folder";
		Width = 400;
		Height = 50 + 36;


		FrameworkElement ^colwnd = LoadContentsFromResource(IDR_POPWND_CREATE);
		Content = colwnd;
		auto createbutton = (Button^)colwnd->FindName("CreateButton");
		createbutton->Click += gcnew System::Windows::RoutedEventHandler(this, &NewFolderWindow::CreateButton);
		auto cancelbutton = (Button^)colwnd->FindName("CancelButton");
		cancelbutton->Click += gcnew System::Windows::RoutedEventHandler(this, &NewFolderWindow::CancelButton);


		mTextBox = (TextBox^)colwnd->FindName("CreateTextBox");


		this->ShowActivated = true;
		Show();
		Visibility = System::Windows::Visibility::Visible;
		this->Activate();
	}

protected:
	void OnActivated(System::EventArgs^ e) override {
		Window::OnActivated(e);
		mTextBox->Focus();
	}


private:
	void CreateButton(Object ^s, System::Windows::RoutedEventArgs ^e) {
		System::IO::Directory::CreateDirectory("Assets/" + mTextBox->Text);
		Close();
	}
	void CancelButton(Object ^s, System::Windows::RoutedEventArgs ^e) {
		Close();
	}
	TextBox ^mTextBox;
	String ^mEx;

};

ref class RenameWindow : public Window {
public:
	static RenameWindow ^mUnique = nullptr;

	RenameWindow(Window^ owner, TestContent::Person^ data)
	{
		if (!data || !data->Parent) {
			Close();
			return;
		}
		if (data->Parent->Name == "$__Root__"){
			Close();
			return;
		}
		if (mUnique) {
			mUnique->Close();
		}
		mUnique = this;
		Owner = owner;
		Title = "Rename";
		Width = 400;
		Height = 50 + 36;

		mData = data;

		FrameworkElement ^colwnd = LoadContentsFromResource(IDR_POPWND_CREATE);
		Content = colwnd;
		auto createbutton = (Button^)colwnd->FindName("CreateButton");
		createbutton->Click += gcnew System::Windows::RoutedEventHandler(this, &RenameWindow::CreateButton);
		auto cancelbutton = (Button^)colwnd->FindName("CancelButton");
		cancelbutton->Click += gcnew System::Windows::RoutedEventHandler(this, &RenameWindow::CancelButton);

		mTextBox = (TextBox^)colwnd->FindName("CreateTextBox");
		mTextBox->Text = mData->Name;



		this->ShowActivated = true;
		Show();
		Visibility = System::Windows::Visibility::Visible;
		this->Activate();
	}

protected:
	void OnActivated(System::EventArgs^ e) override {
		Window::OnActivated(e);
		mTextBox->Focus();
	}


private:
	void CreateButton(Object ^s, System::Windows::RoutedEventArgs ^e) {

		auto oldname = GetFilePath(mData);
		auto newname = GetFolderPath(mData->Parent) + mTextBox->Text;
		System::IO::Directory::Move(oldname, newname);
		Close();
	}
	void CancelButton(Object ^s, System::Windows::RoutedEventArgs ^e) {
		Close();
	}
	TextBox ^mTextBox;
	TestContent::Person^ mData;

};

ref class MyTreeViewItem : public TextBlock{

};

ref class MyTreeView{
protected:

	MyTreeView(System::Windows::Controls::Decorator ^dec){
		//ツリービュー作成
		{
			auto sp = (Panel^)LoadContentsFromResource(IDR_TREEVIEW);

			dec->Child = sp;
			auto treeView = (TreeView^)sp->FindName("treeView1");
			m_TreeView = treeView;
		}

		//アイテムリスト作成
		{
			//アイテムリストのルートを作成
			m_TreeViewItemRoot = gcnew TestContent::Person("$__Root__", gcnew TestContent::MyList());
			m_TreeView->DataContext = m_TreeViewItemRoot;
			auto source = gcnew System::Windows::Data::Binding("Children");
			source->Mode = System::Windows::Data::BindingMode::TwoWay;
			m_TreeView->SetBinding(TreeView::ItemsSourceProperty, source);
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

			datatemp->VisualTree = fact;

			m_TreeView->ItemTemplate = datatemp;
		}
	}

	TreeView ^ m_TreeView;
	TestContent::Person ^m_TreeViewItemRoot;

	//セレクトの解除
	void MyTreeView_OnLostKeyboardFocus(System::Object ^s, System::Windows::Input::KeyboardFocusChangedEventArgs ^e){
		auto t = (TreeViewItem^)s;
		t->IsSelected = false;
	}

	//セレクトの解除
	void MyTreeView_IsSelectedChanged(System::Object ^s, System::Windows::Input::KeyboardFocusChangedEventArgs ^e){
		auto t = (TreeViewItem^)s;
		t->IsSelected = false;
	}

	TreeViewItem^ m_DDItem = nullptr;
	//ツリービュー共通ドラッグ開始処理
	void TreeViewItem_OnMouseLeftClick(Object ^s, System::Windows::Input::MouseButtonEventArgs ^e)
	{
		auto item = (TreeViewItem^)s;
		m_DDItem = item;
	}

	void TreeViewItem_OnMouseRightClick(Object ^s, System::Windows::Input::MouseButtonEventArgs ^e)
	{
		auto t = (TreeViewItem^)s;
		t->IsSelected = true;
	}

	//ツリービュー共通ドラッグ開始処理
	void TreeViewItem_OnMouseLeave(Object ^s, System::Windows::Input::MouseEventArgs ^e)
	{
		//auto item = (TreeViewItem^)s;
		if (e->LeftButton == MouseButtonState::Released) {
			m_DDItem = nullptr;
		}
		if(m_DDItem != nullptr)
		if (e->LeftButton == MouseButtonState::Pressed
			&& e->RightButton == MouseButtonState::Released
			&& e->MiddleButton == MouseButtonState::Released)
			DragDrop::DoDragDrop(
			m_DDItem, // ドラッグされる物
			m_DDItem, // 渡すデータ
			DragDropEffects::Copy); // D&Dで許可するオペレーション

		e->Handled = true;
	}

};

ref class SceneTreeView : public MyTreeView{
public:

	SceneTreeView(System::Windows::Controls::Decorator ^dec)
		:MyTreeView(dec)
	{
		CreateTreeView();
	}

private:

	//シーンツリービュー作成
	void CreateTreeView(){

		//styleのセット
		{
			//今のところ上で設定しているので意味なし?
			auto s = gcnew System::Windows::Style(TreeViewItem::typeid);
			auto setter = s->Setters;
			//setter->Add(gcnew System::Windows::Setter(TreeViewItem::ForegroundProperty, gcnew SolidColorBrush(Color::FromRgb(240, 240, 240))));
			//setter->Add(gcnew System::Windows::Setter(TreeViewItem::BackgroundProperty, gcnew SolidColorBrush(Color::FromRgb(5, 147, 14 * 16 + 2))));
			setter->Add(gcnew System::Windows::EventSetter(TreeViewItem::DropEvent, gcnew DragEventHandler(this, &SceneTreeView::ActorTreeView_OnDrop)));
			setter->Add(gcnew System::Windows::EventSetter(TreeViewItem::PreviewMouseLeftButtonDownEvent, gcnew System::Windows::Input::MouseButtonEventHandler(this, &SceneTreeView::TreeViewItem_OnMouseLeftClick)));
			setter->Add(gcnew System::Windows::EventSetter(TreeViewItem::MouseLeaveEvent, gcnew System::Windows::Input::MouseEventHandler(this, &SceneTreeView::TreeViewItem_OnMouseLeave)));
			setter->Add(gcnew System::Windows::EventSetter(TreeViewItem::DragEnterEvent, gcnew DragEventHandler(this, &SceneTreeView::ActorTreeView_OnDragOver)));
			setter->Add(gcnew System::Windows::EventSetter(TreeViewItem::DragOverEvent, gcnew DragEventHandler(this, &SceneTreeView::ActorTreeView_OnDragOver)));
			setter->Add(gcnew System::Windows::EventSetter(TreeViewItem::MouseDoubleClickEvent, gcnew System::Windows::Input::MouseButtonEventHandler(this, &SceneTreeView::SceneTreeViewItem_OnMouseDoubleClick)));

			setter->Add(gcnew System::Windows::EventSetter(TreeViewItem::SelectedEvent, gcnew System::Windows::RoutedEventHandler(this, &SceneTreeView::SceneTreeViewItem_OnSelected)));
			setter->Add(gcnew System::Windows::EventSetter(TreeViewItem::MouseLeftButtonUpEvent, gcnew System::Windows::Input::MouseButtonEventHandler(this, &SceneTreeView::SceneTreeViewItem_OnMouseClickUp)));

			setter->Add(gcnew System::Windows::EventSetter(TreeViewItem::KeyDownEvent, gcnew System::Windows::Input::KeyEventHandler(this, &SceneTreeView::TreeViewItemKeyDown)));

			setter->Add(gcnew System::Windows::EventSetter(TreeViewItem::PreviewMouseRightButtonDownEvent, gcnew System::Windows::Input::MouseButtonEventHandler(this, &SceneTreeView::TreeViewItem_OnMouseRightClick)));


			auto itemselectbind = gcnew System::Windows::Data::Binding("IsSelected");
			itemselectbind->Mode = BindingMode::TwoWay;
			setter->Add(gcnew System::Windows::Setter(TreeViewItem::IsSelectedProperty, itemselectbind));


			//コンテキストメニュー作成
			{
				auto cm = gcnew System::Windows::Controls::ContextMenu();
				{//コンテキストメニューの要素作成
					auto mitem = gcnew System::Windows::Controls::MenuItem();
					mitem->Header = "Remove";
					mitem->Click += gcnew System::Windows::RoutedEventHandler(this, &SceneTreeView::MenuItem_Click_Remove);
					cm->Items->Add(mitem);
				}
				//ツリービューに反映
				setter->Add(gcnew System::Windows::Setter(TreeViewItem::ContextMenuProperty, cm));
			}

			m_TreeView->ItemContainerStyle = s;


			m_TreeView->AllowDrop = true;
			m_TreeView->Drop += gcnew DragEventHandler(this, &SceneTreeView::ActorTreeView_OnDrop);
			m_TreeView->DragEnter += gcnew DragEventHandler(this, &SceneTreeView::ActorTreeView_OnDragOver);
			m_TreeView->DragOver += gcnew DragEventHandler(this, &SceneTreeView::ActorTreeView_OnDragOver);
		}


		//コンテキストメニュー作成
		{
			auto cm = gcnew System::Windows::Controls::ContextMenu();
			//ツリービューに反映
			m_TreeView->ContextMenu = cm;
		}

	}



public:
	System::Windows::Controls::ContextMenu^ GetContextMenu(){
		return m_TreeView->ContextMenu;
	}

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
		auto c = (gcroot<TestContent::Person^>*)(void*)child;
		if (parent != IntPtr(0)) {
			auto p = (gcroot<TestContent::Person^>*)(void*)parent;
			(*p)->Add(*c);
		}
		else {
			m_TreeViewItemRoot->Add(*c);
		}
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
	//ツリービューアイテムの選択
	void SelectTreeViewItem(IntPtr treeviewptr){
		auto i = (gcroot<TestContent::Person^>*)(void*)treeviewptr;
		//一度展開されていないと選択できない
		(*i)->IsSelected = true;
	}
private:

	void TreeViewItemKeyDown(Object ^sender, System::Windows::Input::KeyEventArgs^ e) {
		if (e->Key == System::Windows::Input::Key::Delete) {
			auto t = (TreeViewItem^)sender;
			auto item = (TestContent::Person^)t->DataContext;
			Data::MyPostMessage(MyWindowMessage::ActorDestroy, (void*)item->DataPtr);
			
		}
		e->Handled = true;
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



	//ファイルの選択
	void SceneTreeViewItem_OnSelected(Object ^s, System::Windows::RoutedEventArgs ^e)
	{
		//左クリックをしていなければ（キーボードで選択していれば）
		if (System::Windows::Input::Mouse::LeftButton == System::Windows::Input::MouseButtonState::Released){
			auto t = (TreeViewItem^)s;

			_Selected(t);

			e->Handled = true;
		}

	}
	//クリックでファイルの選択
	void SceneTreeViewItem_OnMouseClickUp(Object ^s, System::Windows::Input::MouseButtonEventArgs ^e)
	{

		auto t = (TreeViewItem^)s;
		_Selected(t);
		e->Handled = true;
	}

	//ツリービューアイテムの選択
	void _Selected(TreeViewItem^ t){

		auto item = (TestContent::Person^)t->DataContext;
		if (m_TreeView->SelectedItem != t){
			Data::MyPostMessage(MyWindowMessage::SelectActor, (void*)item->DataPtr);
		}
		_ExpandedAll(m_TreeView, t);
		
	}

	//指定のツリービューアイテムまで展開
	void _ExpandedAll(System::Windows::Controls::ItemsControl ^topItemsControl, System::Windows::DependencyObject ^buttomControl)
	{
		auto tmp = topItemsControl;
		TreeViewItem ^back = nullptr;
		while (tmp != nullptr)
		{
			tmp = (System::Windows::Controls::ItemsControl ^)System::Windows::Controls::ItemsControl::ContainerFromElement(tmp, buttomControl);
			if (tmp != nullptr)
			{
				//最後を展開しないため１巡遅らせる
				if (back){
					back->IsExpanded = true;
				}
				back = (TreeViewItem ^)tmp;
			}
		}
	}


	
	//コンテキストメニューのゲームオブジェクトの削除処理
	void MenuItem_Click_Remove(Object ^sender, System::Windows::RoutedEventArgs ^e){

		//カーソルを変えないとダメ
		//要素があるときのみ
		//if (m_TreeViewItemRoot->Children->Count != 0){
		//セレクトし直す
		Data::MyPostMessage(MyWindowMessage::SelectActor, NULL);

		if (m_TreeView->SelectedItem == nullptr)return;
		auto i = (TestContent::Person^)m_TreeView->SelectedItem;
		Data::MyPostMessage(MyWindowMessage::ActorDestroy, (void*)i->DataPtr);

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
		if (!name->Contains(".prefab"))return;
		std::string* str = string_cast(name);
		Data::MyPostMessage(MyWindowMessage::CreatePrefabToActor, (void*)str);
	}


	//ダブルクリックの処理
	void SceneTreeViewItem_OnMouseDoubleClick(Object ^s, System::Windows::Input::MouseButtonEventArgs ^e)
	{

		auto i = (TestContent::Person^)m_TreeView->SelectedItem;
		auto t = (TreeViewItem^)s;
		auto per = (TestContent::Person^)t->DataContext;
		if (per != i)return;
		Data::MyPostMessage(MyWindowMessage::ActorDoubleClick, (void*)per->DataPtr);

		e->Handled = true;
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


};

ref class AssetTreeView : public MyTreeView{
public:

	AssetTreeView(Window ^parent, System::Windows::Controls::Decorator ^dec)
		:MyTreeView(dec)
		,m_ParentWindow(parent){
		CreateTreeView();
	}

private:
	Window ^m_ParentWindow;

	//アセットツリービュー作成
	void CreateTreeView(){


		//styleのセット
		{
			//今のところ上で設定しているので意味なし?
			auto s = gcnew System::Windows::Style(TreeViewItem::typeid);
			auto setter = s->Setters;
			setter->Add(gcnew System::Windows::EventSetter(TreeViewItem::ExpandedEvent, gcnew System::Windows::RoutedEventHandler(this, &AssetTreeView::TreeView_Expanded)));
			setter->Add(gcnew System::Windows::EventSetter(TreeViewItem::DropEvent, gcnew DragEventHandler(this, &AssetTreeView::OnDrop)));
			setter->Add(gcnew System::Windows::EventSetter(TreeViewItem::PreviewMouseLeftButtonDownEvent, gcnew System::Windows::Input::MouseButtonEventHandler(this, &AssetTreeView::TreeViewItem_OnMouseLeftClick)));
			setter->Add(gcnew System::Windows::EventSetter(TreeViewItem::MouseLeaveEvent, gcnew System::Windows::Input::MouseEventHandler(this, &AssetTreeView::TreeViewItem_OnMouseLeave)));
			
			//setter->Add(gcnew System::Windows::EventSetter(TreeViewItem::LostKeyboardFocusEvent, gcnew System::Windows::Input::KeyboardFocusChangedEventHandler(this, &AssetTreeView::MyTreeView_OnLostKeyboardFocus)));
			setter->Add(gcnew System::Windows::EventSetter(TreeViewItem::SelectedEvent, gcnew System::Windows::RoutedEventHandler(this, &AssetTreeView::AssetsTreeViewItem_OnSelected)));
			setter->Add(gcnew System::Windows::EventSetter(TreeViewItem::MouseLeftButtonUpEvent, gcnew System::Windows::Input::MouseButtonEventHandler(this, &AssetTreeView::AssetsTreeViewItem_OnMouseClickUp)));
			setter->Add(gcnew System::Windows::EventSetter(TreeViewItem::MouseDoubleClickEvent, gcnew System::Windows::Input::MouseButtonEventHandler(this, &AssetTreeView::AssetsTreeViewItem_OnMouseDoubleClick)));
			//setter->Add(gcnew System::Windows::EventSetter(TreeViewItem::DragEnterEvent, gcnew DragEventHandler(this, &View::ActorTreeView_OnDragOver)));
			//setter->Add(gcnew System::Windows::EventSetter(TreeViewItem::DragOverEvent, gcnew DragEventHandler(this, &View::ActorTreeView_OnDragOver)));
			
			setter->Add(gcnew System::Windows::EventSetter(TreeViewItem::PreviewMouseRightButtonDownEvent, gcnew System::Windows::Input::MouseButtonEventHandler(this, &AssetTreeView::TreeViewItem_OnMouseRightClick)));

			auto itemselectbind = gcnew System::Windows::Data::Binding("IsSelected");
			itemselectbind->Mode = BindingMode::TwoWay;
			setter->Add(gcnew System::Windows::Setter(TreeViewItem::IsSelectedProperty, itemselectbind));

			//コンテキストメニュー作成
			{
				auto cm = gcnew System::Windows::Controls::ContextMenu();
				{//コンテキストメニューの要素作成
					auto mitem = gcnew System::Windows::Controls::MenuItem();
					mitem->Header = "Rename";
					mitem->Click += gcnew System::Windows::RoutedEventHandler(this, &AssetTreeView::MenuItem_Click_Rename);
					cm->Items->Add(mitem);
				}
				//ツリービューに反映
				setter->Add(gcnew System::Windows::Setter(TreeViewItem::ContextMenuProperty, cm));
			}

			m_TreeView->ItemContainerStyle = s;

			m_TreeView->AllowDrop = true;
			m_TreeView->Drop += gcnew DragEventHandler(this, &AssetTreeView::OnDrop);
			m_TreeView->DragEnter += gcnew DragEventHandler(this, &AssetTreeView::OnDragOver2);
			m_TreeView->DragOver += gcnew DragEventHandler(this, &AssetTreeView::OnDragOver2);
		}


		CreateAssetTreeViewItem_StartFolder(m_TreeViewItemRoot, "EngineResource");
		CreateAssetTreeViewItem_StartFolder(m_TreeViewItemRoot, "Assets");
		CreateAssetTreeViewItem_StartFolder(m_TreeViewItemRoot, "ScriptComponent/Scripts");

		//FileSystemEvent("Assets");
		//FileSystemEvent("EngineResource");

		//コンテキストメニュー作成
		{
			auto cm = gcnew System::Windows::Controls::ContextMenu();


			{//コンテキストメニューの要素作成
				auto mitem = gcnew System::Windows::Controls::MenuItem();
				mitem->Header = "New Folder";
				mitem->Click += gcnew System::Windows::RoutedEventHandler(this, &AssetTreeView::MenuItem_Click_NewFolder);
				cm->Items->Add(mitem);
			}

			//{//コンテキストメニューの要素作成
			//	auto mitem = gcnew System::Windows::Controls::MenuItem();
			//	mitem->Header = "Rename";
			//	mitem->Click += gcnew System::Windows::RoutedEventHandler(this, &AssetTreeView::MenuItem_Click_Rename);
			//	cm->Items->Add(mitem);
			//}

			{//コンテキストメニューの要素作成
				auto mitem = gcnew System::Windows::Controls::MenuItem();
				mitem->Header = "CreateScritp";
				mitem->Click += gcnew System::Windows::RoutedEventHandler(this, &AssetTreeView::MenuItem_Click_CreateScript);
				cm->Items->Add(mitem);
			}

			{//コンテキストメニューの要素作成
				auto mitem = gcnew System::Windows::Controls::MenuItem();
				mitem->Header = "CreatePhysxMaterial";
				mitem->DataContext = ".pxmaterial";
				mitem->Click += gcnew System::Windows::RoutedEventHandler(this, &AssetTreeView::MenuItem_Click_CreateAsset);
				cm->Items->Add(mitem);
			}

			//ツリービューに反映
			m_TreeView->ContextMenu = cm;
		}
	}


	//コンテキストメニューのオブジェクト作成
	void MenuItem_Click_NewFolder(Object ^sender, System::Windows::RoutedEventArgs ^e){

		auto w = gcnew NewFolderWindow(m_ParentWindow);

		e->Handled = true;
	}

	//コンテキストメニューのオブジェクト作成
	void MenuItem_Click_Rename(Object ^sender, System::Windows::RoutedEventArgs ^e) {

		auto item = (TestContent::Person^)m_TreeView->SelectedItem;

		auto w = gcnew RenameWindow(m_ParentWindow, item);

		e->Handled = true;
	}

	//コンテキストメニューのオブジェクト作成
	void MenuItem_Click_CreateScript(Object ^sender, System::Windows::RoutedEventArgs ^e){

		auto w = gcnew CreateScriptWindow(m_ParentWindow);

		e->Handled = true;
	}

	//コンテキストメニューのオブジェクト作成
	void MenuItem_Click_CreateAsset(Object ^sender, System::Windows::RoutedEventArgs ^e){

		auto ex = (String^)((System::Windows::Controls::MenuItem^)sender)->DataContext;
		auto w = gcnew CreateAssetWindow(m_ParentWindow,ex);

		e->Handled = true;
	}

	void UpdateEmptyFolder(TestContent::Person^ item){
		//フォルダなら
		auto path = GetFilePath(item);
		if (IsDirectory(path)){
			//空のフォルダの場合　空のアイテムを追加
			if (item->Children->Count == 0){
				auto nullitem = CreateAssetItem("");
				item->Add(nullitem);
			}
		}
	}

	delegate void fsChange(Object^ source, System::IO::FileSystemEventArgs^ e);
	void OnFSEvent(Object^ source, System::IO::FileSystemEventArgs^ e){

		fsChange ^del;
		if (e->ChangeType == System::IO::WatcherChangeTypes::Changed){
			del = gcnew fsChange(this, &AssetTreeView::OnChanged);
		}
		else if (e->ChangeType == System::IO::WatcherChangeTypes::Created){
			del = gcnew fsChange(this, &AssetTreeView::OnCreated);
		}
		else if (e->ChangeType == System::IO::WatcherChangeTypes::Deleted){
			del = gcnew fsChange(this, &AssetTreeView::OnDeleted);

		}

		m_ParentWindow->Dispatcher->Invoke(del, source, e);
	}

	void OnChanged(Object^ source, System::IO::FileSystemEventArgs^ e)
	{
		
		std::string* str = string_cast(e->FullPath);
		Data::MyPostMessage(MyWindowMessage::AssetFileChanged, (void*)str);
	}

	//TestContent::Person ^ DirSearch(String^ dir, TestContent::Person ^par){
	//	if (!par)return nullptr;
	//
	//	auto count = par->Children->Count;
	//	for (int i = 0; i < count;i++){
	//		auto c = par->Children[i];
	//		if (c->Name == dir){
	//			return c;
	//		}
	//		c = DirSearch(dir,c);
	//		if (c){
	//			return c;
	//		}
	//	}
	//
	//	return nullptr;
	//
	//}
	TestContent::Person ^ DirItemSearch(String ^folder, TestContent::Person ^par){
		if (!par)return nullptr;

		auto count = par->Children->Count;
		for (int j = 0; j < count; j++){
			auto c = par->Children[j];
			if (c->Name == folder){
				return c;
			}
		}

		return nullptr;
	}
	TestContent::Person ^ FullDirSearch(String ^fullpath, TestContent::Person ^par){
		if (!par)return nullptr;


		array<String^> ^r = gcnew array<String^>(1);
		r[0] = "\\";
		auto file = fullpath->Split(r, System::StringSplitOptions::None);

		auto currentPar = par;
		auto num = file->Length - 1;
		for (int i = 0; i < num; i++){

			auto filder = file[i];
			currentPar = DirItemSearch(filder, currentPar);
		
		}
		if (currentPar == par)return nullptr;

		return currentPar;
	}
	TestContent::Person ^ FullDirItemSearch(String ^fullpath, TestContent::Person ^par){
		if (!par)return nullptr;


		array<String^> ^r = gcnew array<String^>(1);
		r[0] = "\\";
		auto file = fullpath->Split(r, System::StringSplitOptions::None);

		auto currentPar = par;
		auto num = file->Length;
		for (int i = 0; i < num; i++){

			auto filder = file[i];
			currentPar = DirItemSearch(filder, currentPar);

		}
		if (currentPar == par)return nullptr;

		return currentPar;
	}

	bool IsDirectory(String^ path)
	{
		return System::IO::Directory::Exists(path);
	}
	void OnCreated(Object^ source, System::IO::FileSystemEventArgs^ e)
	{
		auto par = FullDirSearch(e->FullPath, m_TreeViewItemRoot);
		if (par){
			array<String^> ^r = gcnew array<String^>(1);
			r[0] = "\\";
			auto file = e->Name->Split(r, System::StringSplitOptions::None);
			auto filename = file[file->Length - 1];
			auto item = CreateAssetItem(filename);
			if (!item)return;
			par->Add(item);

			//フォルダなら
			if (IsDirectory(e->FullPath)){
				//空のフォルダの場合　空のアイテムを追加
				if (item->Children->Count == 0){
					auto nullitem = CreateAssetItem("");
					item->Add(nullitem);
				}
			}
			else{
				std::string* str = string_cast(e->FullPath);
				Data::MyPostMessage(MyWindowMessage::AssetFileCreated, (void*)str);
			}
		}

	}
	void OnDeleted(Object^ source, System::IO::FileSystemEventArgs^ e)
	{
		auto item = FullDirItemSearch(e->FullPath, m_TreeViewItemRoot);
		if (item){
			auto par = item->Parent;
			item->RemoveSelf();

			UpdateEmptyFolder(par);
		}

		std::string* str = string_cast(e->FullPath);
		Data::MyPostMessage(MyWindowMessage::AssetFileDeleted, (void*)str);
	}
	void OnRenamed(Object^ source, System::IO::RenamedEventArgs^ e)
	{
		auto item = FullDirItemSearch(e->OldFullPath, m_TreeViewItemRoot);
		if (item){

			array<String^> ^r = gcnew array<String^>(1);
			r[0] = "\\";
			auto file = e->Name->Split(r, System::StringSplitOptions::None);
			auto filename = file[file->Length - 1];
			item->Name = filename;
		}
		

		std::string* str1 = string_cast(e->OldFullPath);
		std::string* str2 = string_cast(e->FullPath);
		Data::MyPostMessage(MyWindowMessage::AssetFileRenamed_OldName, (void*)str1);
		Data::MyPostMessage(MyWindowMessage::AssetFileRenamed_NewName, (void*)str2);
	}

	void FileSystemEvent(String^ FolderPath)
	{
		auto fsWatcher = gcnew System::IO::FileSystemWatcher();
		fsWatcher->Filter = "";
		fsWatcher->Path = FolderPath;
		fsWatcher->IncludeSubdirectories = true;
		fsWatcher->NotifyFilter = static_cast<System::IO::NotifyFilters>
			(System::IO::NotifyFilters::FileName |
			System::IO::NotifyFilters::DirectoryName |
			System::IO::NotifyFilters::Attributes |
			System::IO::NotifyFilters::LastAccess |
			System::IO::NotifyFilters::LastWrite |
			System::IO::NotifyFilters::Security |
			System::IO::NotifyFilters::Size);

		fsWatcher->Changed += gcnew System::IO::FileSystemEventHandler(
			this, &AssetTreeView::OnFSEvent);
		fsWatcher->Created += gcnew System::IO::FileSystemEventHandler(
			this, &AssetTreeView::OnFSEvent);
		fsWatcher->Deleted += gcnew System::IO::FileSystemEventHandler(
			this, &AssetTreeView::OnFSEvent);
		fsWatcher->Renamed += gcnew System::IO::RenamedEventHandler(
			this, &AssetTreeView::OnRenamed);
		fsWatcher->EnableRaisingEvents = true;

	}

	//ツリービューアイテムの作成
	TestContent::Person^ CreateAssetItem(String^ name){

		if (System::IO::Path::GetExtension(name) == ".meta" ||
			name == "desktop.ini"){
			return nullptr;
		}

		auto temp = gcnew TestContent::Person(name, gcnew TestContent::MyList());
		temp->DataPtr = (IntPtr)NULL;
		return temp;
	}

	//指定フォルダから全てのフォルダとファイルを登録
	void CreateAssetTreeViewItem_StartFolder(TestContent::Person^ parent, String^ Path){
		auto dirs = gcnew System::IO::DirectoryInfo(Path);

		if (!dirs->Exists)return;

		auto folder = CreateAssetItem(Path);
		if (!folder)return;
		parent->Add(folder);

		CreateAssetTreeViewItem(folder, Path);

		UpdateEmptyFolder(folder);

		FileSystemEvent(Path);
	}

	//指定フォルダから全てのフォルダとファイルを登録
	void CreateAssetTreeViewItem(TestContent::Person^ parent, String^ Path){
		auto dirs = gcnew System::IO::DirectoryInfo(Path);

		if (!dirs->Exists)return;


		auto direnum = dirs->EnumerateDirectories();
		auto folders = direnum->GetEnumerator();


		while (folders->MoveNext()){

			auto folder = CreateAssetItem(folders->Current->Name);
			if (!folder)continue;
			parent->Add(folder);

			CreateAssetTreeViewItem(folder, folders->Current->FullName);

			//FileSystemEvent(folders->Current->FullName);

			//空のフォルダの場合　空のアイテムを追加
			if (folder->Children->Count == 0){
				auto nullitem = CreateAssetItem("");
				folder->Add(nullitem);
			}

		}

		auto fileenum = dirs->EnumerateFiles();
		auto files = fileenum->GetEnumerator();
		while (files->MoveNext()){
			auto item = CreateAssetItem(files->Current->Name);
			if (!item)continue;
			parent->Add(item);
		}

	}

	//ツリービューでフォルダを展開する処理
	void TreeView_Expanded(Object ^s, System::Windows::RoutedEventArgs ^e)
	{

		auto treeitem = (TreeViewItem^)s;
		auto par = (TestContent::Person^)treeitem->DataContext;

		par->Children->Clear();

		auto dirPath = GetFolderPath(par);

		CreateAssetTreeViewItem(par, dirPath);

		//空のフォルダの場合　空のアイテムを追加
		if (par->Children->Count == 0){
			auto nullitem = CreateAssetItem("");
			par->Add(nullitem);
		}

		e->Handled = true;
	}

	//ファイルの選択
	void AssetsTreeViewItem_OnSelected(Object ^s, System::Windows::RoutedEventArgs ^e)
	{
		//左クリックをしていなければ（キーボードで選択していれば）
		if (System::Windows::Input::Mouse::LeftButton == System::Windows::Input::MouseButtonState::Released){
			auto t = (TreeViewItem^)s;
			
			auto name = GetFilePath((TestContent::Person^)t->DataContext);
			std::string* str = string_cast(name);
			Data::MyPostMessage(MyWindowMessage::SelectAsset, (void*)str);
			
			e->Handled = true;
		}

	}
	//クリックでファイルの選択
	void AssetsTreeViewItem_OnMouseClickUp(Object ^s, System::Windows::Input::MouseButtonEventArgs ^e)
	{
		auto t = (TreeViewItem^)s;

		auto name = GetFilePath((TestContent::Person^)t->DataContext);
		std::string* str = string_cast(name);
		Data::MyPostMessage(MyWindowMessage::SelectAsset, (void*)str);

		e->Handled = true;
	}
	//ダブルクリックでファイルを開く処理
	void AssetsTreeViewItem_OnMouseDoubleClick(Object ^s, System::Windows::Input::MouseButtonEventArgs ^e)
	{
		auto t = (TreeViewItem^)s;

		auto name = GetFilePath((TestContent::Person^)t->DataContext);
		std::string* str = string_cast(name);
		Data::MyPostMessage(MyWindowMessage::OpenAsset, (void*)str);

		e->Handled = true;
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
				AssetDrop(s, dragitem);
			}
		}
		//Windowsファイルをドロップしたか？
		else if (e->Data->GetDataPresent(System::Windows::Forms::DataFormats::FileDrop)){
			AssetTreeView_WindowsFileDrop(e);

		}

		e->Handled = true;
	}

	//アセットファイルをドロップした処理
	void AssetDrop(Object ^s, TestContent::Person^ dragitem){
		auto targettreeitem = dynamic_cast<TreeViewItem^>(s);
		//ツリービューアイテムに対してDropしたか
		if (targettreeitem){
			auto targetitem = dynamic_cast<TestContent::Person^>(targettreeitem->DataContext);
			if (!targetitem)return;
			//フォルダーに対してDropしたか
			if (targetitem->Children->Count != 0){
				//ドロップ先が自分の子ならTRUE
				if (ParentCheck(dragitem, targetitem))return;

				if (!MoveFile(dragitem, targetitem))return;

			}

			//ファイルに対してDropしたか
			if (targetitem->Children->Count == 0){
				targetitem = targetitem->Parent;

				//フォルダーに対してDropしたか
				if (targetitem->Children->Count != 0){
					//ドロップ先が自分の子ならTRUE
					if (ParentCheck(dragitem, targetitem))return;

					if (!MoveFile(dragitem, targetitem))return;

				}
			}
		}
		//ツリービューアイテム外に対してDropしたなら
		else{
			//何もしない
		}
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

	bool MoveFile(TestContent::Person^ dragitem, TestContent::Person^ targetitem){

		auto sourcePath = GetFilePath(dragitem);
		auto destPath = GetFolderPath(targetitem) + dragitem->Name;

		try{
			//フォルダなら
			if (dragitem->Children->Count != 0){
				System::IO::Directory::Move(sourcePath, destPath);
			}
			else{
				System::IO::File::Move(sourcePath, destPath);
			}
		}
		catch(...){
			return false;
		}
		return true;
	}


	void AssetTreeView_ActorDrop(Object ^s, TestContent::Person^ dragitem){

		auto targettreeitem = dynamic_cast<TreeViewItem^>(s);
		//ツリービューアイテムに対してDropしたか
		if (targettreeitem) {
			auto targetitem = dynamic_cast<TestContent::Person^>(targettreeitem->DataContext);
			if (!targetitem)return;
			String^ destPath = String::Empty;
			//フォルダーに対してDropしたか
			if (targetitem->Children->Count != 0) {

				destPath = GetFolderPath(targetitem) + dragitem->Name + ".prefab";
			}

			//ファイルに対してDropしたか
			if (targetitem->Children->Count == 0) {
				targetitem = targetitem->Parent;

				//フォルダーに対してDropしたか
				if (targetitem->Children->Count != 0) {

					destPath = GetFolderPath(targetitem) + dragitem->Name + ".prefab";
				}
			}
			std::string* str = string_cast(destPath);

			Data::MyPostMessage(MyWindowMessage::StackIntPtr, (void*)dragitem->DataPtr);
			Data::MyPostMessage(MyWindowMessage::CreateActorToPrefab, (void*)str);
		}
	}

	void AssetTreeView_WindowsFileDrop(DragEventArgs ^e){

		auto t = (array<String^>^)e->Data->GetData(System::Windows::Forms::DataFormats::FileDrop, false);
		//if (t[0]->Contains(".pmx")){
		for (int i = 0; i < t->Length;i++) {
			std::string* str = string_cast(t[i]);
			Data::MyPostMessage(MyWindowMessage::CreateModelConvert, (void*)str);
		}
		//}

	}

	void AssetTreeView_OtherDrop(){

	}


};

// ビュー
ref class View : public Window {
public:

	View()
		: Window()
		, m_GameScreen(nullptr)
		, mGameScreenHWND(NULL)
		, mWindowHWND(NULL)
		, m_ComponentPanel(nullptr)
		, mSceneTreeView(nullptr)
		, mAssetTreeView(nullptr)
	{


		setlocale(LC_ALL, "Japanese");

		this->Closing += gcnew System::ComponentModel::CancelEventHandler(this, &View::Window_Closing);
		//DataContext = gcnew ViewModel();
		Background = Brushes::Black;
		//w:1184, h : 762
		//Width = 1605 + 16 + 200;
		//Height = 800 + 38;

		this->WindowState = System::Windows::WindowState::Maximized;

		InitializeLoadContentsFromResource(101,115);
		

		FrameworkElement ^contents = LoadContentsFromResource(IDR_VIEW);
		if (contents){
			Content = contents;
		}

		auto wfh = (WindowsFormsHost ^)contents->FindName("GameScreenWFH");
		m_GameScreen = gcnew GameScreen(wfh);

		m_ComponentPanel = (StackPanel ^)contents->FindName("MainDock");
		auto cm = gcnew System::Windows::Controls::ContextMenu();
		m_ComponentPanel->ContextMenu = cm;

		auto TreeViewDec = (Border ^)contents->FindName("TreeView");
		mSceneTreeView = gcnew SceneTreeView(TreeViewDec);
		m_SceneContextMenu = mSceneTreeView->GetContextMenu();

		auto EngineTreeViewDec = (Border ^)contents->FindName("EngineTreeView");
		mEngineTreeView = gcnew SceneTreeView(EngineTreeViewDec);

		auto AssetTreeViewDec = (Border ^)contents->FindName("AssetTreeView");
		mAssetTreeView = gcnew AssetTreeView(this,AssetTreeViewDec);

		auto commandBarPanel = (StackPanel ^)contents->FindName("CommandBarPanel");
		auto commandBar = LoadContentsFromResource(IDR_COMMAND_BAR);
		commandBarPanel->Children->Add(commandBar);

		auto logPanel = (Border ^)contents->FindName("LogPanel");
		auto logBox = (Border ^)LoadContentsFromResource(IDR_LOG_LIST_BOX);
		logPanel->Child = logBox;
		m_LogBox = (System::Windows::Controls::ListBox^)logBox->Child;
		System::Windows::Controls::VirtualizingStackPanel::SetIsVirtualizing(m_LogBox, true);
		System::Windows::Controls::VirtualizingStackPanel::SetVirtualizationMode(m_LogBox, System::Windows::Controls::VirtualizationMode::Recycling);
		//m_LogBox->SetValue(System::Windows::Controls::ScrollViewer::CanContentScrollProperty, true);
		//m_LogBox->SetValue(System::Windows::Controls::ScrollViewer::HorizontalScrollBarVisibilityProperty, System::Windows::Controls::ScrollBarVisibility::Disabled);
		//m_LogBox->SetValue(System::Windows::Controls::ScrollViewer::IsDeferredScrollingEnabledProperty, true);
		//m_LogBox->SetValue(System::Windows::Controls::ScrollViewer::PanningModeProperty, System::Windows::Controls::PanningMode::VerticalOnly);
		//m_LogBox->SetValue(System::Windows::Controls::ScrollViewer::VerticalScrollBarVisibilityProperty, System::Windows::Controls::ScrollBarVisibility::Visible);
		//auto vp = gcnew System::Windows::Controls::VirtualizingStackPanel();
		//vp->SetIsVirtualizing
		//m_LogBox->SetValue(System::Windows::Controls::ItemsControl::ItemsPanelProperty,);
		//m_LogBox->SetValue(System::Windows::Controls::VirtualizingPanel::VirtualizationModeProperty, System::Windows::Controls::VirtualizationMode::Recycling);

		//アイテムリストのデータ構造
		{
			auto datatemp = gcnew System::Windows::DataTemplate();

			//追加するアイテムのコントロール
			auto templateElement = gcnew System::Windows::FrameworkElementFactory(Border::typeid);

			auto templateElementtext = gcnew System::Windows::FrameworkElementFactory(TextBlock::typeid);
			//templateElementtext->SetValue(TextBlock::HorizontalAlignmentProperty, System::Windows::HorizontalAlignment::Stretch);
			templateElementtext->SetValue(TextBlock::TextWrappingProperty, System::Windows::TextWrapping::Wrap);
			templateElementtext->SetBinding(TextBlock::TextProperty, gcnew System::Windows::Data::Binding());

			templateElement->AppendChild(templateElementtext);
			//templateElement->SetValue(Border::HorizontalAlignmentProperty, System::Windows::HorizontalAlignment::Stretch);
			templateElement->SetValue(Border::BorderThicknessProperty, gcnew System::Windows::Thickness(0, 0, 0, 1));
			templateElement->SetValue(Border::BorderBrushProperty, gcnew SolidColorBrush(Color::FromRgb(64, 64, 64)));

			templateElement->AddHandler(Border::MouseDownEvent, gcnew System::Windows::Input::MouseButtonEventHandler(this, &View::Log_OnMouseClick));

			datatemp->VisualTree = templateElement;

			m_LogBox->ItemTemplate = datatemp;

		}


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


	void Window_Closing(Object^ sender, System::ComponentModel::CancelEventArgs^ e)
	{
		//bool noSave = false;
		Data::_SendMessage(WM_DESTROY);
		for (;;){
			if (Data::GetHWND() == NULL){
				break;
			}
			Sleep(1);
		}
		// If data is dirty, notify user and ask for a response
		//if (this->isDataDirty)
		//{
		//	string msg = "Data is dirty. Close without saving?";
		//	MessageBoxResult result =
		//		MessageBox.Show(
		//		msg,
		//		"Data App",
		//		MessageBoxButton.YesNo,
		//		MessageBoxImage.Warning);
		//	if (result == MessageBoxResult.No)
		//	{
		//		// If user doesn't want to close, cancel closure
		//		e.Cancel = true;
		//	}
		//}
	}

	int count = 0;
	DWORD time = 0;
	void AddLog(String^ log){

		count++;
		if (m_LogBox->Items->Count > 500) {
			m_LogBox->Items->RemoveAt(0);
		}
		m_LogBox->Items->Add(count.ToString() + ":" + log);

		auto t = GetTickCount();
		if (time + 500 < t) {
			time = t;
			m_LogBox->ScrollIntoView(m_LogBox->Items[m_LogBox->Items->Count - 1]);
		}
	}

	//ツリービュー共通ドラッグ開始処理
	void Log_OnMouseClick(Object ^sender, System::Windows::Input::MouseButtonEventArgs ^e)
	{
		//if (e->ClickCount == 2) {
			auto item = (Border^)sender;
			auto data = (String^)item->DataContext;

			auto s = gcnew array<String^>(1);
			s[0] = ":";
			auto datas = data->Split(s,System::StringSplitOptions::None);
			int i = datas->Length;
			if (i < 3) return;

			//auto file = gcnew System::IO::FileInfo(datas[1]);
			//
			//if (!file->Exists)return;
			datas[2] = datas[2]->Replace("line(", "");
			datas[2] = datas[2]->Replace(")", "");

			datas[1] = "ScriptComponent/Scripts/" + datas[1];
			int l = 0;
			if (int::TryParse(datas[2], l)) {

				Data::MyPostMessage(MyWindowMessage::StackIntPtr, (void*)l);
				Data::MyPostMessage(MyWindowMessage::OpenAsset, (void*)string_cast(datas[1]));
			}
		//}

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
			{
				auto d = dynamic_cast<Panel^>(e->Header);
				if (d!= nullptr){
					ForDockPanelChild(d);
				}
			}
			if (e == nullptr)continue;
			auto d = dynamic_cast<Panel^>(e->Content);
			if (d == nullptr)continue;
			ForDockPanelChild(d);
		}
	}

	//表示しているコンポーネントを全て非表示（削除）にする
	void ClearAllComponent(){
		if (m_ComponentPanel == nullptr)return;
		ColorPickerWindow::WindowClose();
		m_ComponentPanel->Children->Clear();
	}

	//コンポーネントの作成
	void CreateComponent(Object^ head, IntPtr comptr, array<InspectorData^>^ data){
		if (m_ComponentPanel == nullptr)return;
		try{
			FrameworkElement ^com = LoadContentsFromResource(IDR_COMPONENT);

			m_ComponentPanel->Children->Add(com);

			auto header = (Expander^)com->FindName("ComponentHeaderName");
			header->Header = head;
			auto dock = (DockPanel^)com->FindName("MainDock");
			int num = data->GetLength(0);
			for (int i = 0; i < num; i++){
				data[i]->CreateInspector(dock);
			}

			//if (comptr != (IntPtr)NULL)
			{
				auto cm = gcnew System::Windows::Controls::ContextMenu();
				{//コンテキストメニューの要素作成
					auto mitem = gcnew System::Windows::Controls::MenuItem();
					mitem->Header = "Remove";
					mitem->Tag = comptr;
					mitem->Click += gcnew System::Windows::RoutedEventHandler(this, &View::MenuItem_Click_RemoveComponent);
					if (comptr == (IntPtr)NULL){
						mitem->IsEnabled = false;
					}
					cm->Items->Add(mitem);
				}
				com->ContextMenu = cm;
			}
		}
		catch (...){

		}

	}

	//コンポーネントの作成
	void CreateComponent(InspectorData^ head, IntPtr comptr, array<InspectorData^>^ data){
		if (m_ComponentPanel == nullptr)return;
		try{
			FrameworkElement ^com = LoadContentsFromResource(IDR_COMPONENT);

			m_ComponentPanel->Children->Add(com);

			auto header = (Expander^)com->FindName("ComponentHeaderName");

			DockPanel^ panel = gcnew DockPanel();
			panel->VerticalAlignment = System::Windows::VerticalAlignment::Top;
			head->CreateInspector(panel);

			auto child = (DockPanel^)panel->Children[0];
			auto tb = (TextBlock^)child->FindName("FloatName");
			if (tb)
				tb->Foreground = System::Windows::Media::Brushes::Gray;

			header->Header = panel;
			auto dock = (DockPanel^)com->FindName("MainDock");
			int num = data->GetLength(0);
			for (int i = 0; i < num; i++){
				data[i]->CreateInspector(dock);
			}

			//if (comptr != (IntPtr)NULL)
			{
				auto cm = gcnew System::Windows::Controls::ContextMenu();
				{//コンテキストメニューの要素作成
					auto mitem = gcnew System::Windows::Controls::MenuItem();
					mitem->Header = "Remove";
					mitem->Tag = comptr;
					mitem->Click += gcnew System::Windows::RoutedEventHandler(this, &View::MenuItem_Click_RemoveComponent);
					if (comptr == (IntPtr)NULL){
						mitem->IsEnabled = false;
					}
					cm->Items->Add(mitem);
				}
				com->ContextMenu = cm;
			}
		}
		catch (...){

		}

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

	//ドラッグ中の判定
	void Commponent_ViewModel_OnDragOver_GameObject(Object ^sender, DragEventArgs ^e)
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
	void Commponent_ViewModel_OnDrop_GameObject(Object ^s, DragEventArgs ^e)
	{

		auto dragtreeitem = (TreeViewItem^)e->Data->GetData(TreeViewItem::typeid);
		auto dragitem = dynamic_cast<TestContent::Person^>(dragtreeitem->DataContext);
		//アクターをD&Dしているか
		if (dragitem->DataPtr != (IntPtr)NULL) {
			Commponent_ViewModel_GameObjectDrop(s, dragitem);
		}
		else {
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

	//ゲームオブジェクトをドロップした処理
	void Commponent_ViewModel_GameObjectDrop(Object ^s, TestContent::Person^ dragitem) {

		auto targettextbox = dynamic_cast<TextBox^>(s);
		//TextBoxに対してDropしたか
		if (targettextbox) {
			auto viewmodel = (ViewModel<wp<IActor>>^)targettextbox->DataContext;
			viewmodel->Value = System::Convert::ToString(dragitem->DataPtr);
		}
	}

public:
	//コンテキストメニューの要素作成
	void CreateContextMenu_AddComponent(String^ componentname){
		CreateContextMenu_AddComponent(componentname, m_ComponentPanel->ContextMenu);
	}
	//コンテキストメニューの要素作成
	void CreateContextMenu_CreateObject(String^ objectname, String^ path){
		CreateContextMenu_CreateObject(objectname, path, m_SceneContextMenu);
	}
private:
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
		Data::MyPostMessage(MyWindowMessage::AddComponent, (void*)str);
	}

	//コンテキストメニューのオブジェクト追加の追加
	void CreateContextMenu_CreateObject(String^ text, String^ path, System::Windows::Controls::ContextMenu^ cm){
		auto mitem = gcnew System::Windows::Controls::MenuItem();
		mitem->Header = text;
		mitem->DataContext = path;
		mitem->Click += gcnew System::Windows::RoutedEventHandler(this, &View::MenuItem_Click_CreateObject);
		cm->Items->Add(mitem);
	}
	//コンテキストメニューのオブジェクト追加処理
	void MenuItem_Click_CreateObject(Object ^sender, System::Windows::RoutedEventArgs ^e){
		auto m = (MenuItem^)sender;
		auto s = (String^)m->DataContext;
		auto str = string_cast(s);
		PostMessageCreateObject(str);
		e->Handled = true;
	}
	//コンテキストメニューのオブジェクト追加のコールバックを送信
	void PostMessageCreateObject(std::string* str){
		Data::MyPostMessage(MyWindowMessage::CreatePrefabToActor, (void*)str);
	}

	//コンポーネントウィンドウ
#pragma endregion 

	public:
		//ツリービューにゲームオブジェクトを追加
		void AddItem(String ^Name, IntPtr ptr){
			mSceneTreeView->AddItem(Name, ptr);
		}

		//ツリービューで親子関係の登録
		void SetParent(IntPtr parent, IntPtr child){
			mSceneTreeView->SetParent(parent, child);
		}

		//ツリービューアイテムの名前変更
		void ChangeTreeViewName(String^ name, IntPtr treeviewptr){
			mSceneTreeView->ChangeTreeViewName(name, treeviewptr);
		}
		//ツリービューアイテムの削除
		void ClearTreeViewItem(IntPtr treeviewptr){
			mSceneTreeView->ClearTreeViewItem(treeviewptr);
		}
		//ツリービューアイテムの選択
		void SelectTreeViewItem(IntPtr treeviewptr){
			mSceneTreeView->SelectTreeViewItem(treeviewptr);
		}

		void AddEngineItem(String ^Name, IntPtr ptr){
			mEngineTreeView->AddItem(Name, ptr);
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

		//HWNDを取得
		auto source = gcnew System::Windows::Interop::WindowInteropHelper(this);
		mWindowHWND = reinterpret_cast<HWND>(source->Handle.ToPointer());
		

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
	property HWND WindowHWND{
		HWND get(){
			return mWindowHWND;
		}
	}
private:

	System::Windows::Controls::ListBox^ m_LogBox;

	GameScreen ^m_GameScreen;
	HWND mGameScreenHWND;
	HWND mWindowHWND;

	StackPanel ^m_ComponentPanel;
	System::Windows::Controls::ContextMenu ^m_SceneContextMenu;

	SceneTreeView ^mSceneTreeView;
	SceneTreeView ^mEngineTreeView;
	AssetTreeView ^mAssetTreeView;

};

