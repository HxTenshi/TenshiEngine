#pragma once

class System{
public:
	virtual ~System(){}
	//カーソルをウィンドの中心に固定
	virtual void LockCursorPositionToWindowCenter(bool flag) = 0;
	//カーソルのロック位置
	virtual XMVECTOR GetLockCursorPosition() = 0;
	//カーソルの表示
	virtual void ShowCursor(bool flag) = 0;
	//カーソルを画面内に閉じ込める
	virtual void ClipCursor(bool flag) = 0;
};

struct Cursor{
	bool mLockCursorPos;
	XMVECTOR mLockPos;
	bool mShowCursor;
	bool mClipCursor;
};

class SystemHelper : public System{
public:
	SystemHelper();
	~SystemHelper(){}
	//カーソルをウィンドの中心に固定
	void LockCursorPositionToWindowCenter(bool flag) override;
	//カーソルのロック位置
	XMVECTOR GetLockCursorPosition() override;
	//カーソルの表示
	void ShowCursor(bool flag) override;
	//カーソルを画面内に閉じ込める
	void ClipCursor(bool flag) override;

	void Initialize();
	void Update();

private:
	Cursor mCursor;
};