#include <Application.h>
#include <Button.h>

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
#include "ConfigFields.h"
#include "View1.h"

View1::View1(BRect rect)
 : BView(rect, "View1", B_FOLLOW_ALL_SIDES,B_WILL_DRAW)
{
	BString tmpstring;

	SetFontSize(10.0);

	font1 = new BFont(be_plain_font);
	box2 = new InfoBox();
	listview1 = new ListView1();
	textcontrol2 = new BTextControl(BRect(415, 55, 435+StringWidth(_T("Pause"))+8+30, 75), "tc",
						_T("Pause"), NULL, new BMessage(PAUSECHANGE_MSG),
						B_FOLLOW_RIGHT | B_FOLLOW_TOP, B_WILL_DRAW);
	textcontrol2->SetTarget(be_app_messenger);
	textcontrol2->SetDivider(StringWidth(_T("Pause"))+8);

	SetViewColor((rgb_color) {216,216,216,255});

	AddChild(box2);
	//AddChild(menufield2);
//	AddChild(listview1);
	AddChild(textcontrol2);
	SetViewColor((rgb_color) {216,216,216,255});

	// BURN NOW! BUTTON
	button1=new BButton(BRect(509,178,589,205),"v1_burnnow",_T("Burn now!"), // "TBURNNOW"
		new BMessage(BURN_MSG), B_FOLLOW_LEFT | B_FOLLOW_BOTTOM,B_WILL_DRAW);
	button1->SetTarget(be_app_messenger);
	BFont *buttonfont=new BFont(be_bold_font);
	buttonfont->SetSize(12);
	button1->SetFont(buttonfont);
	//AddChild(button1);
	//button1->SetEnabled(true);

/*	updateB=new BButton(BRect(5,178,264,74),"v1_updateB",_T("Update"), // "TUPDATE"
			new BMessage(UPDATEINFO_MSG),B_FOLLOW_LEFT | B_FOLLOW_BOTTOM,B_WILL_DRAW);
	updateB->SetTarget(be_app_messenger);
	updateB->SetFontSize(10);
	AddChild(updateB);
*/

	// main menu items
	mainMB=new BMenuBar(BRect(0,0,1,1), "mainMB");
	mainMB->SetTargetForItems(be_app_messenger);
	AddChild(mainMB);
	fileM=new BMenu(_T("File")); // "TMI_FILE"
	fileM->SetTargetForItems(be_app_messenger);
	mainMB->AddItem(fileM);
	imageM=new BMenu(_T("Image")); // "TMI_IMAGE"
	imageM->SetTargetForItems(be_app_messenger);
	mainMB->AddItem(imageM);
	copycdM=new BMenu(_T("Copy CD")); // TMI_COPYCD"
	copycdM->SetTargetForItems(be_app_messenger);
	mainMB->AddItem(copycdM);
	rwM=new BMenu(_T("Blank CD-RW")); // "TMI_CDRW"
	rwM->SetTargetForItems(be_app_messenger);
	mainMB->AddItem(rwM);
	windowM=new BMenu(_T("Window"));
	windowM->SetTargetForItems(be_app_messenger);
	mainMB->AddItem(windowM);
	//popupmenu2->SetLabelFromMarked(true);
	//menufield2 = new BMenuField(BRect(415, 25, 589, 45), "menufield", TFILESYSTEM, popupmenu2, new BMessage(FILESYSTEM_MSG), B_FOLLOW_LEFT | B_FOLLOW_TOP, B_WILL_DRAW);
//	menufield2->SetDivider(StringWidth(TFILESYSTEM)+8);
//	menuitem3->SetEnabled(false);

	


	// submenu-items for File-menu
	newprojectMI=new BMenuItem(_T("New project"), new BMessage(NEWPROJECT_MIMSG),'N'); // "TMI_NEWPROJECT"
	newprojectMI->SetTarget(be_app_messenger);
	fileM->AddItem(newprojectMI);
	tmpstring = "";
	tmpstring << _T("Open project") << B_UTF8_ELLIPSIS;  // "TMI_OPENPROJECT"
	openprojectMI=new BMenuItem( tmpstring.String(), new BMessage(OPENPROJECT_MIMSG),'O');
	openprojectMI->SetTarget(be_app_messenger);
	fileM->AddItem(openprojectMI);
	openrecentSM=new BMenu(_T("Open recent project")); // "TMI_OPENRECENT"
	fileM->AddItem(openrecentSM);
	
	fileM->AddSeparatorItem();
	saveprojectMI=new BMenuItem(_T("Save project"), new BMessage(SAVEPROJECT_MIMSG),'S'); // "TMI_SAVEPROJECT"
	saveprojectMI->SetTarget(be_app_messenger);
	fileM->AddItem(saveprojectMI);
	tmpstring = "";
	tmpstring << _T("Save project as") << B_UTF8_ELLIPSIS; // "TMI_SAVEPROJECTAS"
	saveprojectasMI=new BMenuItem( tmpstring.String(), new BMessage(SAVEPROJECTAS_MIMSG));
	saveprojectasMI->SetTarget(be_app_messenger);
	fileM->AddItem(saveprojectasMI);

	// SAVE AS STANDARD PROJECT
	//standardprojectMI=new BMenuItem(_T("TMI_STANDARDPROJECT"), new BMessage(STANDARDPROJECT_MIMSG));
	//standardprojectMI->SetTarget(be_app_messenger);
	//fileM->AddItem(standardprojectMI);
	// EXPORT SUB MENU
	exportSM=new BMenu(_T("Export")); // "TMI_EXPORT"
	fileM->AddItem(exportSM);
		// EXPORT AS M3U PLAYLIST
		export_M3U=new BMenuItem(_T("CL-Amp playlist"), new BMessage(EXPORT_M3U_MIMSG)); // "TMI_EXPORT_M3U"
		export_M3U->SetTarget(be_app_messenger);
		exportSM->AddItem(export_M3U);
		// EXPORT AS APML PLAYLIST
		export_APML=new BMenuItem(_T("APlayer playlist"), new BMessage(EXPORT_APML_MIMSG)); // "TMI_EXPORT_APML"
		export_APML->SetTarget(be_app_messenger);
		exportSM->AddItem(export_APML);
		// EXPORT AS PLAIN FILELIST
		export_FileList=new BMenuItem(_T("Plain filelist"), new BMessage(EXPORT_FILELIST_MIMSG)); // "TMI_EXPORT_FILELIST"
		export_FileList->SetTarget(be_app_messenger);
		exportSM->AddItem(export_FileList);
		// EXPORT AS TRACKER TEMPLATE
		//export_TrackerTemplate=new BMenuItem(_T("MI:Tracker Template"), new BMessage(EXPORT_TRACKERTEMPLATE_MIMSG));
		//export_TrackerTemplate->SetTarget(be_app_messenger);
		//exportSM->AddItem(export_TrackerTemplate);
	removeprojectMI=new BMenuItem(_T("Remove current project"), new BMessage(REMOVEPROJECT_MIMSG)); // "TMI_REMOVEPROJECT"
	removeprojectMI->SetTarget(be_app_messenger);
	fileM->AddItem(removeprojectMI);
	fileM->AddSeparatorItem();
//	if ( ((Application1 *)be_app)->CM->GetBool(INFOBAR_ENABLED) )
//		fileM->AddItem(new IBToggleItem());
	// PREFERENCES
	tmpstring = "";
	tmpstring << _T("Settings") << B_UTF8_ELLIPSIS; // "TMI_PREFERENCES"
	preferencesMI=new BMenuItem( tmpstring.String(), new BMessage(PREFERENCES_MIMSG),'P');
	preferencesMI->SetTarget(be_app_messenger);
	fileM->AddItem(preferencesMI);
	
// Removed minimise and zoom for now - why not rely on BeOS standard controls for this?
// (Commented out in case maurice wants 'em back! MSH21-08-04)
//	windowM->AddItem(new BMenuItem(_T("Minimize"), new BMessage(B_MINIMIZE)));
//	windowM->AddItem(new BMenuItem(_T("Zoom"), new BMessage(B_ZOOM)));
//	windowM->AddSeparatorItem();
	tmpstring = "";
	tmpstring << _T("mkisofs") << B_UTF8_ELLIPSIS; // "TMI_MKISOFSPROTOCOL"
	mkisofs_outputMI=new BMenuItem( tmpstring.String(), new BMessage(MKISOFSPROTOCOL_MIMSG));
	mkisofs_outputMI->SetTarget(be_app_messenger);
	windowM->AddItem(mkisofs_outputMI);
	tmpstring = "";
	tmpstring << _T("cdrecord") << B_UTF8_ELLIPSIS; // "TMI_CDRECORDPROTOCOL"
	cdrecord_outputMI=new BMenuItem( tmpstring.String(), new BMessage(CDRECORDPROTOCOL_MIMSG));
	cdrecord_outputMI->SetTarget(be_app_messenger);
	windowM->AddItem(cdrecord_outputMI);
	
	

	fileM->AddSeparatorItem();
	tmpstring = "";
	tmpstring << _T("About Helios") << B_UTF8_ELLIPSIS; // "TMI_CDRECORDPROTOCOL"
	aboutMI=new BMenuItem( tmpstring.String(),	new BMessage(B_ABOUT_REQUESTED));
	aboutMI->SetTarget(be_app_messenger);
	fileM->AddItem(aboutMI);
	fileM->AddSeparatorItem();
	exitMI=new BMenuItem(_T("Exit"), new BMessage(B_QUIT_REQUESTED),'Q'); // "TMI_EXIT"
	exitMI->SetTarget(be_app_messenger);
	fileM->AddItem(exitMI);

	// submenu-items for image-menu
	cdimgSM=new BMenu(_T("CD image")); // "M:CD Image"
	imageM->AddItem(cdimgSM);
	floppyimgSM=new BMenu(_T("Floppy image")); // "M:Floppy Image"
	imageM->AddItem(floppyimgSM);
	tmpstring = "";
	tmpstring << _T("Read CD image") << B_UTF8_ELLIPSIS; // "TMI_READCD"	
	readcdimageMI=new BMenuItem( tmpstring.String(), new BMessage(READCDIMAGE_MIMSG),'R');
	cdimgSM->AddItem(readcdimageMI);
	readcdimageMI->SetTarget(be_app_messenger);
	tmpstring = "";
	tmpstring << _T("Create image file") << B_UTF8_ELLIPSIS; // "TMI_CREATEFILEIMAGE"	
	createfileimageMI=new BMenuItem( tmpstring.String(), new BMessage(CREATEFILEIMAGE_MIMSG),'F');
	createfileimageMI->SetTarget(be_app_messenger);
	cdimgSM->AddItem(createfileimageMI);
	tmpstring = "";
	tmpstring << _T("Burn image file") << B_UTF8_ELLIPSIS; // "TMI_BURNIMAGEFILE"
	burnimagefileMI=new BMenuItem( tmpstring.String(), new BMessage(BURNIMAGEFILE_MIMSG),'B');
	burnimagefileMI->SetTarget(be_app_messenger);
	cdimgSM->AddItem(burnimagefileMI);
	tmpstring = "";
	tmpstring << _T("Read floppy image") << B_UTF8_ELLIPSIS; // "MI:Read Floppy Image"
	readfloppyimagefileMI=new BMenuItem( tmpstring.String(), new BMessage(READFLOPPYIMAGE_MIMSG));
	readfloppyimagefileMI->SetTarget(be_app_messenger);
	floppyimgSM->AddItem(readfloppyimagefileMI);
	
	cdimgSM->AddSeparatorItem();
	
	readaudiocdMI=new BMenuItem(_T("Read Audio CD"), new BMessage(READAUDIOCD_MIMSG)); // "MI:Read Audio CD"
	readaudiocdMI->SetTarget(be_app_messenger);
	cdimgSM->AddItem(readaudiocdMI);
	
	// submenu-item for cd-copy
	copyaudiocdMI=new BMenuItem(_T("Copy Audio CD"), new BMessage(COPYAUDIOCD_MIMSG)); // "TMI_COPYAUDIOCD"
	copyaudiocdMI->SetTarget(be_app_messenger);
	
	/////////////
	copyaudiocdMI->SetEnabled(true);
	/////////////
	
	copycdM->AddItem(copyaudiocdMI);
	copydatacdMI=new BMenuItem(_T("Copy Data CD"), new BMessage(COPYDATACD_MIMSG)); // "TMI_COPYDATACD"
	copydatacdMI->SetTarget(be_app_messenger);
	copycdM->AddItem(copydatacdMI);
	
	// submenu-tems for cd-rw
	fastblankMI=new BMenuItem(_T("Entire CD (fast)"), new BMessage(FASTBLANK_MIMSG)); // "TMI_FASTBLANK"
	fastblankMI->SetTarget(be_app_messenger);
	rwM->AddItem(fastblankMI);	
	completeblankMI=new BMenuItem(_T("Entire CD (complete)"), new BMessage(COMPLETEBLANK_MIMSG)); // "TMI_COMPLETEBLANK"
	completeblankMI->SetTarget(be_app_messenger);
	rwM->AddItem(completeblankMI);	
	unclosesessionMI=new BMenuItem(_T("Open last session"), new BMessage(UNCLOSESESSION_MIMSG)); // "TMI_UNCLOSESESSION"
	unclosesessionMI->SetTarget(be_app_messenger);
	rwM->AddItem(unclosesessionMI);	
	blanksessionMI=new BMenuItem(_T("Erase last session"), new BMessage(BLANKSESSION_MIMSG)); // "TMI_BLANKSESSION"
	blanksessionMI->SetTarget(be_app_messenger);
	rwM->AddItem(blanksessionMI);	
	
	
//	menuitem1 = new BMenuItem("ISO 9660", new BMessage(FILESYSTEM_MSG));
//	popupmenu2->AddItem(menuitem1);
//	menuitem2 = new BMenuItem("ISO 9660 long", new BMessage(FILESYSTEM_MSG));
//	popupmenu2->AddItem(menuitem2);
//	menuitem3 = new BMenuItem("BeOS bfs", new BMessage(FILESYSTEM_MSG));
//	popupmenu2->AddItem(menuitem3);
//	menuitem4 = new BMenuItem("MacOS hybrid", new BMessage(FILESYSTEM_MSG));
//	popupmenu2->AddItem(menuitem4);
//	menuitem5 = new BMenuItem("UNIX RockRidge", new BMessage(FILESYSTEM_MSG));
//	popupmenu2->AddItem(menuitem5);
//	menuitem6 = new BMenuItem("Windows Joliet", new BMessage(FILESYSTEM_MSG));
//	popupmenu2->AddItem(menuitem6);
//	menuitem6->SetMarked(true);
	float x1;
	float bottom;
	mainMB->GetPreferredSize(&x1,&menu_height);
	//listview1->MoveTo(0,y1+1);
//	listview1->ResizeTo(302-B_V_SCROLL_BAR_WIDTH,Frame().Height()-(stringview1->Frame().Height()+y1));
	//scrollview1 = new ScrollView1(listview1);
	listview1->MoveTo(0,menu_height+1);
	bottom=menu_height+1+listview1->Frame().Height()+1;

	//MoveTo(0,0);
//	stringview1->MoveTo(0,bottom);
//	stringview1->ResizeTo(Bounds().Width(), Bounds().Height()-bottom);
//	scrollview1->ResizeTo(302,Frame().Height()-(y1+stringview1->Frame().Height()));
	//AddChild(scrollview1);
	//AddChild(new ScrollViewCorner(scrollview1->Frame().right-B_V_SCROLL_BAR_WIDTH,scrollview1->Frame().bottom-B_H_SCROLL_BAR_HEIGHT));
	AddChild(listview1);

	CDTypeBOX=new BBox(BRect(5,10+menu_height, list_left-5,10+menu_height+135));
	AddChild(CDTypeBOX);
	
	// BMenuField which will be in the label view of CDTypeBOX
	// for selecting a specific cd-type
	CDTypeMF=new CDTypeMenu(BRect(0,0,130,16),new BMessage(DATAUDIO_MSG));
	
	
	CDTypeBOX->SetLabel(CDTypeMF);
	
	
	statusBAR=new StatusBar(BRect(0, 0, 1, 1), "v1_statusBAR", B_FOLLOW_LEFT|B_FOLLOW_BOTTOM, B_WILL_DRAW);
	AddChild(statusBAR);
}

