//#include "unknownbase.h"
//
//#pragma warning(disable:4355)
//
////コンストラクタ
//CUnknownBase::CUnknownBase(IUnknown *lpUnknown)
//	: m_lRefCount(0), m_lpUnknown(lpUnknown != NULL ? lpUnknown : reinterpret_cast<IUnknown *>(static_cast<INonDelegatingUnknown *>(this)))
//{
//
//}
//
////デストラクタ
//CUnknownBase::~CUnknownBase()
//{
//
//}
//
////インターフェイスを要求する
//STDMETHODIMP CUnknownBase::NonDelegatingQueryInterface(REFIID riid, void **ppv)
//{
//	//ポインタチェック
//	CheckPointer(ppv, E_POINTER);
//	ValidateReadWritePtr(ppv, sizeof(PVOID));
//
//	//IUnknownのみインターフェイスの受け継ぎを行う
//	if (IsEqualIID(riid, IID_IUnknown)){
//		GetInterface(reinterpret_cast<IUnknown *>(static_cast<INonDelegatingUnknown *>(this)), ppv);
//		return S_OK;
//	}
//	else{
//		*ppv = NULL;
//		return E_NOINTERFACE;
//	}
//}
//
////参照カウント用の大きい方の値を返すテンプレート関数
//template <class T> inline static T ourmax(const T &a, const T &b)
//{
//	return a > b ? a : b;
//}
//
////参照カウントの増加
//STDMETHODIMP_(ULONG) CUnknownBase::NonDelegatingAddRef(void)
//{
//	long lRef = InterlockedIncrement(&m_lRefCount);
//	return ourmax((ULONG)lRef, 1ul);
//}
//
////参照カウントの減少
//STDMETHODIMP_(ULONG) CUnknownBase::NonDelegatingRelease(void)
//{
//	long lRef = InterlockedDecrement(&m_lRefCount);
//	if (lRef == 0){ m_lRefCount++; delete this; return (ULONG)0; }
//	else return ourmax((ULONG)lRef, 1ul);
//}
//
////インターフェイスの変換時に使用する補助関数
//STDAPI GetInterface(IUnknown *lpUnknown, void **ppv)
//{
//	CheckPointer(ppv, E_POINTER);
//	*ppv = lpUnknown; lpUnknown->AddRef();
//	return S_OK;
//}
//
//STDAPI GetInterfaceEx(INonDelegatingUnknown *lpUnknown, void **ppv)
//{
//	CheckPointer(ppv, E_POINTER);
//	*ppv = lpUnknown; lpUnknown->NonDelegatingAddRef();
//	return S_OK;
//}