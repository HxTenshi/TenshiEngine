//
//#include "hwndMovie.h"
//
////コンストラクタ
//CMFSession::CMFSession(HWND hWnd)
//	: CUnknownBase(NULL), m_hWnd(hWnd), m_nStatusCode(MFSSTATUS_NONE), m_mtMediaLength(0), m_fAudioVolume(1.0f), m_fAudioBalance(0.0f),
//	m_lpMediaSession(NULL), m_lpByteStream(NULL), m_lpMediaSource(NULL), m_lpPresentationClock(NULL), m_lpVideoDisplay(NULL), m_lpAudioVolume(NULL)
//{
//	NonDelegatingAddRef();
//}
//
////デストラクタ
//CMFSession::~CMFSession()
//{
//	ReleaseMovie();
//}
//
////インターフェイスの要求
//STDMETHODIMP CMFSession::NonDelegatingQueryInterface(REFIID riid, void **ppv)
//{
//	if (IsEqualIID(riid, IID_IMFAsyncCallback)) return GetInterface(static_cast<IMFAsyncCallback *>(this), ppv);
//	else return CUnknownBase::NonDelegatingQueryInterface(riid, ppv);
//}
//
////メディアを読み込む
//HRESULT CMFSession::LoadMovie(const TCHAR *lpFileName)
//{
//	IMFByteStream *lpByteStream = NULL; IMFSourceResolver *lpSourceResolver = NULL; IUnknown *lpSource = NULL;
//	IMFTopology *lpTopology = NULL; IMFPresentationDescriptor *lpPresentDesc = NULL; IMFClock *lpClock = NULL;
//
//#ifdef _UNICODE
//	const WCHAR *wszFileName;
//#else
//	WCHAR wszFileName[4096];
//#endif
//
//	MF_OBJECT_TYPE objtype; DWORD count; HRESULT hr;
//
//	//すでに読み込まれているときは解放を行う
//	if (InnerTestStatus(MFSSTATUS_LOAD)){ ReleaseMovie(); }
//
//
//	do{
//		//各オブジェクトの初期化
//		m_nStatusCode = MFSSTATUS_NONE;
//
//		//ファイル名をTCHARからWCHARへと変換する
//#ifdef _UNICODE
//		wszFileName = lpFileName;
//#else
//		MultiByteToWideChar(CP_ACP, 0, lpFileName, -1, wszFileName, sizeof(wszFileName) / sizeof(WCHAR));
//#endif
//
//		//MediaSessionの作成
//		FAILED_BREAK(hr, MFCreateMediaSession(NULL, &m_lpMediaSession));
//
//		//イベント取得を自分自身に割り当て
//		FAILED_BREAK(hr, m_lpMediaSession->BeginGetEvent(static_cast<IMFAsyncCallback *>(this), NULL));
//
//		//ByteStreamの作成
//		FAILED_BREAK(hr, MFCreateFile(MF_ACCESSMODE_READ, MF_OPENMODE_FAIL_IF_NOT_EXIST, MF_FILEFLAGS_NONE, wszFileName, &m_lpByteStream));
//		FAILED_BREAK(hr, SetByteStreamContentType(lpFileName, NULL));
//
//		//SourceResolverの作成
//		FAILED_BREAK(hr, MFCreateSourceResolver(&lpSourceResolver));
//
//		//IMFMediaSourceの作成
//		objtype = MF_OBJECT_INVALID;
//		FAILED_BREAK(hr, lpSourceResolver->CreateObjectFromByteStream(m_lpByteStream, NULL, MF_RESOLUTION_MEDIASOURCE, NULL, &objtype, &lpSource));
//		FAILED_BREAK(hr, lpSource->QueryInterface(IID_PPV_ARGS(&m_lpMediaSource)));
//
//		//IMFTopologyの作成
//		FAILED_BREAK(hr, MFCreateTopology(&lpTopology));
//
//		//MediaSourceからPresentationDescriptionを取得する
//		FAILED_BREAK(hr, m_lpMediaSource->CreatePresentationDescriptor(&lpPresentDesc));
//
//		//Descriptionの数を取得
//		FAILED_BREAK(hr, lpPresentDesc->GetStreamDescriptorCount(&count));
//
//		//取得された各Streamに対してトポロジーの状態を割り当てる
//		for (DWORD i = 0; i < count; i++){
//			IMFStreamDescriptor *lpStreamDesc = NULL; IMFTopologyNode *lpSourceNode = NULL, *lpOutputNode = NULL; BOOL bIsSelected = FALSE;
//
//			do{
//				//StreamDescriptionを取得
//				FAILED_BREAK(hr, lpPresentDesc->GetStreamDescriptorByIndex(i, &bIsSelected, &lpStreamDesc));
//
//				//対象のストリームが使われないときは作業を行わない
//				if (!bIsSelected) break;
//
//				//対象のストリームに対するNodeを作成
//				FAILED_BREAK(hr, CreateSourceNode(lpPresentDesc, lpStreamDesc, &lpSourceNode));
//				FAILED_BREAK(hr, CreateOutputNode(lpStreamDesc, &lpOutputNode));
//
//				//各ノードをトポロジーに登録
//				FAILED_BREAK(hr, lpTopology->AddNode(lpSourceNode));
//				FAILED_BREAK(hr, lpTopology->AddNode(lpOutputNode));
//
//				//ノードを接続
//				FAILED_BREAK(hr, lpSourceNode->ConnectOutput(0, lpOutputNode, 0));
//
//				//対象のストリームの処理完了
//				hr = S_OK;
//			} while (0);
//
//			if (lpOutputNode != NULL){ lpOutputNode->Release(); }
//			if (lpSourceNode != NULL){ lpSourceNode->Release(); }
//			if (lpStreamDesc != NULL){ lpStreamDesc->Release(); }
//			if (FAILED(hr)) break;
//		}
//
//		//トポロジーの処理に失敗したときは失敗する
//		if (FAILED(hr)) break;
//
//		//トポロジーをセッションに設定
//		FAILED_BREAK(hr, m_lpMediaSession->SetTopology(0, lpTopology));
//
//		//クロックを取得
//		FAILED_BREAK(hr, m_lpMediaSession->GetClock(&lpClock));
//		FAILED_BREAK(hr, lpClock->QueryInterface(IID_PPV_ARGS(&m_lpPresentationClock)));
//
//		//メディア長を取得
//		FAILED_BREAK(hr, lpPresentDesc->GetUINT64(MF_PD_DURATION, (UINT64*)&m_mtMediaLength));
//
//		//セッション作成完了
//		m_fAudioVolume = 1.0f; m_fAudioBalance = 0.0f; m_nStatusCode |= MFSSTATUS_LOAD; hr = S_OK;
//	} while (0);
//
//	//使用したインターフェイスを解放
//	if (lpClock != NULL){ lpClock->Release(); }
//	if (lpPresentDesc != NULL){ lpPresentDesc->Release(); }
//	if (lpTopology != NULL){ lpTopology->Release(); }
//	if (lpSource != NULL){ lpSource->Release(); }
//	if (lpSourceResolver != NULL){ lpSourceResolver->Release(); }
//	if (lpByteStream != NULL){ lpByteStream->Release(); }
//
//	return hr;
//}
//
////メディアを解放する
//BOOL CMFSession::ReleaseMovie(void)
//{
//	HRESULT hr;
//
//	if (m_lpMediaSession != NULL){
//		if (m_lpVideoDisplay != NULL){
//			m_lpVideoDisplay->Release();
//			m_lpVideoDisplay = NULL;
//		}
//		if (m_lpAudioVolume != NULL){
//			m_lpAudioVolume->Release();
//			m_lpAudioVolume = NULL;
//		}
//
//		hr = m_lpMediaSession->Close();
//		if (FAILED(hr)) return FALSE;
//	}
//
//	if (m_lpMediaSource != NULL){ m_lpMediaSource->Shutdown(); }
//	if (m_lpMediaSession != NULL){ m_lpMediaSession->Shutdown(); }
//
//	if (m_lpPresentationClock != NULL){
//		m_lpPresentationClock->Release();
//		m_lpPresentationClock = NULL;
//	}
//	if (m_lpMediaSource != NULL){
//		m_lpMediaSource->Release();
//		m_lpMediaSource = NULL;
//	}
//	if (m_lpByteStream != NULL){
//		m_lpByteStream->Release();
//		m_lpByteStream = NULL;
//	}
//	if (m_lpMediaSession != NULL){
//		m_lpMediaSession->Release();
//		m_lpMediaSession = NULL;
//	}
//
//	m_nStatusCode = MFSSTATUS_NONE;
//	return TRUE;
//}
//
////再生
//BOOL CMFSession::PlayMovie(BOOL bIsLoop)
//{
//	if (!InnerTestStatus(MFSSTATUS_LOAD)) return FALSE;
//	if (!InnerTestStatus(MFSSTATUS_PAUSE)){
//		PROPVARIANT varStart;
//		PropVariantInit(&varStart);
//		m_lpMediaSession->Start(NULL, &varStart);
//		if (bIsLoop) m_nStatusCode |= MFSSTATUS_LOOP;
//		m_nStatusCode = (m_nStatusCode & ~MFSSTATUS_PAUSE) | MFSSTATUS_PLAY;
//	}
//	return TRUE;
//}
//
////停止
//BOOL CMFSession::StopMovie(void)
//{
//	if (!InnerTestStatus(MFSSTATUS_LOAD)) return FALSE;
//	if (InnerTestStatus(MFSSTATUS_PLAY)){
//		m_lpMediaSession->Stop();
//		m_nStatusCode &= ~(MFSSTATUS_PLAY | MFSSTATUS_PAUSE);
//	}
//	return TRUE;
//}
//
////再生中かどうか
//BOOL CMFSession::IsPlayMovie(void) const
//{
//	return InnerTestStatus(MFSSTATUS_PLAY);
//}
//
////一時停止
//BOOL CMFSession::PauseMovie(void)
//{
//	if (!InnerTestStatus(MFSSTATUS_LOAD)) return FALSE;
//	if (InnerTestStatus(MFSSTATUS_PAUSE)){
//		PROPVARIANT varStart;
//		PropVariantInit(&varStart);
//		m_lpMediaSession->Start(NULL, &varStart);
//		m_nStatusCode &= ~MFSSTATUS_PAUSE;
//	}
//	else if (InnerTestStatus(MFSSTATUS_PLAY)){
//		m_lpMediaSession->Pause();
//		m_nStatusCode |= MFSSTATUS_PAUSE;
//	}
//
//	return TRUE;
//}
//
////早送り(ms)
//BOOL CMFSession::ForwardMovie(uint32_t nTime)
//{
//	return SeekMovieRel((int64_t)MilliSecondToMediaTime(nTime));
//}
//
////巻き戻し(ms)
//BOOL CMFSession::BackwardMovie(uint32_t nTime)
//{
//	return SeekMovieRel(-(int64_t)MilliSecondToMediaTime(nTime));
//}
//
////再生位置の移動(ms)
//BOOL CMFSession::SeekMovie(uint32_t nTime)
//{
//	return SeekMovieAbs(MilliSecondToMediaTime(nTime));
//}
//
////再生位置の相対移動(100ns)
//BOOL CMFSession::SeekMovieRel(int64_t llTime)
//{
//	MFTIME vMFTime; HRESULT hr;
//
//	if (m_lpPresentationClock == NULL) return FALSE;
//	hr = m_lpPresentationClock->GetTime(&vMFTime);
//	if (FAILED(hr)) return FALSE;
//
//	return SUCCEEDED(InnerSeekTime(vMFTime + llTime));
//}
//
////再生位置の絶対移動(100ns)
//BOOL CMFSession::SeekMovieAbs(uint64_t ullTime)
//{
//	return SUCCEEDED(InnerSeekTime(ullTime));
//}
//
////再生ウィンドウの設定
//BOOL CMFSession::SetPlayWindow(const POINT *lpPlayPos, const SIZE *lpPlaySize)
//{
//	RECT rcRect; POINT ptPlay; SIZE sizePlay, sizeVideo;
//
//	if (!IsEnableVideo()) return FALSE;
//
//	if (m_lpVideoDisplay != NULL){
//		m_lpVideoDisplay->GetNativeVideoSize(&sizeVideo, NULL);
//
//		if (lpPlayPos != NULL){ ptPlay = *lpPlayPos; }
//		else{ ptPlay.x = ptPlay.y = 0; }
//		if (lpPlaySize != NULL){ sizePlay = *lpPlaySize; }
//		else{ sizePlay = sizeVideo; }
//
//		SetRect(&rcRect, ptPlay.x, ptPlay.y, ptPlay.x + sizePlay.cx, ptPlay.y + sizePlay.cy);
//		m_lpVideoDisplay->SetVideoPosition(NULL, &rcRect);
//	}
//	return TRUE;
//}
//
////スピードの変更
//BOOL CMFSession::SetSpeed(double dSpeed)
//{
//	IMFGetService *lpGetService = NULL; IMFRateSupport *lpRateSupport = NULL; IMFRateControl *lpRateControl = NULL; float fSpeed, fNearSpeed; HRESULT hr;
//
//	do{
//		fSpeed = (float)dSpeed;
//		FAILED_BREAK(hr, m_lpMediaSession->QueryInterface(IID_PPV_ARGS(&lpGetService)));
//		FAILED_BREAK(hr, lpGetService->GetService(MF_RATE_CONTROL_SERVICE, IID_PPV_ARGS(&lpRateSupport)));
//		FAILED_BREAK(hr, lpRateSupport->IsRateSupported(FALSE, fSpeed, &fNearSpeed));
//		FAILED_BREAK(hr, lpGetService->GetService(MF_RATE_CONTROL_SERVICE, IID_PPV_ARGS(&lpRateControl)));
//		FAILED_BREAK(hr, lpRateControl->SetRate(FALSE, fSpeed));
//	} while (0);
//
//	if (lpRateControl != NULL){ lpRateControl->Release(); }
//	if (lpRateSupport != NULL){ lpRateSupport->Release(); }
//	if (lpGetService != NULL){ lpGetService->Release(); }
//	return SUCCEEDED(hr);
//}
//
////パンの変化を設定する
//BOOL CMFSession::SetPan(int nAbsolutePan)
//{
//	float fBalance; HRESULT hr;
//
//	if (!IsEnableAudio()) return FALSE;
//	if (nAbsolutePan < MFS_PANLEFT || nAbsolutePan > MFS_PANRIGHT) return FALSE;
//
//	fBalance = (float)nAbsolutePan / (float)MFS_VOLUMEMAX;
//	if (fBalance == m_fAudioBalance) return TRUE;
//	m_fAudioBalance = fBalance;
//
//	hr = InnerSetVolume();
//	return SUCCEEDED(hr);
//}
//
////音量の設定をする
//BOOL CMFSession::SetVolume(int nAbsoluteVolume)
//{
//	float fVolume; HRESULT hr;
//
//	if (!IsEnableAudio()) return FALSE;
//	if (nAbsoluteVolume < MFS_VOLUMEMIN || nAbsoluteVolume > MFS_VOLUMEMAX) return FALSE;
//
//	fVolume = (float)(nAbsoluteVolume - MFS_VOLUMEMIN) / (float)(MFS_VOLUMEMAX - MFS_VOLUMEMIN);
//	if (fVolume == m_fAudioVolume) return TRUE;
//	m_fAudioVolume = fVolume;
//
//	hr = InnerSetVolume();
//	return SUCCEEDED(hr);
//}
//
//
////メディアの再生時間を取得
//uint32_t CMFSession::GetMovieTime(void) const
//{
//	return MediaTimeToMilliSecond(m_mtMediaLength);
//}
//
////メディア時間で再生時間を取得
//uint64_t CMFSession::GetMediaTime(void) const
//{
//	return m_mtMediaLength;
//}
//
////メディアの画像のサイズを取得
//BOOL CMFSession::GetMovieSize(int32_t *lpnWidth, int32_t *lpnHeight) const
//{
//	SIZE sizeVideo; HRESULT hr;
//
//	if (m_lpVideoDisplay == NULL) return FALSE;
//	if (lpnWidth == NULL || lpnHeight == NULL) return FALSE;
//
//	hr = m_lpVideoDisplay->GetNativeVideoSize(&sizeVideo, NULL);
//	if (FAILED(hr)) return FALSE;
//
//	*lpnWidth = sizeVideo.cx; *lpnHeight = sizeVideo.cy;
//	return TRUE;
//}
//
////メディアが映像ラインを保持しているかどうか
//BOOL CMFSession::HasMediaVideo(void) const
//{
//	return InnerTestStatus(MFSSTATUS_HASVIDEOLINE);
//}
//
////メディアが音声ラインを保持しているかどうか
//BOOL CMFSession::HasMediaAudio(void) const
//{
//	return InnerTestStatus(MFSSTATUS_HASAUDIOLINE);
//}
//
////ByteStreamにContentTypeを設定する
//HRESULT CMFSession::SetByteStreamContentType(const TCHAR *lpFileName, const TCHAR *lpContentType)
//{
//	IMFAttributes *lpAttributes = NULL; const MEDIACONTENTTYPE *lpMediaType; const TCHAR *lp, *lpLastPath, *lpLastExt; HRESULT hr;
//
//	do{
//		CheckPointer(m_lpByteStream, E_POINTER);
//		FAILED_BREAK(hr, m_lpByteStream->QueryInterface(IID_PPV_ARGS(&lpAttributes)));
//
//		if (lpContentType != NULL){
//#ifdef _UNICODE
//			lpAttributes->SetString(MF_BYTESTREAM_CONTENT_TYPE, lpContentType);
//#else
//			WCHAR wszContentType[64];
//			MultiByteToWideChar(CP_ACP, 0, lpContentType, -1, wszContentType, sizeof(wszContentType) / sizeof(WCHAR));
//			lpAttributes->SetString(MF_BYTESTREAM_CONTENT_TYPE, wszContentType);
//#endif //_UNICODE
//			hr = S_OK; break;
//		}
//
//		for (lp = lpLastPath = lpLastExt = lpFileName; *lp != TEXT('\0'); lp++){
//			if (_istlead(*lp) && _isttrail(*(lp + 1))){ lp++; }
//			else if (*lp == TEXT('\\') || *lp == TEXT('/')){ lpLastPath = lp; }
//			else if (*lp == TEXT('.')){ lpLastExt = lp; }
//		}
//		if (lpLastPath >= lpLastExt){ hr = S_FALSE; break; } lpLastExt++;
//
//		for (lpMediaType = g_avMediaType, hr = S_FALSE; lpMediaType->ext != NULL; lpMediaType++){
//			if (_tcsicmp(lpMediaType->ext, lpLastExt) == 0){
//				hr = lpAttributes->SetString(MF_BYTESTREAM_CONTENT_TYPE, lpMediaType->type);
//				break;
//			}
//		}
//	} while (0);
//
//	if (lpAttributes != NULL){ lpAttributes->Release(); }
//	return hr;
//}
//
////ソース部のトポロジーノードを作成する
//HRESULT CMFSession::CreateSourceNode(IMFPresentationDescriptor *lpPresentDesc, IMFStreamDescriptor *lpStreamDesc, IMFTopologyNode **lplpNode)
//{
//	IMFTopologyNode *lpNode = NULL; HRESULT hr;
//
//	do{
//		//SourceStream用のノードを作成
//		FAILED_BREAK(hr, MFCreateTopologyNode(MF_TOPOLOGY_SOURCESTREAM_NODE, &lpNode));
//
//		//必要な属性を設定する
//		FAILED_BREAK(hr, lpNode->SetUnknown(MF_TOPONODE_SOURCE, m_lpMediaSource));
//		FAILED_BREAK(hr, lpNode->SetUnknown(MF_TOPONODE_PRESENTATION_DESCRIPTOR, lpPresentDesc));
//		FAILED_BREAK(hr, lpNode->SetUnknown(MF_TOPONODE_STREAM_DESCRIPTOR, lpStreamDesc));
//
//		//作成が完了したので戻す
//		*lplpNode = lpNode; lpNode = NULL; hr = S_OK;
//	} while (0);
//
//	if (lpNode != NULL){ lpNode->Release(); }
//	return hr;
//}
//
////出力部のトポロジーノードを作成する
//HRESULT CMFSession::CreateOutputNode(IMFStreamDescriptor *lpStreamDesc, IMFTopologyNode **lplpNode)
//{
//	IMFTopologyNode *lpNode = NULL; IMFMediaTypeHandler *lpHandler = NULL; IMFActivate *lpActivate = NULL; GUID guidMajorType; DWORD dwStreamID; HRESULT hr;
//
//	do{
//		memset(&guidMajorType, 0x00, sizeof(guidMajorType));
//
//		//ストリームのIDを取得
//		lpStreamDesc->GetStreamIdentifier(&dwStreamID);
//
//		//ストリームのメデイア種別を取得
//		FAILED_BREAK(hr, lpStreamDesc->GetMediaTypeHandler(&lpHandler));
//		FAILED_BREAK(hr, lpHandler->GetMajorType(&guidMajorType));
//
//		//出力部のトポロジーノードを作成
//		FAILED_BREAK(hr, MFCreateTopologyNode(MF_TOPOLOGY_OUTPUT_NODE, &lpNode));
//
//		//レンダラを作成
//		if (IsEqualGUID(guidMajorType, MFMediaType_Audio)){
//			//オーディオレンダラを作成
//			FAILED_BREAK(hr, MFCreateAudioRendererActivate(&lpActivate));
//			m_nStatusCode |= MFSSTATUS_HASAUDIOLINE;
//		}
//		else if (IsEqualGUID(guidMajorType, MFMediaType_Video)){
//			//ビデオレンダラを作成
//			FAILED_BREAK(hr, MFCreateVideoRendererActivate(m_hWnd, &lpActivate));
//			m_nStatusCode |= MFSSTATUS_HASVIDEOLINE;
//		}
//
//		//出力できる種別でないときは失敗する
//		else{ hr = E_FAIL; break; }
//
//		//ノードに出力を設定する
//		FAILED_BREAK(hr, lpNode->SetObject(lpActivate));
//
//		//作成が完了したので戻す
//		*lplpNode = lpNode; lpNode = NULL; hr = S_OK;
//	} while (0);
//
//	if (lpActivate != NULL){ lpActivate->Release(); }
//	if (lpHandler != NULL){ lpHandler->Release(); }
//	if (lpNode != NULL){ lpNode->Release(); }
//
//	return S_OK;
//}
//
//
////再生位置を設定する
//HRESULT CMFSession::InnerSeekTime(MFTIME nTime)
//{
//	if (!InnerTestStatus(MFSSTATUS_LOAD)) return E_ABORT;
//
//	if (nTime < 0){ nTime = 0; }
//	else if ((uint64_t)nTime >= m_mtMediaLength){ nTime = m_mtMediaLength; }
//
//	PROPVARIANT varStart;
//	varStart.vt = VT_I8;
//	varStart.hVal.QuadPart = nTime;
//
//	m_lpMediaSession->Start(NULL, &varStart);
//	if (!InnerTestStatus(MFSSTATUS_PLAY)){ m_lpMediaSession->Stop(); }
//	else if (InnerTestStatus(MFSSTATUS_PAUSE)){ m_lpMediaSession->Pause(); }
//
//	return S_OK;
//}
//
////ボリュームを設定する
//HRESULT CMFSession::InnerSetVolume(void)
//{
//	float *lpfVolumes; uint32_t i, nChannels;
//
//	CheckPointer(m_lpAudioVolume, E_FAIL);
//
//	m_lpAudioVolume->GetChannelCount(&nChannels);
//	lpfVolumes = (float *)_malloca(sizeof(float) * nChannels);
//
//	for (i = 0; i < nChannels; i++){
//		if (i & 0x01){ lpfVolumes[i] = (m_fAudioBalance >= 0.0f ? 1.0f : (1.0f + m_fAudioBalance)) * m_fAudioVolume; }
//		else{ lpfVolumes[i] = (m_fAudioBalance <= 0.0f ? 1.0f : (1.0f - m_fAudioBalance)) * m_fAudioVolume; }
//	}
//
//	if (nChannels & 0x01){ lpfVolumes[nChannels - 1] = m_fAudioVolume; }
//	m_lpAudioVolume->SetAllVolumes(nChannels, lpfVolumes);
//	_freea(lpfVolumes);
//
//	return S_OK;
//}
//
//
////ビデオ有効テスト
//bool CMFSession::IsEnableVideo(void) const
//{
//	return InnerTestStatus(MFSSTATUS_LOAD | MFSSTATUS_HASVIDEOLINE);
//}
//
////サウンド有効テスト
//bool CMFSession::IsEnableAudio(void) const
//{
//	return InnerTestStatus(MFSSTATUS_LOAD | MFSSTATUS_HASAUDIOLINE);
//}
//
////内部的なステータステスト
//bool CMFSession::InnerTestStatus(uint32_t nStatus) const
//{
//	return (m_nStatusCode & nStatus) == nStatus;
//}
//
////ミリ秒をメディアタイムに変換する
//uint64_t CMFSession::MilliSecondToMediaTime(uint32_t t)
//{
//	return (uint64_t)t * (1000 * 10);
//}
//
////メディアタイムをミリ秒に変換する
//uint32_t CMFSession::MediaTimeToMilliSecond(uint64_t t)
//{
//	return (uint32_t)(t / (1000 * 10));
//}
//
//
////内部パラメータを取得する
//STDMETHODIMP CMFSession::GetParameters(DWORD *pdwFlags, DWORD *pdwQueue)
//{
//	return E_NOTIMPL;
//}
//
////非同期処理が行われたときのコールバック
//STDMETHODIMP CMFSession::Invoke(IMFAsyncResult *pAsyncResult)
//{
//	IMFMediaEvent *lpMediaEvent; MediaEventType type; HRESULT hr;
//
//	do{
//		lpMediaEvent = NULL;
//
//		//イベントキューからイベントを取得
//		FAILED_BREAK(hr, m_lpMediaSession->EndGetEvent(pAsyncResult, &lpMediaEvent));
//		FAILED_BREAK(hr, lpMediaEvent->GetType(&type));
//
//		//セッション終了でないときは再度イベント取得を有効にする
//		if (type != MESessionClosed){ m_lpMediaSession->BeginGetEvent(static_cast<IMFAsyncCallback *>(this), NULL); }
//
//		//セッションが有効であるときはイベントをウィンドウプロシージャから発行してもらう
//		if (m_lpMediaSession != NULL){
//			::PostMessage(m_hWnd, WM_MFSNOTIFY, (WPARAM)this, (LPARAM)lpMediaEvent);
//			lpMediaEvent = NULL;
//		}
//
//		hr = S_OK;
//	} while (0);
//
//	if (lpMediaEvent != NULL){ lpMediaEvent->Release(); }
//	return hr;
//}
//
//
////再描画を行う
//BOOL CMFSession::Repaint(void)
//{
//	if (m_lpVideoDisplay == NULL) return FALSE;
//	m_lpVideoDisplay->RepaintVideo();
//	return TRUE;
//}
//
////イベント処理発行
//BOOL CMFSession::HandleEvent(LPARAM lParam)
//{
//	IUnknown *lpUnknown; IMFMediaEvent *lpMediaEvent = NULL; MF_TOPOSTATUS topostatus; MediaEventType type; HRESULT hrStatus, hr;
//
//	if (lParam == NULL) return FALSE;
//
//	do{
//		lpUnknown = (IUnknown *)lParam;
//		FAILED_BREAK(hr, lpUnknown->QueryInterface(IID_PPV_ARGS(&lpMediaEvent)));
//		FAILED_BREAK(hr, lpMediaEvent->GetType(&type));
//		FAILED_BREAK(hr, lpMediaEvent->GetStatus(&hrStatus));
//		if (FAILED(hrStatus)){ hr = hrStatus; break; }
//
//		switch (type){
//		case MESessionTopologyStatus:
//			FAILED_BREAK(hr, lpMediaEvent->GetUINT32(MF_EVENT_TOPOLOGY_STATUS, (UINT32 *)&topostatus));
//			switch (topostatus){
//			case MF_TOPOSTATUS_READY:
//			{
//				IMFGetService *lpGetService = NULL;
//
//				do{
//					FAILED_BREAK(hr, m_lpMediaSession->QueryInterface(IID_PPV_ARGS(&lpGetService)));
//					if (InnerTestStatus(MFSSTATUS_HASVIDEOLINE)){ FAILED_BREAK(hr, lpGetService->GetService(MR_VIDEO_RENDER_SERVICE, IID_PPV_ARGS(&m_lpVideoDisplay))); }
//					if (InnerTestStatus(MFSSTATUS_HASAUDIOLINE)){ FAILED_BREAK(hr, lpGetService->GetService(MR_STREAM_VOLUME_SERVICE, IID_PPV_ARGS(&m_lpAudioVolume))); }
//				} while (0);
//				if (lpGetService != NULL){ lpGetService->Release(); }
//			} break;
//			} break;
//
//		case MEEndOfPresentation:
//			if (InnerTestStatus(MFSSTATUS_LOOP)){
//				PROPVARIANT varStart;
//				varStart.vt = VT_I8;
//				varStart.hVal.QuadPart = 0;
//				m_lpMediaSession->Start(NULL, &varStart);
//			}
//			else{ m_nStatusCode &= ~(MFSSTATUS_PLAY | MFSSTATUS_PAUSE); }
//			hr = S_OK;
//			break;
//
//		default: hr = S_OK;
//		}
//	} while (0);
//
//	if (lpMediaEvent != NULL){ lpMediaEvent->Release(); }
//	if (lpUnknown != NULL){ lpUnknown->Release(); }
//	return SUCCEEDED(hr);
//}
//
//
//
//
////LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
////
////int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nShowCmd)
////{
////	HWND hMainWnd; HRESULT hr;
////
////	CMFSession *g_lpSession = NULL;
////	MFStartup(MF_VERSION);
////	g_lpSession = new CMFSession(hMainWnd);
////	hr = g_lpSession->LoadMovie(TEXT("EngineResource/vi.wmv"));
////	hr = g_lpSession->PlayMovie(FALSE);
////	//g_lpSession->SetPlayWindow();
////	
////	g_lpSession->ReleaseMovie();
////	delete g_lpSession;
////	g_lpSession = NULL;
////	MFShutdown();
////}
//
////LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
////{
////	switch (uMsg){
////	case WM_MFSNOTIFY:
////		if (wParam != NULL){ ((CMFSession *)wParam)->HandleEvent(lParam); }
////		return 0;
////
////	case WM_PAINT:
////	{
////		PAINTSTRUCT ps; HDC hdc;
////		hdc = BeginPaint(hWnd, &ps);
////		・・・
////			EndPaint(hWnd, &ps);
////		if (g_lpSession != NULL){ g_lpSession->Repaint(); }
////	} break;
////
////	・・・
////	}
////	return DefWindowProc(hWnd, uMsg, wParam, lParam);
////}