View1::~View1(void) {

}

void View1::AllAttached(void) {
	ResizeTo(Window()->Bounds().Width(), Window()->Bounds().Height());
	
//	stringview1->ResizeTo(Window()->Bounds().Width(), 15);
//	stringview1->MoveTo(0, Window()->Bounds().Height()-15);
	list_left=Bounds().Width()*(2.0/5.0);
	listview1->MoveTo(list_left+1,menu_height+1+31);
	listview1->ResizeTo(Bounds().Width()-list_left,  Bounds().Height()-menu_height-31);

	mainBAR=new IconBar(BRect(0,0,list_left, 30), "v2_iconBAR", B_FOLLOW_LEFT|B_FOLLOW_TOP);
	((BButton *)mainBAR->ChildViewAt(mainBAR->AddButton("New Project", "bar_newproject", new BMessage(NEWPROJECT_MIMSG))))->SetTarget(be_app_messenger);
	((BButton *)mainBAR->ChildViewAt(mainBAR->AddButton("Open Project", "bar_openproject", new BMessage(OPENPROJECT_MIMSG))))->SetTarget(be_app_messenger);
	((BButton *)mainBAR->ChildViewAt(mainBAR->AddButton("Save Project", "bar_saveproject", new BMessage(SAVEPROJECT_MIMSG))))->SetTarget(be_app_messenger);
//	((BButton *)mainBAR->ChildViewAt(mainBAR->AddButton("Save Project As", "", new BMessage(SAVEPROJECTAS_MIMSG))))->SetTarget(be_app_messenger);
	mainBAR->AddChildView(new IconBarSeparator(BRect(0,0,1,22), "separator"));
	((BButton *)mainBAR->ChildViewAt(mainBAR->AddButton(5, "bar_preferences", new BMessage(PREFERENCES_MIMSG))))->SetTarget(be_app_messenger);
	mainBAR->AddChildView(new IconBarSeparator(BRect(0,0,1,22), "separator"));
	((BButton *)mainBAR->ChildViewAt(mainBAR->AddButtonMSGG("Burn Now", "bar_burnnow", new BMessage(BURN_MSG))))->SetTarget(be_app_messenger);
	mainBAR->AddChildView(new IconBarSeparator(BRect(0,0,1,22), "separator"));
	AddChild(mainBAR);
	mainBAR->MoveTo(0, menu_height+1);
	mainBAR->ResizeTo(list_left, 30);

	iconBAR=new IconBar(BRect(0,0,listview1->Bounds().Width(), 30), "v1_iconBAR", B_FOLLOW_LEFT_RIGHT|B_FOLLOW_TOP);
//	((BButton *)iconBAR->ChildViewAt(iconBAR->AddButton("New Folder", "bar_newfolder", new BMessage('mm12'))))->SetTarget(be_app_messenger);
	((BButton *)iconBAR->ChildViewAt(iconBAR->AddButton("New Folder", "bar_newfolder", new BMessage(NEW_FOLDER_MSG))))->SetTarget(be_app_messenger);
	((BButton *)iconBAR->ChildViewAt(iconBAR->AddButton(1, "bar_deleteitem", new BMessage(DELETE_FILE_MSG))))->SetTarget(be_app_messenger);
	iconBAR->AddChildView(new IconBarSeparator(BRect(0,0,1,22), "separator"));
	iconBAR->AddChildView((IBpathDisplayTC=new BTextControl(BRect(0,0,222,20), "bar_path", "", "[no path]", NULL, B_FOLLOW_LEFT_RIGHT|B_FOLLOW_TOP_BOTTOM)));
	((BButton *)iconBAR->ChildViewAt(iconBAR->AddButton(2, "bar_parent", new BMessage(GOTOPARENT_MSG), B_FOLLOW_RIGHT|B_FOLLOW_TOP)))->SetTarget(be_app_messenger);
	iconBAR->AddChildView(new IconBarSeparator(BRect(0,0,1,22), "separator", B_FOLLOW_RIGHT|B_FOLLOW_TOP));
	((BButton *)iconBAR->ChildViewAt(iconBAR->AddButton(4, "bar_update", new BMessage(UPDATEINFO_MSG), B_FOLLOW_RIGHT|B_FOLLOW_TOP)))->SetTarget(be_app_messenger);
	AddChild(iconBAR);
	iconBAR->MoveTo(list_left+1, menu_height+1);
	iconBAR->ResizeTo(listview1->Bounds().Width()-1, 30);
	
	IBpathDisplayTC->SetDivider(-3);
	IBpathDisplayTC->TextView()->MakeEditable(false);

	// cdtype box (the one with the menu in the title view)
	CDTypeBOX->ResizeTo(list_left-11, 105);
	CDTypeBOX->MoveTo(5,7+menu_height+30);
	// info box
	box2->MoveTo(5,CDTypeBOX->Frame().bottom+7);
	box2->ResizeTo(list_left-11, box2->Bounds().Height());
	// "burn now!" button
	button1->ResizeTo(list_left/2, 28);
	button1->MoveTo(list_left-(5+list_left/2),box2->Frame().bottom+5);
	
/*	updateB->ResizeTo(list_left/4, 28);
	updateB->MoveTo(4,box2->Frame().bottom+5);
*/	

	statusBAR->ResizeTo(list_left, B_H_SCROLL_BAR_HEIGHT-1);
	statusBAR->MoveTo(0, Bounds().Height()-statusBAR->Bounds().Height());

	BRect	innerView=CDTypeBOX->Bounds();
	innerView.InsetBy(7.0F, 8.0F);
	innerView.SetLeftTop(BPoint(6.0F,23.0F));
	audiopanel=new AudioCDPanel(innerView);
	datapanel=new DataCDPanel(innerView);
	bootablepanel=new BootableCDPanel(innerView);
	cdextrapanel=new CDExtraPanel(innerView);
	
#ifdef INCLUDE_DVD
	dvdvideopanel=new VideoDVDPanel(innerView);
#endif

	CDTypeMF->GetMenu()->SetTargetForItems(be_app_messenger);
};


