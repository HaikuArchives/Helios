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
#include "FolderRow.h"
#include "Application1.h"
#include "ListView1.h"
#include "ErrorBox.h"
//#include "syslogprot.h"
#include "BootableCDPanel.h"

//extern SyslogProt	*SYSLOG;

#define CUSTOM_FS_MIMSG	'BCP0'

BootableCDPanel::BootableCDPanel(BRect frame):BView(frame, "DataCDPanel", B_FOLLOW_LEFT | B_FOLLOW_TOP, B_WILL_DRAW) {
	filesystemPUM=new BPopUpMenu("");
	filesystemMF=new BMenuField(BRect(0,0,200,21),"bcdp_filesystemMF", _T("Filesystem"), // "TFILESYSTEM"
						filesystemPUM, false);
	filesystemMF->SetDivider(StringWidth(_T("Filesystem"))+10); // "TFILESYSTEM"
	AddChild(filesystemMF);
	
	filesystemPUM->AddItem(new BMenuItem(_T("ISO9660"), NULL)); // "MI:ISO9660"
	filesystemPUM->AddItem(new BMenuItem(_T("ISO9660 long"), NULL)); // "MI:ISO9660 long"
	filesystemPUM->AddItem(new BMenuItem(_T("MacOS hfs"), NULL)); // "MI:MacOS hfs"
	filesystemPUM->AddItem(new BMenuItem(_T("Unix RockRidge"), NULL)); // "MI:Unix RockRidge"
	filesystemPUM->AddItem(new BMenuItem(_T("Windows Joliet"), NULL)); // "MI:Windows Joliet"
	filesystemPUM->AddItem(new BMenuItem(_T("UDF"), NULL)); // "MI:UDF"
	filesystemPUM->AddSeparatorItem();
	BString tmpstring;
	tmpstring << _T("Custom") << B_UTF8_ELLIPSIS;  // "MI:Custom..."
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
	
	volumenameTC=new BTextControl(BRect(0,0,200,21), "bcdp_volumenameTC",
														_T("Volume name"), // "TVOLUMENAME"
														_T("Untitled"), // "TUNTITLED"
														new BMessage('mm05'));
	volumenameTC->SetAlignment(B_ALIGN_LEFT, B_ALIGN_LEFT);
	volumenameTC->SetDivider(StringWidth(_T("Volume name"))+28); // "TVOLUMENAME"
	volumenameTC->TextView()->SetMaxBytes(32);
	volumenameTC->SetModificationMessage(new BMessage('mm05'));
	volumenameTC->SetTarget(be_app_messenger);
	AddChild(volumenameTC);
	eltoritoimageTC=new BTextControl(BRect(0,0,200,21), "bcdp_eltoritoimageTC",
							_T("Floppy image"), // "TELTORITOIMAGE"
							"", NULL);
	eltoritoimageTC->SetDivider(StringWidth(_T("Volume name"))+28); // "TVOLUMENAME"
	AddChild(eltoritoimageTC);
	SetViewColor((rgb_color){216,216,216,255});
}

BootableCDPanel::~BootableCDPanel() {
	delete filesystemMF;
	delete volumenameTC;
	delete eltoritoimageTC;
}

void BootableCDPanel::AllAttached() {
	mi->SetTarget(this, Looper());
	filesystemMF->SetDivider(StringWidth(_T("Volume name"))+30); // "TVOLUMENAME"
	filesystemMF->MoveTo(5,2);
	filesystemMF->ResizeTo(200, 22);
	volumenameTC->MoveTo(5,28);
	volumenameTC->ResizeTo(200, 20);
	eltoritoimageTC->MoveTo(5, 54);
	eltoritoimageTC->ResizeTo(200,20);
}

BList	*BootableCDPanel::GetFilesystem() {
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

	if (list->CountItems()==1) {
		if (((BString *)list->ItemAt(0))->Compare("BFS")==0) {
			ErrorBox *eb=new ErrorBox(E_ORANGE_COLOR,	_T("Note"), // "L:Note"
														_T("Bootable BFS discs are not supported by Helios. Please select another file system."), // "Error:BootableBFS"
														_T("Ok")); // "TOK"
			// { DEBUG
			/*	char debug[512];
				sprintf(debug, "Helios|BootableCDPanel::GetFilesystem()|%s",
						_T("Bootable BFS discs are not supported by Helios. Please select another file system.")); // "Error:BootableBFS"
				SYSLOG->SendSyslogMessage(1, 7, debug);
			*/// } DEBUG
			eb->Go();
		}
	}
	// { DEBUG
	/*	char debug[512];
		sprintf(debug, "Helios|BootableCDPanel::GetFilesystem()|");
		for (int i=0; i<list->CountItems(); i++)
			sprintf(debug, "%s[%s]", strdup(debug), ((BString *)list->ItemAt(i))->String());
		SYSLOG->SendSyslogMessage(1, 7, debug);
	*/// } DEBUG

	return list;
}

//const char	*BootableCDPanel::GetFileSystem() {
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

const char *BootableCDPanel::GetVolumeName() {
	return volumenameTC->Text();
}

void	BootableCDPanel::SetVolumeName(const char *name) {
	volumenameTC->SetText(name);
}

const char *BootableCDPanel::GetImageName() {
	return eltoritoimageTC->Text();
}

void	BootableCDPanel::SetImageName(const char *name) {
	eltoritoimageTC->SetText(name);
}

void	BootableCDPanel::MessageReceived(BMessage *msg) {
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
		case LISTITEM_DROPPED:
				int32	i;
				if (msg->FindInt32("index", &i)==B_OK) {
					BString str;
					const char *str2;
					
					// check for the correct file size (1440k, 2880k, 1200k)
					str.SetTo(((Application1 *)be_app)->current_folder.String());
					str.Append(((FolderRow *)(((Application1 *)be_app)->window1->view1->listview1->RowAt(i)))->GetFilename());
					BFile		*file=new BFile(str.String(), B_READ_ONLY);
					off_t		filesize=0;
					if (file->InitCheck()==B_OK) {
						file->GetSize(&filesize);
					}
					file->Unset();
					delete file;
					// wrong size?
					if ((filesize!=1440*1024) && (filesize!=2880*1024) && (filesize!=1200*1024)) {
						ErrorBox	*eb=new ErrorBox(E_RED_COLOR,	_T("Error"), // "TERROR"
																	_T("The selected floppy image file has an unsupported length. Valid floppy image sizes are 1200kB, 1440kB and 2880kB."), // "Error:Wrong Image Length"
																	_T("Ok")); // "TOK"
						eb->Go();
						break;
					}
					
					str.SetTo(((Application1 *)be_app)->current_folder.String());
					str.RemoveFirst(((Application1 *)be_app)->base_folder.String());
					str.Prepend("/");
					//str.RemoveAll("//");
					//if (str.Length()==0) str="/";
					
					str2=((FolderRow *)(((Application1 *)be_app)->window1->view1->listview1->RowAt(i)))->GetFilename();
					str.Append(str2);
					SetImageName(str.String());
				}
				break;
		default:	BView::MessageReceived(msg);
				break;
	}
}
