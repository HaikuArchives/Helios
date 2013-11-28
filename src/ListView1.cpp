#ifndef _ZETA_USING_DEPRECATED_API_
#define _ZETA_USING_DEPRECATED_API_
#endif
	// To allow PostMessage() to work. Grrr...

#include <stdio.h>
#include <stdlib.h>

#include <Application.h>
#include <PopUpMenu.h>
#include <MenuItem.h>
#include <ScrollBar.h>

#if defined(_BEOS_R5_BUILD_) || defined(_BEOS_HAIKU_BUILD_)
#include "MSHLanguageMgr.h"
extern MSHLanguageMgr* gMSHLangMgr;
#define _T(str)	gMSHLangMgr->_T(str).String()
#define _TPS(str)	gMSHLangMgr->_T(str)
#else
#include <locale/Locale.h>
#endif

#include "Defines.h"
#include "AudioRow.h"
#include "Application1.h"
#include "ConfigFields.h"
#include "Window1.h"
#include "ListView1.h"

extern bool project_has_changed;

ListView1::ListView1()
 : BColumnListView(BRect(0,0, 302, 213-B_H_SCROLL_BAR_HEIGHT), "listview", B_FOLLOW_ALL_SIDES, B_WILL_DRAW|B_FULL_UPDATE_ON_RESIZE, B_NO_BORDER, true)
{
cmenu=new BPopUpMenu("context-menu",false,false);
CDMode=-1;

deleteItem =		new BMenuItem(_T("Delete"), 		new BMessage(DELETE_FILE_MSG)); // "TDELETE"
newfolderItem =		new BMenuItem(_T("New folder"), 	new BMessage(NEW_FOLDER_MSG)); 	// "TNEWFOLDER"
renamefileItem =	new BMenuItem(_T("Rename"), 		new BMessage(RENAME_FILE_MSG)); // "TRENAME"
moveupItem =		new BMenuItem(_T("Move up"), 		new BMessage(MOVE_UP_MSG)); 	// "TMOVEUP"
movedownItem =		new BMenuItem(_T("Move down"), 		new BMessage(MOVE_DOWN_MSG)); 	// "TMOVEDOWN"
indexItem =			new BMenuItem(_T("Set indices"), 	new BMessage(SET_INDICES_MSG));	// "TSETINDICES"
splitItem =			new BMenuItem(_T("Split title"), 	new BMessage(SPLIT_TRACK_MSG));	// "TSPLITTRACK"
CDTEXTTitleItem =	new BMenuItem(_T("CD text title"), 	new BMessage(SETCDTEXTTITLE_MSG));// "TCDTEXTTITLE"
	
//cmenu->ItemAt(0)->SetTarget(be_app_messenger);
cmenu->SetTargetForItems(be_app_messenger);
SetColor(B_COLOR_SELECTION,(rgb_color){90,90,90,255});
SetColor(B_COLOR_NON_FOCUS_SELECTION,(rgb_color){90,90,90,255});
SetColor(B_COLOR_BACKGROUND,(rgb_color){2,255,255,255});
SetColor(B_COLOR_ROWS_1,Color(B_COLOR_BACKGROUND));
SetColor(B_COLOR_ROWS_2,Color(B_COLOR_BACKGROUND));
SetColor(B_COLOR_ROWS_3,Color(B_COLOR_BACKGROUND));
SetColor(B_COLOR_ROWS_4,Color(B_COLOR_BACKGROUND));
SetColor(B_COLOR_ROWS_5,Color(B_COLOR_BACKGROUND));
SetColor(B_COLOR_ROWS_6,Color(B_COLOR_BACKGROUND));
SetColor(B_COLOR_ROWS_7,Color(B_COLOR_BACKGROUND));
SetColor(B_COLOR_ROWS_8,Color(B_COLOR_BACKGROUND));
SetColor(B_COLOR_ROW_DIVIDER,Color(B_COLOR_BACKGROUND));
SetColorBands(8);
}

void ListView1::SetSelectionColor(rgb_color color) {
	SetColor(B_COLOR_SELECTION, color);
}