void View1::Draw(BRect updateRect) {
	rgb_color	high=HighColor();
	
	SetHighColor((rgb_color){150,150,150,255});
	StrokeLine(BPoint(list_left,0),BPoint(list_left, statusBAR->Frame().top-1),B_SOLID_HIGH);
	//StrokeLine(BPoint(0, statusBAR->Frame().top-1), BPoint(list_left, statusBAR->Frame().top-1));
	SetHighColor((rgb_color){255,255,255,255});
	StrokeLine(BPoint(0,iconBAR->Frame().bottom+1),BPoint(list_left-1, iconBAR->Frame().bottom+1),B_SOLID_HIGH);
	StrokeLine(BPoint(0, iconBAR->Frame().bottom+1), BPoint(0, statusBAR->Frame().top-1));
	SetHighColor(high);
}

void	View1::UpdatePanels() {
	if (audiopanel->Window()!=NULL) CDTypeBOX->RemoveChild(audiopanel);
	if (datapanel->Window()!=NULL) CDTypeBOX->RemoveChild(datapanel);
	if (bootablepanel->Window()!=NULL) CDTypeBOX->RemoveChild(bootablepanel);
	if (cdextrapanel->Window()!=NULL) CDTypeBOX->RemoveChild(cdextrapanel);
#ifdef INCLUDE_DVD
	if (dvdvideopanel->Window()!=NULL) CDTypeBOX->RemoveChild(dvdvideopanel);
#endif
	if (CDType()==BOOTABLECD_INDEX) CDTypeBOX->AddChild(bootablepanel);
	if (CDType()==AUDIOCD_INDEX) CDTypeBOX->AddChild(audiopanel);
	if (CDType()==DATACD_INDEX) CDTypeBOX->AddChild(datapanel);
	if (CDType()==CDEXTRA_INDEX) CDTypeBOX->AddChild(cdextrapanel);

#ifdef INCLUDE_DVD
	if (CDType()==DVDVIDEO_INDEX) CDTypeBOX->AddChild(dvdvideopanel);
#endif
}

