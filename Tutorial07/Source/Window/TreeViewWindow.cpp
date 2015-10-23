

#include "TreeViewWindow.h"
#include "Window.h"

#include <CommCtrl.h>

void TreeViewWindow::Create(Window* window){
	mWindow = window;

	RECT rc = { WindowState::mWidth, 0, WindowState::mTreeViewWidth, WindowState::mHeight/2};

	mhWnd = CreateWindowEx(0, WC_TREEVIEW, NULL,
		WS_CHILD | WS_BORDER | WS_VISIBLE | TVS_HASLINES | //WS_THICKFRAME |
		TVS_HASBUTTONS | TVS_LINESATROOT | TVS_SHOWSELALWAYS,
		rc.left, rc.top, rc.right, rc.bottom,
		mWindow->mhWnd, (HMENU)1, mWindow->mhInstance, NULL);

	SetMyWndProc();
}

#include "Game/Actor.h"
void TreeViewWindow::AddItem(void* item){


	Window::AddTreeViewItem(((Actor*)item)->Name(), item);
	return;

	//WPARAM index = SendMessage(list, LB_ADDSTRING, 0, (LPARAM)entity._Get()->Edit_ListString());
	//SendMessage(list, LB_SETITEMDATA, index, (LPARAM)entity._Get());

	HTREEITEM hParent;
	TV_INSERTSTRUCT tv;

	tv.hInsertAfter = TVI_SORT;
	tv.item.mask = TVIF_TEXT | TVIF_PARAM;
	tv.hParent = TVI_ROOT;
	tv.item.lParam = (LPARAM)item;
	auto act = (Actor*)item;
	char c[64];
	strcpy_s(c, act->Name().c_str());
	tv.item.pszText = (LPSTR)c;
	hParent = TreeView_InsertItem(mhWnd, &tv);
	//entity->Set_hItem(hParent, this);
	//entity->DebugButtonsInit(hChildWnd, hInstance, mALL_BID);
}

void TreeViewWindow::DeleteItem(void* item){

	//Window::DeleteTreeViewItem(((Actor*)item)->Name(), item);
}

#include "Game/Game.h"
void* TreeViewWindow::GetSelectItem(){

	return NULL;

	//if (!mSelectActorIntPtr) return NULL;
	//
	//Window::mInspectorWindow.InsertConponentData((Actor*)mSelectActorIntPtr);
	//
	//return (void*)mSelectActorIntPtr;



	HTREEITEM hItem = TreeView_GetSelection(mhWnd);
	if (hItem) {
		TVITEM tvItem;
		tvItem.mask = TVIF_HANDLE | TVIF_PARAM;
		tvItem.hItem = hItem;
		if (TreeView_GetItem(mhWnd, &tvItem)){
			Window::mInspectorWindow.InsertConponentData((Actor*)tvItem.lParam);
			return (void*)tvItem.lParam;
		}
	}

	return NULL;
}


bool TreeViewWindow::IsActive(){
	return true;// GetFocus() == mhWnd;
}