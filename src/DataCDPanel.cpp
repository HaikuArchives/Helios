#include <Application.h>
#include "Application1.h"
#include <Messenger.h>

#if defined(_BEOS_R5_BUILD_) || defined(_BEOS_HAIKU_BUILD_)
#include "MSHLanguageMgr.h"
extern MSHLanguageMgr* gMSHLangMgr;
#define _T(str) gMSHLangMgr->_T(str).String()
#define _TPS(str) gMSHLangMgr->_T(str)
#else
#include <locale/Locale.h>
#endif

#include "ConfigWindow.h"
//#include "syslogprot.h"
#include "DataCDPanel.h"

// extern SyslogProt	*SYSLOG;

#define CUSTOM_FS_MIMSG 'DCP0'
#define MULTISESSION_MSG 'DCP1'

DataCDPanel::DataCDPanel(BRect frame)
	: BView(frame, "DataCDPanel", B_FOLLOW_LEFT | B_FOLLOW_TOP, B_WILL_DRAW)
{
	multisessionCB =
		new BCheckBox(BRect(frame.left + 4, frame.top, frame.right - 4, frame.top + 16),
					  "dcdp_multisessionCB", _T("Multisession-CD"), // "TMULTISESSION"
					  new BMessage(MULTISESSION_MSG));
	multisessionCB->SetTarget(this, Looper());
	filesystemPUM = new BPopUpMenu("");
	filesystemMF =
		new BMenuField(BRect(0, 0, 200, 21), "dcdp_filesystemMF", _T("Filesystem"), // "TFILESYSTEM"
					   filesystemPUM, false);
	filesystemMF->SetDivider(StringWidth(_T("Filesystem")) + 10); // "TFILESYSTEM"

	filesystemPUM->AddItem(new BMenuItem(_T("ISO9660"), NULL));		   // "MI:ISO9660"
	filesystemPUM->AddItem(new BMenuItem(_T("ISO9660 long"), NULL));   // "MI:ISO9660 long"
	filesystemPUM->AddItem(new BMenuItem(_T("BeOS bfs"), NULL));	   // "MI:BeOS bfs"
	filesystemPUM->AddItem(new BMenuItem(_T("MacOS hfs"), NULL));	  // "MI:MacOS hfs"
	filesystemPUM->AddItem(new BMenuItem(_T("Unix RockRidge"), NULL)); // "MI:Unix RockRidge"
	filesystemPUM->AddItem(new BMenuItem(_T("Windows Joliet"), NULL)); // "MI:Windows Joliet"
	filesystemPUM->AddItem(new BMenuItem(_T("UDF"), NULL));			   // "MI:UDF"
	filesystemPUM->AddSeparatorItem();
	BString tmpstring;
	tmpstring << _T("Custom") << B_UTF8_ELLIPSIS; // "MI:Custom..."
	filesystemPUM->AddItem((mi = new BMenuItem(tmpstring.String(), new BMessage(CUSTOM_FS_MIMSG))));

	volumenameTC = new BTextControl(BRect(0, 0, 200, 21), "dcdp_volumenameTC",
									_T("Volume name"), // "TVOLUMENAME"
									_T("Untitled"),	// "TUNTITLED"
									new BMessage('mm05'));
	volumenameTC->SetAlignment(B_ALIGN_LEFT, B_ALIGN_LEFT);
	volumenameTC->SetDivider(StringWidth(_T("Volume name")) + 28); // "TVOLUMENAME"
	volumenameTC->TextView()->SetMaxBytes(32);
	volumenameTC->SetModificationMessage(new BMessage('mm05'));
	volumenameTC->SetTarget(be_app_messenger);
	AddChild(filesystemMF);
	AddChild(volumenameTC);
	AddChild(multisessionCB);
	SetViewColor((rgb_color){216, 216, 216, 255});
}

DataCDPanel::~DataCDPanel()
{
	delete multisessionCB;
	delete filesystemMF;
	delete volumenameTC;
}

void DataCDPanel::AllAttached()
{
	mi->SetTarget(this, Looper());
	multisessionCB->SetTarget(this, Looper());

	filesystemMF->SetDivider(StringWidth(_T("Volume name")) + 30); // "TVOLUMENAME"
	filesystemMF->MoveTo(5, 2);
	filesystemMF->ResizeTo(200, 22);
	volumenameTC->MoveTo(5, 28);
	volumenameTC->ResizeTo(200, 20);
	multisessionCB->MoveTo(5, 54);
	multisessionCB->ResizeTo(22 + StringWidth(multisessionCB->Label()), 16);
}