int	View1::CDType() {
	return CDTypeMF->GetMenu()->IndexOf(CDTypeMF->GetMenu()->FindMarked());
}

void	View1::SetCDType(int32 index) {
	CDTypeMF->GetMenu()->ItemAt(index)->SetMarked(true);
	UpdatePanels();
	// CDTypeBOX->Invalidate();
}

const char *View1::GetVolumeName() {
	return datapanel->GetVolumeName();
}

const char *View1::GetVolumeNameBootCD() {
	return bootablepanel->GetVolumeName();
}

bool	View1::IsMultisession() {
	return datapanel->IsMultisession();
}

void	View1::SetMultisession(bool flag) {
	datapanel->SetMultisession(flag);
}

int32	View1::GetFilesystem() {
	return datapanel->filesystemPUM->IndexOf(datapanel->filesystemPUM->FindMarked());
}

void	View1::SetFilesystem(int32 index) {
	datapanel->filesystemPUM->ItemAt(index)->SetMarked(true);
}

void 	View1::SetVolumeName(const char *name) {
	datapanel->SetVolumeName(name);
}

void 	View1::SetVolumeNameBootCD(const char *name) {
	bootablepanel->SetVolumeName(name);
}

int32	View1::GetFilesystemBootCD() {
	return bootablepanel->filesystemPUM->IndexOf(bootablepanel->filesystemPUM->FindMarked());
}

