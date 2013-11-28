#include <strings.h>

#include <String.h>
#include <Path.h>
#include <Entry.h>

#if defined(_BEOS_R5_BUILD_) || defined(_BEOS_HAIKU_BUILD_)
#include "MSHLanguageMgr.h"
extern MSHLanguageMgr* gMSHLangMgr;
#define _T(str)	gMSHLangMgr->_T(str).String()
#define _TPS(str)	gMSHLangMgr->_T(str)
#else
#include <locale/Locale.h>
#endif

#include "Defines.h"
#include "Application1.h"
#include "FolderRow.h"
//#include "syslogprot.h"
#include "CDExtraPanel.h"

//extern SyslogProt	*SYSLOG;

#define CDTEXTBOX_MSG 'AP01'
#define CUSTOM_FS_MIMSG	'AP02'


CDExtraPanel::CDExtraPanel(BRect frame):BView(frame, "CDExtraPanel", B_FOLLOW_LEFT | B_FOLLOW_TOP, B_WILL_DRAW) {
	cdtextCB=new BCheckBox(BRect(frame.left+4, frame.top, frame.right-4, frame.top+16), "xcdp_writeCDText",
									_T("Write CD text"), // "TWRITECDTEXT"
									new BMessage(CDTEXTBOX_MSG));
	copyprotectionCB=new BCheckBox(BRect(frame.left+4, frame.top, frame.right-4, frame.top+16), "xcdp_copyprotection",
									_T("Add SCMS-Copyright"), // "TCOPYPROTECTION"
									NULL);
	CDTextAlbum=new BTextControl(BRect(frame.left+4, frame.top, frame.right-4, frame.top+16), "xcdp_cdtextalbum",
									_T("CD text album"), // "TCDTEXTALBUMLABEL"
									"", NULL);
	//AddChild(cdtextCB);
//	AddChild(copyprotectionCB);
	//AddChild(CDTextAlbum);
	SetViewColor((rgb_color){216,216,216,255});
	imagenameTC=new BTextControl(BRect(0,0,200,21), "xcdp_imagenameTC", 
							_T("Data path"), // "TDATAIMAGE"
							"", NULL);
	imagenameTC->SetDivider(StringWidth(_T("Volume name"))+28); // "TVOLUMENAME"
	CDTextAlbum->SetDivider(StringWidth(_T("Volume name"))+28); // "TVOLUMENAME"

	filesystemPUM=new BPopUpMenu("");
	filesystemMF=new BMenuField(BRect(0,0,200,21),"dcdp_filesystemMF",
							_T("Filesystem"), // "TFILESYSTEM"
							filesystemPUM, false);
	filesystemMF->SetDivider(StringWidth(_T("Filesystem"))+10); // "TFILESYSTEM"

	filesystemPUM->AddItem(new BMenuItem(_T("ISO9660"), NULL)); // "MI:ISO9660"
	filesystemPUM->AddItem(new BMenuItem(_T("ISO9660 long"), NULL)); // "MI:ISO9660 long"
	filesystemPUM->AddItem(new BMenuItem(_T("BeOS bfs"), NULL)); // "MI:BeOS bfs"
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
//	filesystemPUM->AddItem(bfsMI);
//	hfsMI=new BMenuItem("MacOS hfs", NULL);
//	filesystemPUM->AddItem(hfsMI);
//	rockridgeMI=new BMenuItem("Unix RockRidge", NULL);
//	filesystemPUM->AddItem(rockridgeMI);
//	jolietMI=new BMenuItem("Windows Joliet", NULL);
//	filesystemPUM->AddItem(jolietMI);
	
	volumenameTC=new BTextControl(BRect(0,0,200,21), "dcdp_volumenameTC", 
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
	AddChild(imagenameTC);

}

CDExtraPanel::~CDExtraPanel() {
	delete imagenameTC;
	delete cdtextCB;
	delete copyprotectionCB;
	delete CDTextAlbum;
}

void CDExtraPanel::AllAttached() {
	mi->SetTarget(this, Looper());
//	copyprotectionCB->MoveTo(5,28);
//	copyprotectionCB->ResizeTo(22+StringWidth(copyprotectionCB->Label()), 16);
	filesystemMF->SetDivider(StringWidth(_T("Volume name"))+30); // "TVOLUMENAME"
	filesystemMF->MoveTo(5,2);
	filesystemMF->ResizeTo(200, 22);
	volumenameTC->MoveTo(5,28);
	volumenameTC->ResizeTo(200, 20);


	//cdtextCB->MoveTo(5,52);
	//cdtextCB->ResizeTo(22+StringWidth(cdtextCB->Label()), 16);
	//CDTextAlbum->MoveTo(5, 77);
	//CDTextAlbum->ResizeTo(200, 20);
	imagenameTC->MoveTo(5,54);
	imagenameTC->ResizeTo(200,20);
	// initializes the textcontrol with the previously loaded user-settings
	CDTextAlbum->SetEnabled(cdtextCB->Value()==B_CONTROL_ON);
	cdtextCB->SetTarget(this);
}

BList	*CDExtraPanel::GetFilesystem() {
	BList	*list=new BList();
	
	switch (filesystemPUM->IndexOf(filesystemPUM->FindMarked())) {
		// ISO 9660
		case 0:	break;
		// ISO 9660 long
		case 1:	list->AddItem(new BString("-l")); break;
		// BeOS bfs
		case 2:	list->AddItem(new BString("BFS")); break;
		// MacOS hfs
		case 3:	list->AddItem(new BString("-h")); 
				list->AddItem(new BString("-part")); 
				break;
		// Unix RockRidge
		case 4:	list->AddItem(new BString("-R")); break;
		// Windows Joliet
		case 5:	list->AddItem(new BString("-J"));
				list->AddItem(new BString("-R"));
				break;
		// UDF
		case 6:	list->AddItem(new BString("-udf")); break;
		case 8:	delete list; list=((Application1 *)be_app)->fspanelCV->GetArgumentList();break;
		default:	break;
	}

	// { DEBUG
	/*	char debug[512];
		sprintf(debug, "Helios|CDExtraPanel::GetFilesystem()|");
		for (int i=0; i<list->CountItems(); i++)
			sprintf(debug, "%s[%s]", strdup(debug), ((BString *)list->ItemAt(i))->String());
		SYSLOG->SendSyslogMessage(1, 7, debug);
	*/// } DEBUG
	
	return list;
}


//const char	*CDExtraPanel::GetFileSystem() {
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
//		default:	return "ERROR"; break;
//	}
//	return "ERROR";
//}

const char *CDExtraPanel::GetVolumeName() {
	return volumenameTC->Text();
}

void	CDExtraPanel::SetVolumeName(const char *name) {
	volumenameTC->SetText(name);
}

void CDExtraPanel::MessageReceived(BMessage *msg) {
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
		case CDTEXTBOX_MSG:
			CDTextAlbum->SetEnabled(cdtextCB->Value()==B_CONTROL_ON);
			break;
			
		case B_SIMPLE_DATA: {
			uint32 		type;
			int32 		count;
			BEntry		*entry=new BEntry();
			BPath		*path=new BPath();
			entry_ref	ref;	
	
			msg->GetInfo("refs", &type, &count);
		
			if (type != B_REF_TYPE) { delete entry; return;}
			if (msg->FindRef("refs", 0, &ref) == B_OK) {
				if (entry->SetTo(&ref,true)==B_OK)
					entry->GetPath(path);
					SetImageName(path->Path());
			}
			delete entry;	
			delete path;
			break;
		}
//		case LISTITEM_DROPPED:
//				int32	i;
//				if (msg->FindInt32("index", &i)==B_OK) {
//					BString str;
//					const char *str2;
//					str.SetTo(((Application1 *)be_app)->current_folder.String());
//					str.RemoveFirst(((Application1 *)be_app)->base_folder.String());
//					str.Prepend("/");
//					//str.RemoveAll("//");
//					//if (str.Length()==0) str="/";
//					str2=((FolderRow *)(((Application1 *)be_app)->window1->view1->listview1->RowAt(i)))->GetFilename();
//					str.Append(str2);
//					SetImageName(str.String());
//				}
//				break;
		default:
			BView::MessageReceived(msg);
			break;
	}
}

bool CDExtraPanel::writeCDText() {
	return (cdtextCB->Value()==B_CONTROL_ON);
}

void CDExtraPanel::SetCDText(bool flag) {
	if (flag)  	cdtextCB->SetValue(B_CONTROL_ON); else
			cdtextCB->SetValue(B_CONTROL_OFF);
}

bool CDExtraPanel::copyProtection() {
	return (copyprotectionCB->Value()==B_CONTROL_ON);
}

void CDExtraPanel::SetProtection(bool flag) {
	if (flag)  	copyprotectionCB->SetValue(B_CONTROL_ON); else
			copyprotectionCB->SetValue(B_CONTROL_OFF);
}

void CDExtraPanel::SetCDAlbum(const char *name) {
	CDTextAlbum->SetText(name);
}

const char *CDExtraPanel::GetCDAlbum() {
	return strdup(CDTextAlbum->Text());
}

const char *CDExtraPanel::GetImageName() {
	return strdup(imagenameTC->Text());
}

void CDExtraPanel::SetImageName(const char *filename) {
	imagenameTC->SetText(filename);
}
