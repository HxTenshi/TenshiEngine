#pragma once

class Inspector;
class I_ioHelper;
class Enabled{
public:
	Enabled()
		: m_IsEnabled(true)
		, m_IsParentEnabled(true)
	{}

	virtual ~Enabled(){}

	//有効化
	void Enable(){
		_TryChange(true, m_IsParentEnabled);
	}
	//無効化
	void Disable(){
		_TryChange(false, m_IsParentEnabled);
	}
	//有効か？
	bool IsEnabled(){
		return m_IsEnabled && m_IsParentEnabled;
	}
	//有効か？
	bool IsLocalEnabled(){
		return m_IsEnabled;
	}

protected:
	//コールバック用
	void ChildEnableChanged(Enabled* child){
		bool current = m_IsEnabled && m_IsParentEnabled;
		child->_TryChange(child->IsLocalEnabled(), current);
	}

	void IO_Data(I_ioHelper* io);
private:
	virtual void OnEnabled(){}
	virtual void OnDisabled(){}

	void _TryChange(bool enable,bool parent_enable){
		//変更の確認
		if (m_IsEnabled == enable && m_IsParentEnabled == parent_enable)return;
		//コールバックの状況確認用
		bool current = m_IsEnabled && m_IsParentEnabled;
		bool next = enable && parent_enable;
		//パラメーターのセット
		m_IsEnabled = enable;
		m_IsParentEnabled = parent_enable;
		//コールバックを呼ぶ必要がなければ
		if (current == next)return;
		//コールバック
		next?
			OnEnabled():
			OnDisabled();
	}

	bool m_IsEnabled;
	bool m_IsParentEnabled;

	friend Inspector;
};