void ListView1::SetBackgroundColor(rgb_color color) {
	SetColor(B_COLOR_BACKGROUND, color);
	SetColor(B_COLOR_ROWS_1,Color(B_COLOR_BACKGROUND));
	SetColor(B_COLOR_ROWS_2,Color(B_COLOR_BACKGROUND));
	SetColor(B_COLOR_ROWS_3,Color(B_COLOR_BACKGROUND));
	SetColor(B_COLOR_ROWS_4,Color(B_COLOR_BACKGROUND));
	SetColor(B_COLOR_ROWS_5,Color(B_COLOR_BACKGROUND));
	SetColor(B_COLOR_ROWS_6,Color(B_COLOR_BACKGROUND));
	SetColor(B_COLOR_ROWS_7,Color(B_COLOR_BACKGROUND));
	SetColor(B_COLOR_ROWS_8,Color(B_COLOR_BACKGROUND));
	SetColor(B_COLOR_ROW_DIVIDER,Color(B_COLOR_BACKGROUND));
}


void ListView1::AllAttached() {
//	SetEditMode(false);
	MakeFocus(false);
	SetColumnFlags((column_flags)(B_ALLOW_COLUMN_MOVE | B_ALLOW_COLUMN_RESIZE | B_ALLOW_COLUMN_POPUP | B_ALLOW_COLUMN_REMOVE));
	SetMouseTrackingEnabled(false);
	SetSelectionMessage(new BMessage('slct'));
	SetInvocationMessage(new BMessage('mm03'));
	SetTarget(this, Looper());
}

ListView1::~ListView1(void) {
	delete cmenu;
}


void
ListView1::SelectionChanged(void) {

			Window1 *window1=(Window1 *)Window();
			
			((BButton *)window1->view1->iconBAR->ChildViewAt("bar_deleteitem"))->SetEnabled(CurrentSelection()!=NULL);
	//be_app->PostMessage('mm05');
}

void
ListView1::KeyDown(const char *bytes, int32 numByte)
{

if (bytes[0]==B_DELETE) 
     be_app->PostMessage(DELETE_FILE_MSG);

BColumnListView::KeyDown(bytes,numByte);

}

void ListView1::SetDataCDList() {
	while (CountRows()>0) RemoveRow(0);
	while (CountColumns()>0) {
		BColumn	*col=ColumnAt(0);
		//SetColumnVisible(col, true);
		RemoveColumn(col);
/*
		switch(col->LogicalFieldNum()) {
			case TRACKNUMBER_FIELD: {
				((Application1 *)be_app)->CM->SetInt16(TRACKNUMBERCOLUMN_WIDTH, (int16)col->Width());
				break;
			}
			case TRACKNAME_FIELD: {
				((Application1 *)be_app)->CM->SetInt16(TRACKNAMECOLUMN_WIDTH, (int16)col->Width());
				break;
			}
			case LENGTH_FIELD: {
				((Application1 *)be_app)->CM->SetInt16(LENGTHCOLUMN_WIDTH, (int16)col->Width());
				break;
			}
			case BYTES_FIELD: {
				((Application1 *)be_app)->CM->SetInt16(BYTESCOLUMN_WIDTH, (int16)col->Width());
				break;
			}
			case INDEX_FIELD: {
				((Application1 *)be_app)->CM->SetInt16(INDEXCOLUMN_WIDTH, (int16)col->Width());
				break;
			}
			case CDTEXT_FIELD: {
				((Application1 *)be_app)->CM->SetInt16(CDTEXTCOLUMN_WIDTH, (int16)col->Width());
				break;
			}
			default: {
				break;
			}
		}
*/
		delete col;
	}
	
	SetSortingEnabled(true);
	AddColumn(new BBitmapColumn(_T("Icon"), // "FILEICONCOLUMN_TITLE"
				18.0,18.0,18.0, B_ALIGN_CENTER), ICON_FIELD);
	AddColumn(new BFileColumn(_T("File name"), // "FILENAMECOLUMN_TITLE"
				200.0, 60.0, 900.0,0), FILENAME_FIELD);
	AddColumn(new BSizeColumn(_T("Size"), // "SIZECOLUMN_TITLE"
				50.0, 20.0, 120.0,B_ALIGN_RIGHT), SIZE_FIELD);
	ColumnAt(FILENAME_FIELD)->SetEditFlags(B_EDIT_EXIT_RETURN);
	ColumnAt(FILENAME_FIELD)->SetColumnFlags(ColumnAt(FILENAME_FIELD)->GetColumnFlags() | B_ALLOW_COLUMN_EDIT);
	ColumnAt(ICON_FIELD)->SetColumnFlags(B_ALLOW_COLUMN_NONE);
	ColumnAt(ICON_FIELD)->SetShowHeading(false);
	SetSortColumn(ColumnAt(FILENAME_FIELD), false, true);
	for (int32 i=0; i<CountColumns(); i++) {
		SetColumnVisible(ColumnAt(i), true);
	}
	for (int32 i=0; i<CountColumns(); i++) {
		SetColumnVisible(ColumnAt(i), false);
	}
	for (int32 i=0; i<CountColumns(); i++) {
		SetColumnVisible(ColumnAt(i), true);
	}
}


