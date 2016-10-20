#include <Screen.h>
#include <Application.h>
#include <OS.h>
#include <Path.h>
#include <Bitmap.h>
#include <NodeInfo.h>
#include <FilePanel.h>

#if defined(_BEOS_R5_BUILD_) || defined(_BEOS_HAIKU_BUILD_)
#include "MSHLanguageMgr.h"
extern MSHLanguageMgr* gMSHLangMgr;
#define _T(str) gMSHLangMgr->_T(str).String()
#define _TPS(str) gMSHLangMgr->_T(str)
#else
#include <locale/Locale.h>
#endif

#include "FileDialog.h"
#include "Application1.h"
#include "ColumnTypes.h"
#include "FileColumn.h"
#include "Defines.h"
#include "FileChooser.h"

// Messages
#define OK_BUTTON_MSG 'FCok'
#define CANCEL_BUTTON_MSG 'FCcn'
#define BROWSE_BUTTON_MSG 'FCbr'
#define SELECTION_MESSAGE 'FCsl'

FileChooser::FileChooser()
	: BWindow(BRect(0, 0, 330, 250), _T("Select file"), // "TCHOOSEFILEWINDOWTITLE"
			  B_DOCUMENT_WINDOW, B_NOT_ZOOMABLE)
{

	sem = create_sem(0, "FileChooser");
	fileList = new BList();
	winView = new BView(Bounds(), "FileChooserView", B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP_BOTTOM,
						B_WILL_DRAW);
	winView->SetViewColor(216, 216, 216);
	listView = new BColumnListView(BRect(0, 42, 331, 251), "FileChooserCLV",
								   B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP_BOTTOM, B_WILL_DRAW);
	okB = new BButton(BRect(253, 7, 323, 34), "OK_Button", _T("Use"), new BMessage(OK_BUTTON_MSG),
					  B_FOLLOW_TOP | B_FOLLOW_RIGHT);
	cancelB = new BButton(BRect(173, 7, 243, 34), "CANCEL_Button", _T("Cancel"),
						  new BMessage(B_QUIT_REQUESTED), B_FOLLOW_TOP | B_FOLLOW_RIGHT);
	browseB = new BButton(BRect(85, 7, 155, 34), "SELECT_Button", _T("Browse"),
						  new BMessage(BROWSE_BUTTON_MSG), B_FOLLOW_TOP | B_FOLLOW_RIGHT);

	SetSizeLimits(330, 50000, 250, 50000);

	filepanel = NULL;
	listView->SetColor(B_COLOR_SELECTION, (rgb_color){90, 90, 90, 255});
	listView->SetColor(B_COLOR_NON_FOCUS_SELECTION, (rgb_color){150, 150, 150, 255});
	listView->SetColor(B_COLOR_BACKGROUND, (rgb_color){255, 255, 255, 255});
	listView->SetColor(B_COLOR_ROWS_1, listView->Color(B_COLOR_BACKGROUND));
	listView->SetColor(B_COLOR_ROWS_2, listView->Color(B_COLOR_BACKGROUND));
	listView->SetColor(B_COLOR_ROWS_3, listView->Color(B_COLOR_BACKGROUND));
	listView->SetColor(B_COLOR_ROWS_4, listView->Color(B_COLOR_BACKGROUND));
	listView->SetColor(B_COLOR_ROWS_5, listView->Color(B_COLOR_BACKGROUND));
	listView->SetColor(B_COLOR_ROWS_6, listView->Color(B_COLOR_BACKGROUND));
	listView->SetColor(B_COLOR_ROWS_7, listView->Color(B_COLOR_BACKGROUND));
	listView->SetColor(B_COLOR_ROWS_8, listView->Color(B_COLOR_BACKGROUND));
	listView->SetColor(B_COLOR_ROW_DIVIDER, listView->Color(B_COLOR_BACKGROUND));
	listView->SetColorBands(8);
	listView->SetEditMode(false);
	listView->MakeFocus(false);
	listView->SetColumnFlags((column_flags)(B_ALLOW_COLUMN_MOVE | B_ALLOW_COLUMN_RESIZE |
											B_ALLOW_COLUMN_POPUP | B_ALLOW_COLUMN_REMOVE));
	listView->SetMouseTrackingEnabled(false);
	listView->SetSelectionMessage(new BMessage(SELECTION_MESSAGE));
	listView->SetSortingEnabled(true);
	listView->AddColumn(new BBitmapColumn(_T("Icon"), // "FILEICONCOLUMN_TITLE"
										  18, 18, 18, B_ALIGN_CENTER),
						ICON_FIELD);
	listView->AddColumn(new BFileColumn(_T("File name"), // "FILENAMECOLUMN_TITLE"
										200, 60, 900, 0, B_ALIGN_LEFT, false),
						FILENAME_FIELD);
	listView->AddColumn(new BSizeColumn(_T("Size"), // "SIZECOLUMN_TITLE"
										57, 20, 100, B_ALIGN_RIGHT),
						SIZE_FIELD);
	listView->ColumnAt(ICON_FIELD)->SetColumnFlags(B_ALLOW_COLUMN_NONE);
	listView->ColumnAt(ICON_FIELD)->SetShowHeading(false);
	listView->SetSortColumn(listView->ColumnAt(FILENAME_FIELD), false, true);
	listView->SetSelectionMode(B_SINGLE_SELECTION_LIST);
	listView->SetTarget(this);
	listView->SetInvocationMessage(new BMessage(OK_BUTTON_MSG));
	// AddChilds
	AddChild(winView);
	winView->AddChild(listView);
	winView->AddChild(okB);
	winView->AddChild(cancelB);
	winView->AddChild(browseB);
	okB->SetEnabled(false);
	SetDefaultButton(okB);

	BScreen* screen = new BScreen();
	MoveTo((int)((screen->Frame().Width() - Bounds().Width()) / 2),
		   (int)((screen->Frame().Height() - Bounds().Height()) / 2));
	delete screen;
}