void	View1::SetFilesystemBootCD(int32 index) {
	if (index<bootablepanel->filesystemPUM->CountItems())
		bootablepanel->filesystemPUM->ItemAt(index)->SetMarked(true);
	else
		bootablepanel->filesystemPUM->ItemAt(bootablepanel->filesystemPUM->CountItems()-1)->SetMarked(true);
}


bool	View1::WriteCDText() {
	return audiopanel->writeCDText();
}

void	View1::SetCDText(bool flag) {
	audiopanel->SetCDText(flag);
}

bool	View1::ProtectAudioCD() {
	return audiopanel->copyProtection();
}

void	View1::SetProtection(bool flag) {
	audiopanel->SetProtection(flag);
}

void View1::SetCDAlbum(const char *name) {
	audiopanel->SetCDAlbum(name);
}

const char *View1::GetCDAlbum() {
	return audiopanel->GetCDAlbum();
}

// BOOTABLE CD


void 	View1::SetImageNameBootCD(const char *name) {
	bootablepanel->SetImageName(name);
}

const char *View1::GetImageNameBootCD() {
	return bootablepanel->GetImageName();
}


// CD EXTRA

void 	View1::SetImageNameCDExtra(const char *name) {
	cdextrapanel->SetImageName(name);
}

const char *View1::GetImageNameCDExtra() {
	return cdextrapanel->GetImageName();
}