void ListView1::SetAudioCDList() {
	while (CountRows()>0) RemoveRow(0);
	while (CountColumns()>0) {
		BColumn	*col=ColumnAt(0);
		//SetColumnVisible(col, true);
		RemoveColumn(col);/*
		switch(col->LogicalFieldNum()) {
			case FILENAME_FIELD: {
				((Application1 *)be_app)->CM->SetInt16(FILENAMECOLUMN_WIDTH, (int16)col->Width());
				break;
			}
			case SIZE_FIELD: {
				((Application1 *)be_app)->CM->SetInt16(SIZECOLUMN_WIDTH, (int16)col->Width());
				break;
			}
			default: {
				break;
			}
		}*/
		delete col;
	}

	SetSortingEnabled(false);
	AddColumn(new BIntegerColumn(_T("Track"), // "TRACKNUMBERCOLUMN_TITLE"
			30.0, 30.0, 55.0, B_ALIGN_RIGHT), TRACKNUMBER_FIELD);
	AddColumn(new BStringColumn(_T("Pause"), // "PREGAPCOLUMN_TITLE"
			50.0, 50.0, 50.0,1, B_ALIGN_RIGHT), PREGAP_FIELD);
	AddColumn(new BStringColumn(_T("File name"), // "TRACKNAMECOLUMN_TITLE"
			120.0, 60.0, 900.0,0), TRACKNAME_FIELD);
	AddColumn(new BStringColumn(_T("Duration"), // "LENGTHCOLUMN_TITLE"
			80.0, 70.0, 90.0,0, B_ALIGN_RIGHT), LENGTH_FIELD);
	AddColumn(new BSizeColumn(_T("Size"), // "BYTESCOLUMN_TITLE"
			50.0, 30.0, 70.0,B_ALIGN_RIGHT), BYTES_FIELD);
	AddColumn(new BStringColumn(_T("Indices"), // "INDEXCOLUMN_TITLE"
			120.0, 50.0, 250.0,0), INDEX_FIELD);
	AddColumn(new BStringColumn(_T("CD text"), // "CDTEXTCOLUMN_TITLE"
			120.0, 50.0, 250.0,0), CDTEXT_FIELD);
	for (int32 i=0; i<CountColumns(); i++) {
		SetColumnVisible(ColumnAt(i), true);
	}
	for (int32 i=0; i<CountColumns(); i++) {
		SetColumnVisible(ColumnAt(i), false);
	}
	for (int32 i=0; i<CountColumns(); i++) {
		SetColumnVisible(ColumnAt(i), true);
	}
}

void ListView1::ResetTrackNumbers() {
	for (int i=0; i<CountRows(); i++) {
		((AudioRow *)RowAt(i))->SetTrackNumber(i+1);
	}
}

