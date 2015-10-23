#include "ProjectWindow.h"
#include "Window.h"

#include <CommCtrl.h>

#include <iostream>
#include <algorithm>
#include <filesystem>
#include <vector>

void folder(std::vector<std::string>& forlderList) {
	namespace sys = std::tr2::sys;
	sys::path p("."); // 列挙の起点
	//std::for_each(sys::directory_iterator(p), sys::directory_iterator(),
	//  再帰的に走査するならコチラ↓
	 std::for_each(sys::recursive_directory_iterator(p), sys::recursive_directory_iterator(),
		[&](const sys::path& p) {
		if (sys::is_regular_file(p)) { // ファイルなら...
			p.filename();
		}
		else if (sys::is_directory(p)) { // ディレクトリなら...
			forlderList.push_back(p.string());
		}
	});
}

void file(const std::string& pass,std::vector<std::string>& fileList) {
	namespace sys = std::tr2::sys;
	sys::path p = pass;
	std::for_each(sys::directory_iterator(p), sys::directory_iterator(),
	//  再帰的に走査するならコチラ↓
	//std::for_each(sys::recursive_directory_iterator(p), sys::recursive_directory_iterator(),
		[&](const sys::path& p) {
		if (sys::is_regular_file(p)) { // ファイルなら...
			fileList.push_back(p.filename());
		}
		else if (sys::is_directory(p)) { // ディレクトリなら...
		}
	});
}
void FolderTreeWindow::Create(Window* window){
	mWindow = window;

	RECT rc = { WindowState::mWidth, WindowState::mHeight/2, WindowState::mTreeViewWidth, WindowState::mHeight/2 };

	mhWnd = CreateWindowEx(0, WC_TREEVIEW, NULL,
		WS_CHILD | WS_BORDER | WS_VISIBLE | TVS_HASLINES | //WS_THICKFRAME |
		TVS_HASBUTTONS | TVS_LINESATROOT | TVS_SHOWSELALWAYS,
		rc.left, rc.top, rc.right, rc.bottom,
		mWindow->mhWnd, (HMENU)IDC::ForlderTree, mWindow->mhInstance, NULL);

	std::vector<std::string> ForlderList;
	folder(ForlderList);
	
	for (auto& dir : ForlderList){
		HTREEITEM hParent;
		TV_INSERTSTRUCT tv;

		tv.hInsertAfter = TVI_SORT;
		tv.item.mask = TVIF_TEXT | TVIF_PARAM;
		tv.hParent = TVI_ROOT;
		tv.item.lParam = NULL;
		char c[256];
		strcpy_s(c, dir.c_str());
		tv.item.pszText = (LPSTR)c;
		hParent = TreeView_InsertItem(mhWnd, &tv);
	}

	SetMyWndProc();
}

LRESULT CALLBACK FolderTreeWindow::MyWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam){

	switch (((LPNMTREEVIEW)lParam)->hdr.code)
	{
		case TVN_SELCHANGED:{
			HTREEITEM hItem = TreeView_GetSelection(mhWnd);
			if (hItem) {
				TVITEM tvItem;
				tvItem.mask = TVIF_HANDLE | TVIF_TEXT;
				tvItem.hItem = hItem;
				char szStr[256];
				tvItem.cchTextMax = 256;
				tvItem.pszText = szStr;
				if (TreeView_GetItem(mhWnd, &tvItem)){
					mFolderWindow->OpenFolder(szStr);
				}
			}
			break;
		}
		default:
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	return 0;
}

void FolderWindow::Create(Window* window){
	mWindow = window;

	RECT rc = { WindowState::mWidth + WindowState::mTreeViewWidth, WindowState::mHeight / 2, WindowState::mTreeViewWidth, WindowState::mHeight / 2 };

	mhWnd = CreateWindowEx(0, WC_LISTVIEW, 0,
		WS_CHILD | WS_VISIBLE | LVS_REPORT,
		rc.left, rc.top, rc.right, rc.bottom,
		mWindow->mhWnd, (HMENU)IDC::ForlderItem, mWindow->mhInstance, NULL);

	//typedef struct _LVCOLUMN {
	//	UINT mask;         // 有効メンバを示すフラグ
	//	int  fmt;          // 列の配置
	//	int  cx;           // カラム（項目）の幅
	//	LPTSTR pszText;    // カラムのヘッダの文字列
	//	int  cchTextMax;   // pszTextのサイズ
	//	int  iSubItem;     // カラムのサブアイテムインデックス
	//	int  iImage;       // イメージのインデックス
	//	int  iOrder;       // カラムのオフセット
	//} LVCOLUMN, FAR *LPLVCOLUMN;

	LVCOLUMN col;
	col.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	col.fmt = LVCFMT_LEFT;
	col.cx = 100;
	col.pszText = "test";

	ListView_InsertColumn(mhWnd, 0, &col);

	OpenFolder(".");
	mOpenFolderPass = ".";


	SetMyWndProc();
}

void FolderWindow::OpenFolder(const std::string& pass){

	ListView_DeleteAllItems(mhWnd);

	std::vector<std::string> FileList;
	file(pass,FileList);
	mOpenFolderPass = pass;

	int count = 0;
	for (auto& dir : FileList)
	{
		LVITEM item = { 0 };
		item.mask = LVIF_TEXT;
		char c[256];
		strcpy_s(c, dir.c_str());
		item.pszText = (LPSTR)c;
		item.iItem = count;
		ListView_InsertItem(mhWnd, &item);
		count++;
	}

}

#include "Game/Game.h"
LRESULT CALLBACK FolderWindow::MyWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam){

	switch (((LPNMLISTVIEW)lParam)->hdr.code)
	{

		// case LVN_ITEMCHANGED: // 項目を選択
	case NM_DBLCLK:{  // 項目をダブルクリック


		LV_HITTESTINFO lvinfo;
		TCHAR buf[256];
		LVITEM item;

		GetCursorPos((LPPOINT)&lvinfo.pt);
		ScreenToClient(((LPNMLISTVIEW)lParam)->hdr.hwndFrom, &lvinfo.pt);
		ListView_HitTest(((LPNMLISTVIEW)lParam)->hdr.hwndFrom, &lvinfo);
		if ((lvinfo.flags & LVHT_ONITEM) != 0)
		{
			item.mask = TVIF_HANDLE | TVIF_TEXT;
			item.iItem = lvinfo.iItem;
			item.iSubItem = 0;          //取得するサブアイテムの番号
			item.pszText = buf;         //格納するテキストバッファ
			item.cchTextMax = 256; //バッファ容量
			ListView_GetItem(((LPNMLISTVIEW)lParam)->hdr.hwndFrom, &item);
			Game::AddResource(mOpenFolderPass +"/"+ buf);
			
		}
		break;
	}
	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	return 0;
}
void ProjectWindow::Create(Window* window){
	
	mFolderTreeWindow.Create(window);
	mFolderWindow.Create(window);
}