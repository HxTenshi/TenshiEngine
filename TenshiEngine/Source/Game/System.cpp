#include "System.h"
#include "Window/Window.h"

SystemHelper::SystemHelper(){
	mCursor.mLockCursorPos = false;
	mCursor.mLockPos = XMVectorZero();
	mCursor.mShowCursor = true;
	mCursor.mClipCursor = false;
}

//カーソルをウィンドの中心に固定
void SystemHelper::LockCursorPositionToWindowCenter(bool flag){
	mCursor.mLockCursorPos = flag;
}

XMVECTOR SystemHelper::GetLockCursorPosition(){
	return mCursor.mLockPos;
}
//カーソルの表示
void SystemHelper::ShowCursor(bool flag){
	mCursor.mShowCursor = flag;
	::ShowCursor(mCursor.mShowCursor);
}
//カーソルを画面内に閉じ込める
void SystemHelper::ClipCursor(bool flag){

	mCursor.mClipCursor = flag;
	if (mCursor.mClipCursor){
		RECT lprc;
		if (GetClientRect(Window::GetGameScreenHWND(), &lprc)){
			POINT p;
			p.x = 0;
			p.y = 0;
			if (ClientToScreen(Window::GetGameScreenHWND(), &p)){
				lprc.left = p.x;
				lprc.top = p.y;
				lprc.right = lprc.left + lprc.right;
				lprc.bottom = lprc.top + lprc.bottom;
				::ClipCursor(&lprc);
			}

		}
	}
	else{
		::ClipCursor(NULL);
	}
}


void SystemHelper::Initialize(){
	mCursor.mLockCursorPos = false;
	mCursor.mLockPos = XMVectorZero();
	mCursor.mShowCursor = true;
	mCursor.mClipCursor = false;
	
	::ShowCursor(TRUE);
	::ClipCursor(NULL);
}
void SystemHelper::Update(){

	if (mCursor.mLockCursorPos && Window::IsActive()){
		RECT lprc;
		if (GetClientRect(Window::GetGameScreenHWND(), &lprc)){
			POINT p;
			p.x = 0;
			p.y = 0;
			if (ClientToScreen(Window::GetGameScreenHWND(), &p)){
				auto x = (int)(p.x + lprc.right / 2);
				auto y = (int)(p.y + lprc.bottom / 2);
				::SetCursorPos(x, y);

				mCursor.mLockPos.x = (float)WindowState::mWidth/2;
				mCursor.mLockPos.y = (float)WindowState::mHeight/2;
			}

		}

	}

}