void ListView1::SaveAllStates() {
	BMessage	*msg=new BMessage();
	SaveState(msg);
	switch(CDMode) {
		case DATACD_INDEX: {
			((Application1 *)be_app)->CM->SetMessage(DATA_CD_LIST_STATE, msg);
			break;
		}
		case AUDIOCD_INDEX: {
			((Application1 *)be_app)->CM->SetMessage(AUDIO_CD_LIST_STATE, msg);
			break;
		}
		case BOOTABLECD_INDEX: {
			((Application1 *)be_app)->CM->SetMessage(BOOTABLE_CD_LIST_STATE, msg);
			break;
		}
		case CDEXTRA_INDEX: {
			((Application1 *)be_app)->CM->SetMessage(CD_EXTRA_LIST_STATE, msg);
			break;
		}
		case DVDVIDEO_INDEX: {
			((Application1 *)be_app)->CM->SetMessage(DVD_VIDEO_LIST_STATE, msg);
			break;
		}
		default: break;
	}
	for (int32 i=0; i<CountColumns(); i++) {
		//SetColumnVisible(ColumnAt(i), false);
	}
}

void ListView1::SetListMode(int32 mode) {
/*
	if (((CDMode==DATACD_INDEX) || (CDMode==BOOTABLECD_INDEX) || (CDMode==DVDVIDEO_INDEX))		&&
		((mode==DATACD_INDEX) || (mode==BOOTABLECD_INDEX) || (mode==DVDVIDEO_INDEX)))
		return;
	if (((CDMode==AUDIOCD_INDEX) || (CDMode==CDEXTRA_INDEX))		&&
		((mode==AUDIOCD_INDEX) || (mode==CDEXTRA_INDEX)))
		return;
*/
	// save old state
	SaveAllStates();

	CDMode=mode;
	switch(mode) {
		case DATACD_INDEX: {
			SetDataCDList();
			if (((Application1 *)be_app)->CM->HasData(DATA_CD_LIST_STATE))
				LoadState(((Application1 *)be_app)->CM->GetMessage(DATA_CD_LIST_STATE));
			break;
		}
		case AUDIOCD_INDEX: {
			SetAudioCDList();
			if (((Application1 *)be_app)->CM->HasData(AUDIO_CD_LIST_STATE))
				LoadState(((Application1 *)be_app)->CM->GetMessage(AUDIO_CD_LIST_STATE));
			break;
		}
		case BOOTABLECD_INDEX: {
			SetDataCDList();
			if (((Application1 *)be_app)->CM->HasData(BOOTABLE_CD_LIST_STATE))
				LoadState(((Application1 *)be_app)->CM->GetMessage(BOOTABLE_CD_LIST_STATE));
			break;
		}
		case CDEXTRA_INDEX: {
			SetAudioCDList();
			if (((Application1 *)be_app)->CM->HasData(CD_EXTRA_LIST_STATE))
				LoadState(((Application1 *)be_app)->CM->GetMessage(CD_EXTRA_LIST_STATE));
			break;
		}
		case DVDVIDEO_INDEX: {
			SetDataCDList();
			if (((Application1 *)be_app)->CM->HasData(DVD_VIDEO_LIST_STATE))
				LoadState(((Application1 *)be_app)->CM->GetMessage(DVD_VIDEO_LIST_STATE));
			break;
		}
		default: break;
	}
	for (int32 i=0; i<CountColumns(); i++) {
		SetColumnVisible(ColumnAt(i), true);
	}
	for (int32 i=0; i<CountColumns(); i++) {
		SetColumnVisible(ColumnAt(i), false);
	}
	for (int32 i=0; i<CountColumns(); i++) {
		SetColumnVisible(ColumnAt(i), true);
	}
	SelectionChanged();
	Window1 *window1=(Window1 *)Window();
			
	((BButton *)window1->view1->iconBAR->ChildViewAt("bar_parent"))->SetEnabled((mode==DATACD_INDEX)||(mode==BOOTABLECD_INDEX)||(mode==DVDVIDEO_INDEX));
	((BButton *)window1->view1->iconBAR->ChildViewAt("bar_newfolder"))->SetEnabled((mode==DATACD_INDEX)||(mode==BOOTABLECD_INDEX)||(mode==DVDVIDEO_INDEX));
	((BTextControl *)window1->view1->iconBAR->ChildViewAt("bar_path"))->SetEnabled((mode==DATACD_INDEX)||(mode==BOOTABLECD_INDEX)||(mode==DVDVIDEO_INDEX));
}