BList* DataCDPanel::GetFilesystem()
{
	BList* list = new BList();

	switch (filesystemPUM->IndexOf(filesystemPUM->FindMarked())) {
	// ISO 9660
	case 0:
		break;
	// ISO 9660 long
	case 1:
		list->AddItem(new BString("-l"));
		break;
	// BeOS bfs
	case 2:
		list->AddItem(new BString("BFS"));
		break;
	// MacOS hfs
	case 3:
		list->AddItem(new BString("-h"));
		break;
	// Unix RockRidge
	case 4:
		list->AddItem(new BString("-R"));
		break;
	// Windows Joliet
	case 5:
		list->AddItem(new BString("-J"));
		list->AddItem(new BString("-R"));
		break;
	// UDF
	case 6:
		list->AddItem(new BString("-udf"));
		break;
	case 8:
		delete list;
		list = ((Application1*)be_app)->fspanelCV->GetArgumentList();
		break;
	default:
		break;
	}

	// { DEBUG
	/*	char debug[512];
		sprintf(debug, "Helios|DataCDPanel::GetFilesystem()|");
		for (int i=0; i<list->CountItems(); i++)
			sprintf(debug, "%s[%s]", strdup(debug), ((BString *)list->ItemAt(i))->String());
		SYSLOG->SendSyslogMessage(1, 7, debug);
	*/ // } DEBUG

	return list;
}

// const char	*DataCDPanel::GetFileSystem() {
//	switch (filesystemPUM->IndexOf(filesystemPUM->FindMarked())) {
//		// ISO 9660
//		case 0:	return "-pad"; break;
//		// ISO 9660 long
//		case 1:	return "-l"; break;
//		// BeOS bfs
//		case 2:	return "-bfs ;-)"; break;
//		// MacOS hfs
//		case 3:	return "-h"; break;
//		// Unix RockRidge
//		case 4:	return "-pad"; break;
//		// Windows Joliet
//		case 5:	return "-J"; break;
//		// UDF
//		case 6:	return "-udf"; break;
//		default:	return "ERROR"; break;
//	}
//	return "ERROR";
//}

int DataCDPanel::GetFilesystemIndex()
{
	return filesystemPUM->IndexOf(filesystemPUM->FindMarked());
}

bool DataCDPanel::IsMultisession()
{
	return (multisessionCB->Value() == B_CONTROL_ON);
}

void DataCDPanel::SetMultisession(bool flag)
{
	if (flag)
		multisessionCB->SetValue(B_CONTROL_ON);
	else
		multisessionCB->SetValue(B_CONTROL_OFF);
}

const char* DataCDPanel::GetVolumeName()
{
	return volumenameTC->Text();
}

void DataCDPanel::SetVolumeName(const char* name)
{
	volumenameTC->SetText(name);
}

void DataCDPanel::MessageReceived(BMessage* msg)
{
	switch (msg->what) {
	case CUSTOM_FS_MIMSG: {
		be_app_messenger.SendMessage(PREFERENCES_MIMSG);
		if (((Application1*)be_app)->configW->Lock()) {
			((Application1*)be_app)
				->configW->SelectView(
					((Application1*)be_app)
						->configW->GetConfigIndex(_T("Filesystem"))); // "OLV_FILESYSTEM"
			((Application1*)be_app)->configW->Unlock();
		}
		break;
	}

	case MULTISESSION_MSG: {
		if (multisessionCB->Value() == B_CONTROL_ON) {
			Application1* app = ((Application1*)be_app);

			if (app->configW->Lock()) {
				if (app->heliosCV->IsDAO()) {
					app->heliosCV->SetDAO(false);
					if (app->window1->Lock()) {
						app->window1->view1->statusBAR->SetText(_T("DAO disabled."),
																4000000); // "L:DAO disabled."
						app->window1->Unlock();
					}
				}
				app->configW->Unlock();
			}
		}
		break;
	}

	default: {
		BView::MessageReceived(msg);
		break;
	}
	}
}