FileChooser::~FileChooser()
{
	delete_sem(sem);
	for (int i = fileList->CountItems() - 1; i >= 0; i--) {
		delete (BFile*)(fileList->RemoveItem(i));
	}
	delete fileList;
}

void FileChooser::AddFile(const char* path)
{
	BEntry* file = new BEntry(path, B_READ_ONLY);
	this->AddFile(file);
	delete file;
}

void FileChooser::AddFile(BEntry* file)
{
	BPath path;
	BBitmap* icon = new BBitmap(BRect(0.0, 0.0, 15.0, 15.0), B_CMAP8);
	entry_ref r;
	off_t size;

	if (file->Exists()) {
		file->GetRef(&r);
		file->GetSize(&size);
		BNodeInfo::GetTrackerIcon(&r, icon, B_MINI_ICON);
		fileList->AddItem(new BEntry(*file));
		file->GetPath(&path);
		listView->AddRow(new FolderRow(path.Path(), size, file->IsDirectory(), icon, false));
	}
}

const char* FileChooser::Go()
{
	SetWorkspaces(B_CURRENT_WORKSPACE);
	this->Show();
	acquire_sem(sem); // wait
	return strdup(filename.String());
}

//
// PRIVATE
//

void FileChooser::MessageReceived(BMessage* msg)
{
	switch (msg->what) {
	case SELECTION_MESSAGE: {
		if (listView->CurrentSelection() == NULL)
			okB->SetEnabled(false);
		else
			okB->SetEnabled(true);
		break;
	}

	case B_REFS_RECEIVED:
	case B_SIMPLE_DATA: {
		uint32 type;
		int32 count;
		BEntry* entry = new BEntry();
		entry_ref ref;

		msg->GetInfo("refs", &type, &count);

		if (type != B_REF_TYPE) {
			delete entry;
			break;
		}
		if (msg->FindRef("refs", 0, &ref) == B_OK)
			if (entry->SetTo(&ref, true) == B_OK) this->AddFile(entry);
		entry->Unset();
		delete entry;
		break;
	}

	case OK_BUTTON_MSG: {
		filename = ((FolderRow*)listView->CurrentSelection())->GetFilename();
		release_sem(sem);
		Quit();
		break;
	}

	case CANCEL_BUTTON_MSG: {
		filename = "";
		release_sem(sem);
		Quit();
		break;
	}

	case B_QUIT_REQUESTED: {
		filename = "";
		release_sem(sem);
		break;
	}

	case B_CANCEL: {
		delete filepanel;
		break;
	}

	case BROWSE_BUTTON_MSG: {
		FileDialog::fmainWindow = NULL;
		FileDialog::fSourceWindow = this;
		BEntry* entry = FileDialog::OpenDialog(NULL, "/boot/home/config/bin/");

		this->AddFile(entry);
		entry->Unset();
		delete entry;
		break;
	}

	default: {
		BWindow::MessageReceived(msg);
		break;
	}
	}
}