bool ListView1::InitiateDrag(BPoint point,  bool wasSelected) {
printf("InitiateDrag():");
	
	point.PrintToStream();
	BRow	*row=FocusRow();
	if (row==NULL) return false;
/*	
	if ((CDMode==DATACD_INDEX) || (CDMode==BOOTABLECD_INDEX)) {	// yes? then we are not in audio mode
		folderRow=new FolderRow(((FolderRow *)row)->GetFilename(),
						((FolderRow *)row)->IsFolder(),
						((FolderRow *)row)->GetBitmap());
	} else */
	if ((CDMode==AUDIOCD_INDEX) || (CDMode==CDEXTRA_INDEX)){
		audioRow=new AudioRow(((AudioRow *)row)->GetTrackNumber(),((AudioRow *)row)->GetFilename(),
						((AudioRow *)row)->GetPregap(),
						((AudioRow *)row)->GetBytes(),
						((AudioRow *)row)->GetLength());
		audioRow->SetCDTitle(((AudioRow *)row)->GetCDTitle());
		audioRow->SetIndexList(((AudioRow *)row)->GetIndexList());
		audioRow->SetPregap(((AudioRow *)row)->GetPregap());
		audioRow->SetStartFrame(((AudioRow *)row)->GetStartFrame());
		audioRow->SetEndFrame(((AudioRow *)row)->GetEndFrame());
//		audioRow->SetStartTime(((AudioRow *)row)->GetStartTime());
//		audioRow->SetEndTime(((AudioRow *)row)->GetEndTime());
		//audioRow=new AudioRow(*((AudioRow *)row));
	}
	
	BMessage	*message=new BMessage(LISTITEM_DROPPED);
	BRect		rect;
	if (GetRowRect(row, &rect)) {
		void *r=(void *)row;
		message->AddPointer("from", r);
		if (!((FolderRow *)row)->IsFolder())
			message->AddInt32("index", (int32)IndexOf(row));
		DragMessage(message, rect);
//		if ((CDMode==AUDIOCD_INDEX) || (CDMode==CDEXTRA_INDEX))
//			RemoveRow(IndexOf(row));
		//delete row;
	}
	return true;
}

void	ListView1::MessageDropped(BMessage *msg, BPoint point) {
printf("MessageDropped():");
	switch(msg->what) {
		case B_MOUSE_DOWN: printf("B_MOUSE_DOWN\n");break;
		case B_SIMPLE_DATA: be_app->PostMessage(msg); break;
		case B_MOUSE_MOVED: printf("B_MOUSE_MOVED\n");break;
		case LISTITEM_DROPPED: 	printf("LISTITEM_DROPPED\n");
				if ((CDMode!=AUDIOCD_INDEX) && (CDMode!=CDEXTRA_INDEX)) break;
				if (msg->WasDropped()) {
					BPoint where=msg->DropPoint();
					ConvertFromScreen(&where);
					where.PrintToStream();
					BRow *draggedRow=NULL;
					if ((CDMode==DATACD_INDEX) || (CDMode==BOOTABLECD_INDEX) || (CDMode==DVDVIDEO_INDEX)) {	// yes? then we are not in audio mode
						draggedRow=folderRow;
					} else
					if ((CDMode==AUDIOCD_INDEX) || (CDMode==CDEXTRA_INDEX)) {
						draggedRow=audioRow;
					}
					void	*row;
					msg->FindPointer("from", &row);
					if (RowAt(where)) {
						int32 index=IndexOf(RowAt(where));
						if (index>0) index--;
						AddRow(draggedRow, index);
					} else
						AddRow(draggedRow);
					RemoveRow(IndexOf((BRow *)row));
					Invalidate();
					ResetTrackNumbers();
					//Refresh();
					
					project_has_changed=true;
				}
				break;
		default:  BColumnListView::MessageDropped(msg, point);
	}
}

void	ListView1::MessageReceived(BMessage *message) {
	switch(message->what) {
		case B_MOUSE_DOWN: break;
		case B_SIMPLE_DATA: break;
		case B_MOUSE_MOVED: break;
		case LISTITEM_DROPPED: break;
		case 'slct': {
			Window1 *window1=(Window1 *)Window();
			
			((BButton *)window1->view1->iconBAR->ChildViewAt("bar_deleteitem"))->SetEnabled(CurrentSelection()!=NULL);
			break;
		}
		//case 'mm03': be_app->PostMessage('mm03'); break;
		default: BColumnListView::MessageReceived(message);break;
	}
}


