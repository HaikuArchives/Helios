#ifndef _LISTVIEW1_H_
#define _LISTVIEW1_H_

#include <Window.h>

#include "ColumnListView.h"
#include "Defines.h"
#include "AudioRow.h"
#include "FolderRow.h"

class ListView1 : public BColumnListView
{
public:
	ListView1();
	~ListView1(void);

	int32 iCountItems();
	void iMakeEmpty();

	void SelectionChanged(void);
	virtual void KeyDown(const char* bytes, int32 numByte);
	// virtual void MouseDown(BPoint point);
	void MessageDropped(BMessage*, BPoint point);
	void MessageReceived(BMessage* message);
	bool InitiateDrag(BPoint point, bool wasSelected);
	void AddFile(const char* name, size_t size = 0);
	void AddFolder(const char* name, size_t size = 0);
	void AddAudioTrack(const char* name, size_t size, float length, int32** indeces = NULL,
					   const char* title = ""); // length in seconds
	void MakeEmpty();
	bool IsSelected(int32 i);
	int32 SelectionCount();
	void RemoveRow(int32 index);
	BRow* CurrentSelection(int32 index = 0);
	int32 iCurrentSelection(int32 index = 0);

	void SaveAllStates();

	void SetSelectionColor(rgb_color color);
	void SetBackgroundColor(rgb_color color);
	void SetListMode(int32 mode);
	void SetDataCDList();
	void SetAudioCDList();
	void EndEditing();

	void ResetTrackNumbers();

	void AllAttached(void);

	BPopUpMenu* cmenu;
	BMenuItem* deleteItem;
	BMenuItem* newfolderItem;
	BMenuItem* renamefileItem;
	BMenuItem* moveupItem;
	BMenuItem* movedownItem;
	BMenuItem* indexItem;
	BMenuItem* splitItem;
	BMenuItem* CDTEXTTitleItem;

	int32 dragged;
	AudioRow* audioRow;
	FolderRow* folderRow;
	int32 CDMode;
};

#endif
