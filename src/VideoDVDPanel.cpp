#include <Application.h>

#if defined(_BEOS_R5_BUILD_) || defined(_BEOS_HAIKU_BUILD_)
#include "MSHLanguageMgr.h"
extern MSHLanguageMgr* gMSHLangMgr;
#define _T(str)	gMSHLangMgr->_T(str).String()
#define _TPS(str)	gMSHLangMgr->_T(str)
#else
#include <locale/Locale.h>
#endif

#include "Application1.h"
#include "VideoDVDPanel.h"

#define CUSTOM_FS_MIMSG	'DVP0'

VideoDVDPanel::VideoDVDPanel(BRect frame):BView(frame, "VideoDVDPanel", B_FOLLOW_LEFT | B_FOLLOW_TOP, B_WILL_DRAW) {
	filesystemPUM=new BPopUpMenu("");
	filesystemMF=new BMenuField(BRect(0,0,200,21),"filesystemMF", _T("Filesystem"), filesystemPUM, false); // "TFILESYSTEM"
	filesystemMF->SetDivider(StringWidth(_T("Filesystem"))+10); // "TFILESYSTEM"

	filesystemPUM->AddItem(new BMenuItem(_T("ISO9660"), NULL)); // "MI:ISO9660"
	filesystemPUM->AddItem(new BMenuItem(_T("ISO9660 long"), NULL)); // "MI:ISO9660 long"
	filesystemPUM->AddItem(new BMenuItem(_T("MacOS hfs"), NULL)); // "MI:MacOS hfs"
	filesystemPUM->AddItem(new BMenuItem(_T("Unix RockRidge"), NULL)); // "MI:Unix RockRidge"
	filesystemPUM->AddItem(new BMenuItem(_T("Windows Joliet"), NULL)); // "MI:Windows Joliet"
	filesystemPUM->AddItem(new BMenuItem(_T("UDF"), NULL)); // "MI:UDF"
	filesystemPUM->AddSeparatorItem();
	BString tmpstring;
	tmpstring << _T("Custom") << B_UTF8_ELLIPSIS; // "MI:Custom..."
	filesystemPUM->AddItem((mi=new BMenuItem( tmpstring.String(), new BMessage(CUSTOM_FS_MIMSG))));
	
//	iso9660MI=new BMenuItem("ISO 9660", NULL);
//	filesystemPUM->AddItem(iso9660MI);
//	iso9660longMI=new BMenuItem("ISO 9660 long", NULL);
//	filesystemPUM->AddItem(iso9660longMI);
//	bfsMI=new BMenuItem("BeOS bfs", NULL);
//	bfsMI->SetEnabled(false);
//	filesystemPUM->AddItem(bfsMI);
//	hfsMI=new BMenuItem("MacOS hfs", NULL);
//	filesystemPUM->AddItem(hfsMI);
//	rockridgeMI=new BMenuItem("Unix RockRidge", NULL);
//	filesystemPUM->AddItem(rockridgeMI);
//	jolietMI=new BMenuItem("Windows Joliet", NULL);
//	filesystemPUM->AddItem(jolietMI);
//	DVDVideoMI=new BMenuItem(_T("MI:DVD Video"), NULL);
//	filesystemPUM->AddItem(DVDVideoMI);
//	DVDDataMI=new BMenuItem(_T("MI:DVD Data"), NULL);
//	filesystemPUM->AddItem(DVDDataMI);
	
	volumenameTC=new BTextControl(BRect(0,0,200,21), "volumenameTC",
			_T("Volume name"), // "TVOLUMENAME"
			_T("Untitled"), // "TUNTITLED"
			new BMessage('mm05'));
	volumenameTC->SetAlignment(B_ALIGN_LEFT, B_ALIGN_LEFT);
	volumenameTC->SetDivider(StringWidth(_T("Volume name"))+28); // "TVOLUMENAME"
	volumenameTC->TextView()->SetMaxBytes(32);
	volumenameTC->SetModificationMessage(new BMessage('mm05'));
	volumenameTC->SetTarget(be_app_messenger);
	AddChild(filesystemMF);
	AddChild(volumenameTC);
	SetViewColor((rgb_color){216,216,216,255});
}

VideoDVDPanel::~VideoDVDPanel() {
	delete filesystemMF;
	delete volumenameTC;
}

void VideoDVDPanel::AllAttached() {
	mi->SetTarget(this, Looper());
	filesystemMF->SetDivider(StringWidth(_T("Volume name"))+30); // "TVOLUMENAME"
	filesystemMF->MoveTo(5,2);
	filesystemMF->ResizeTo(200, 22);
	volumenameTC->MoveTo(5,28);
	volumenameTC->ResizeTo(200, 20);
}

BList	*VideoDVDPanel::GetFilesystem() {
	BList	*list=new BList();
	
	switch (filesystemPUM->IndexOf(filesystemPUM->FindMarked())) {
		// ISO 9660
		case 0:	break;
		// ISO 9660 long
		case 1:	list->AddItem(new BString("-l")); break;
		// MacOS hfs
		case 2:	list->AddItem(new BString("-h")); break;
		// Unix RockRidge
		case 3:	list->AddItem(new BString("-R")); break;
		// Windows Joliet
		case 4:	list->AddItem(new BString("-J"));
				list->AddItem(new BString("-R"));
				break;
		// UDF
		case 5:	list->AddItem(new BString("-udf")); break;
		case 7:	delete list; list=((Application1 *)be_app)->fspanelCV->GetArgumentList();break;
		default:	break;
	}
	return list;
}

//const char	*VideoDVDPanel::GetFileSystem() {
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
//		// DVD Video UDF
//		case 6:	return "-dvd-video"; break;
//		// UDF
//		case 7:	return "-udf"; break;
//		default:	return "ERROR"; break;
//	}
//	return "ERROR";
//}

const char *VideoDVDPanel::GetVolumeName() {
	return volumenameTC->Text();
}

void	VideoDVDPanel::SetVolumeName(const char *name) {
	volumenameTC->SetText(name);
}


void VideoDVDPanel::MessageReceived(BMessage *msg) {
	switch(msg->what) {
		case CUSTOM_FS_MIMSG: {
			be_app_messenger.SendMessage(PREFERENCES_MIMSG);
			if (((Application1 *)be_app)->configW->Lock()) {
				((Application1 *)be_app)->configW->SelectView(
						((Application1 *)be_app)->configW->GetConfigIndex(_T("Filesystem"))); // "OLV_FILESYSTEM"
				((Application1 *)be_app)->configW->Unlock();
			}
			break;
		}
		
		default: {
			BView::MessageReceived(msg);
			break;
		}
	}
}