/*
void
ListView1::MouseDown(BPoint point){
	BColumnListView::MouseDown(point);
	return;
//printf("MouseDown():");
//point.PrintToStream();
bool stay=false;
	BMessage *currentMsg = Window()->CurrentMessage();
	if (currentMsg->what == B_MOUSE_DOWN) {
		uint32 buttons = 0;
		currentMsg->FindInt32("buttons", (int32 *)&buttons);

		uint32 modifiers = 0;
		currentMsg->FindInt32("modifiers", (int32 *)&modifiers);
		if (
			(!(buttons & B_SECONDARY_MOUSE_BUTTON))
//			 ||
//				(
//				(!(modifiers & B_SHIFT_KEY)) &&
//				((buttons & B_SECONDARY_MOUSE_BUTTON))
//				)
			
			
			)	{
				stay=true;
				}

		if ((buttons & B_SECONDARY_MOUSE_BUTTON)) {
			// secondary button was clicked or control key was down, show menu and return

			//BPopUpMenu *menu = new BPopUpMenu("Info");
			//menu->SetFont(be_plain_font);
			//BuildInfoMenu(menu);
			
			//BMenuItem *selected = menu->Go(ConvertToScreen(where));
			//if (selected)
			//	Window()->PostMessage(selected->Message()->what);
			
			//UMBENENNEN
			// nur ein item selektiert
			if ((IndexOf(CurrentSelection(0))>=0) && (IndexOf(CurrentSelection(1))<0))
				{
				renamefileItem->SetEnabled(true);
				indexItem->SetEnabled(true);
				splitItem->SetEnabled(true);
				} else
				{
				renamefileItem->SetEnabled(false);
				indexItem->SetEnabled(false);
				splitItem->SetEnabled(false);
				}
			//LÖSCHEN
			if (!(IndexOf(CurrentSelection(0))<0))
				deleteItem->SetEnabled(true); else
				deleteItem->SetEnabled(false);
			//MOVE UP/DOWN
			if (IndexOf(CurrentSelection(0))<0)
				{
				moveupItem->SetEnabled(false);
				movedownItem->SetEnabled(false);
				CDTEXTTitleItem->SetEnabled(false);
				} else
				{
				moveupItem->SetEnabled(true);
				movedownItem->SetEnabled(true);
				CDTEXTTitleItem->SetEnabled(true);
				}
			cmenu->Go(ConvertToScreen(point),true, stay, false);

		//	return;
		}


	}

}
*/
int32 ListView1::iCountItems() {
	return this->CountRows();
}

void ListView1::iMakeEmpty() {
	this->Clear();
}


void ListView1::AddFile(const char *name, size_t size) {
}

void ListView1::AddFolder(const char *name, size_t size) {
}

void ListView1::AddAudioTrack(const char *name, size_t size, float length, int32 **indeces, const char *title) { 		// length in seconds
}


bool ListView1::IsSelected(int32 i) {
	BRow	*lastSelected=BColumnListView::CurrentSelection();
	
	do {
		if (lastSelected!=NULL) {
			if (IndexOf(lastSelected)==i) return true;
		lastSelected=BColumnListView::CurrentSelection(lastSelected);
		}
	} while (lastSelected!=NULL);
	return false;
}


int32 ListView1::SelectionCount() {
	return CountRows();
}

void ListView1::RemoveRow(int32 index) {
	BRow *row=RowAt(index);
	BColumnListView::RemoveRow(row);
	delete row;
}

BRow *ListView1::CurrentSelection(int32 index) {
	int32 count=-1;
	BRow	*lastSelected=NULL;
	
	do {
		count++;
		lastSelected=BColumnListView::CurrentSelection(lastSelected);
		if (lastSelected!=NULL) {
			if (count==index) return lastSelected;
		}
	} while (lastSelected!=NULL);
	return NULL;
}

int32 ListView1::iCurrentSelection(int32 index) {
	int32 i;
	
	if (CurrentSelection(index)==NULL) i=-1; 
	else i=IndexOf(CurrentSelection(index));
	return i;
}


void	ListView1::EndEditing() {
	BColumnListView::EndEditing();
}
