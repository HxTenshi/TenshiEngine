//#ifndef __unknownbase_h__
//#define __unknownbase_h__
//
////仮想関数テーブルを初期化しないようにする宣言
//#ifndef AM_NOVTABLE
//
//#ifdef  _MSC_VER
//#if _MSC_VER >= 1100
//#define AM_NOVTABLE __declspec(novtable)
//#else
//#define AM_NOVTABLE
//#endif
//#endif //MSC_VER
//
//#endif //AM_NOVTABLE
//
//#ifndef INONDELEGATINGUNKNOWN_DEFINED
//DECLARE_INTERFACE(INonDelegatingUnknown)
//{
//	STDMETHOD(NonDelegatingQueryInterface) (THIS_ REFIID, LPVOID *) PURE;
//	STDMETHOD_(ULONG, NonDelegatingAddRef)(THIS)PURE;
//	STDMETHOD_(ULONG, NonDelegatingRelease)(THIS)PURE;
//};
//#define INONDELEGATINGUNKNOWN_DEFINED
//#endif
//
////IUnknownの機能を実装する
//class AM_NOVTABLE CUnknownBase : public INonDelegatingUnknown
//{
//public:
//	CUnknownBase(IUnknown *lpUnknown); //コンストラクタ
//	virtual ~CUnknownBase(); //デストラクタ
//
//	STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void **ppv); //インターフェイスを要求する
//	STDMETHODIMP_(ULONG) NonDelegatingAddRef(void); //参照カウントの増加
//	STDMETHODIMP_(ULONG) NonDelegatingRelease(void); //参照カウントの減少
//
//	//オーナーオブジェクトを取得する
//	IUnknown *GetOwner(void) const { return const_cast<IUnknown *>(m_lpUnknown); }
//
//protected:
//	volatile long m_lRefCount; //参照カウント
//
//private:
//	//オブジェクトのコピーの禁止
//	CUnknownBase(const CUnknownBase &unk); //コピーコンストラクタ
//	CUnknownBase & operator = (const CUnknownBase &unk); //コピーオペレータ
//
//private:
//	const IUnknown *m_lpUnknown; //IUnknownインターフェイス
//};
//
////インターフェイスの変換時に使用する補助関数
//STDAPI GetInterface(IUnknown *lpUnknown, void **ppv);
//STDAPI GetInterfaceEx(INonDelegatingUnknown *lpUnknown, void **ppv);
//
////デバッグチェックマクロ
//#define CheckPointer(p,ret)			{ if((p) == NULL){ return (ret); } }
//#define ValidateReadPtr(p,cb)		0
//#define ValidateWritePtr(p,cb)		0
//#define ValidateReadWritePtr(p,cb)	0
//
////IUnknownの機能を実装するときに行うマクロ
//#define DECLARE_IUNKNOWN                                        \
//    STDMETHODIMP QueryInterface(REFIID riid, __deref_out void **ppv) {      \
//        return GetOwner()->QueryInterface(riid,ppv);            \
//    };                                                          \
//    STDMETHODIMP_(ULONG) AddRef() {                             \
//        return GetOwner()->AddRef();                            \
//    };                                                          \
//    STDMETHODIMP_(ULONG) Release() {                            \
//        return GetOwner()->Release();                           \
//    };
//
//
//#endif //__unknownbase_h__