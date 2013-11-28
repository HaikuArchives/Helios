#include <Application.h>

#if defined(_BEOS_R5_BUILD_) || defined(_BEOS_HAIKU_BUILD_)
#include "MSHLanguageMgr.h"
extern MSHLanguageMgr* gMSHLangMgr;
#define _T(str)	gMSHLangMgr->_T(str).String()
#define _TPS(str)	gMSHLangMgr->_T(str)
#else
#include <locale/Locale.h>
#endif

#include "Defines.h"
#include "FileColumn.h"


BFileColumn::BFileColumn(const char *title,
				float width,
				float maxWidth,
				float minWidth,
				uint32 truncate,
				alignment align, bool contextmenu)
				: BStringColumn(title, width, maxWidth, minWidth, truncate, align) {
	
	
	contextM=contextmenu;

	if (contextM) {
		cmenu=new BPopUpMenu("context-menu",false,false);
		deleteItem=new BMenuItem(_T("Delete"), // "TDELETE"
			new BMessage(DELETE_FILE_MSG));
		newfolderItem=new BMenuItem(_T("New folder"), // "TNEWFOLDER"
			new BMessage(NEW_FOLDER_MSG));
		renamefileItem=new BMenuItem(_T("Rename"), // "TRENAME"
			new BMessage(RENAME_FILE_MSG));
		moveupItem=new BMenuItem(_T("Move up"), // "TMOVEUP"
			new BMessage(MOVE_UP_MSG));
		movedownItem=new BMenuItem(_T("Move down"), // "TMOVEDOWN"
			new BMessage(MOVE_DOWN_MSG));
		indexItem=new BMenuItem(_T("Set indices"), // "TSETINDICES"
			new BMessage(SET_INDICES_MSG));
		splitItem=new BMenuItem(_T("Split title"), // "TSPLITTRACK"
			new BMessage(SPLIT_TRACK_MSG));
		CDTEXTTitleItem=new BMenuItem(_T("CD text title"), // "TCDTEXTTITLE"
			new BMessage(SETCDTEXTTITLE_MSG));
		//cmenu->ItemAt(0)->SetTarget(be_app_messenger);
		cmenu->AddItem(newfolderItem);
		cmenu->AddItem(renamefileItem);
		cmenu->AddItem(deleteItem);
		cmenu->SetTargetForItems(be_app_messenger);
	}
	kFoldersFirst = true;
}


void	BFileColumn::SortFoldersFirst(bool foldersFirst) {
	
	kFoldersFirst=foldersFirst;
}

int	BFileColumn::CompareFields(BField* field1, BField* field2) {
	
	BFileField	*f1=(BFileField *)field1, *f2=(BFileField *)field2;
	
	if (kFoldersFirst) {
		// both fields are folders
		if ((f1->IsFolder()) && (f2->IsFolder())) {
			if (f1->IsParentFolder()) return -1; else
			if (f2->IsParentFolder()) return 1; else
			return BStringColumn::CompareFields(field1, field2);
		} else
		// field1 is a folder
		if ((f1->IsFolder()) && (!f2->IsFolder())) {
			return -1;
		} else
		// field2 is a folder
		if ((!f1->IsFolder()) && (f2->IsFolder())) {
			return 1;
		} else	
		// no folder anywhere...
		if ((!f1->IsFolder()) && (!f2->IsFolder())) {
			return BStringColumn::CompareFields(field1, field2);
		}
	}
	return BStringColumn::CompareFields(field1, field2);
}

/*
void BFileColumn::MouseDown(BColumnListView *parent, BRow *row, BField *field,
					              BRect field_rect, BPoint point, uint32 buttons) {
	if (contextM)
	if (buttons & B_SECONDARY_MOUSE_BUTTON) {
		parent->DeselectAll();
		parent->SetFocusRow(row, true);

		if (parent->CurrentSelection(0)!=NULL) {
			renamefileItem->SetEnabled(true);
			indexItem->SetEnabled(true);
			splitItem->SetEnabled(true);
			deleteItem->SetEnabled(true);
			moveupItem->SetEnabled(true);
			movedownItem->SetEnabled(true);
			CDTEXTTitleItem->SetEnabled(true);
		} else	{
			renamefileItem->SetEnabled(false);
			indexItem->SetEnabled(false);
			splitItem->SetEnabled(false);
			deleteItem->SetEnabled(false);
			moveupItem->SetEnabled(false);
			movedownItem->SetEnabled(false);
			CDTEXTTitleItem->SetEnabled(false);
		}
		cmenu->SetTargetForItems(be_app_messenger);
		cmenu->Go(parent->ScrollView()->ConvertToScreen(point),true);
	}
	BColumn::MouseDown(parent, row, field, field_rect, point, buttons);
}
*/