bool	View1::WriteCDTextCDExtra() {
	return cdextrapanel->writeCDText();
}

void	View1::SetCDTextCDExtra(bool flag) {
	cdextrapanel->SetCDText(flag);
}

void View1::SetCDAlbumCDExtra(const char *name) {
	cdextrapanel->SetCDAlbum(name);
}

const char *View1::GetCDAlbumCDExtra() {
	return cdextrapanel->GetCDAlbum();
}

int32	View1::GetFilesystemCDExtra() {
	return cdextrapanel->filesystemPUM->IndexOf(cdextrapanel->filesystemPUM->FindMarked());
}

void	View1::SetFilesystemCDExtra(int32 index) {
	cdextrapanel->filesystemPUM->ItemAt(index)->SetMarked(true);
}

const char *View1::GetVolumeNameCDExtra() {
	return cdextrapanel->GetVolumeName();
}

void 	View1::SetVolumeNameCDExtra(const char *name) {
	cdextrapanel->SetVolumeName(name);
}


// VIDEO DVD

#ifdef INCLUDE_DVD
const char *View1::GetVolumeNameDVDVideo() {
	return dvdvideopanel->GetVolumeName();
}

void 	View1::SetVolumeNameDVDVideo(const char *name) {
	dvdvideopanel->SetVolumeName(name);
}

int32	View1::GetFilesystemDVDVideo() {
	return dvdvideopanel->filesystemPUM->IndexOf(dvdvideopanel->filesystemPUM->FindMarked());
}

void	View1::SetFilesystemDVDVideo(int32 index) {
	dvdvideopanel->filesystemPUM->ItemAt(index)->SetMarked(true);
}
#endif






// ICONBAR STUFF

void View1::SetCurrentPath(const char *text) {
	IBpathDisplayTC->SetText(text);
}







