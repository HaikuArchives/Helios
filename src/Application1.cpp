#ifndef _ZETA_USING_DEPRECATED_API_
#define _ZETA_USING_DEPRECATED_API_
#endif
	// To allow PostMessage() to work. Grrr...

#if defined(_BEOS_R5_BUILD_) || defined(_BEOS_HAIKU_BUILD_)
#include "MSHLanguageMgr.h"
extern MSHLanguageMgr* gMSHLangMgr;
#define _T(str)	gMSHLangMgr->_T(str).String()
#define _TPS(str)	gMSHLangMgr->_T(str)
#else
#include <locale/Locale.h>
#endif

#include <app/Roster.h>
#include <AppFileInfo.h>
#include <Beep.h>
#include <be/support/UTF8.h>
#include <errno.h>
#include <FilePanel.h>
#include <fs_attr.h>
#include <MediaFile.h>
#include <MediaFormats.h>
#include <MediaTrack.h>
#include <support/Debug.h>
#include <support/ClassInfo.h>
#include <sys/types.h>
#include <storage/FindDirectory.h>

#include "Application1.h"
#include "AboutWindow.h"
#include "AudioRow.h"
#include "BitmapMenuItem.h"
#include "CDRECORDCmdLine.h"
#include "CDText/CDText.h"
#include "ColumnTypes.h"
#include "ConfigFields.h"
#include "DebugTools.h"
#include "Defines.h"
#include "ErrorBox.h"
#include "FileChooser.h"
#include "FileDialog.h"
#include "FileUtils.h"
#include "FolderRow.h"
#include "Globals.h"
#include "InfoFile.h"
#include "MakeBFSImage.h"
#include "MKISOFSCmdLine.h"
#include "MString.h"
#include "PipeCmd.h"
#include "VersionControl.h"

#ifdef _BEOS_R5_BUILD_
#define APP_VERSION "1.71b2 R5"
#else
#ifdef _BEOS_HAIKU_BUILD_
#define APP_VERSION "1.71b2 Haiku"
#else
#ifdef _SUTTER_TEST_VERSION_
#define APP_VERSION "1.71b2 Sutter Test Build - DO NOT RELEASE"
#else
#define APP_VERSION "1.71b2 Zeta"
#endif
#endif
#endif

sem_id quitandsave_sem;

int32
QuitAndSaveThread(void *data)
{
	acquire_sem(quitandsave_sem);
	((Application1 *)data)->PostMessage(B_QUIT_REQUESTED);
	return 0;
}


void
Application1::AboutRequested(void)
{
	app_info	info;
	AboutWindow	*aboutW = new AboutWindow(BRect(100,100,420,260), "About");
	BScreen	*screen = new BScreen();

	aboutW->MoveTo((int)((screen->Frame().Width()-aboutW->Bounds().Width())/2), (int)((screen->Frame().Height()-aboutW->Bounds().Height())/2));
	delete screen;
	if (GetAppInfo(&info)==B_OK) {
		BBitmap bmp(BRect(0,0,31,31), B_CMAP8);
		if (BNodeInfo::GetTrackerIcon(&info.ref, &bmp, B_LARGE_ICON)==B_OK) {
			aboutW->SetIcon(&bmp);
		}
	}
	aboutW->SetApplicationName(_T("Helios")); // "ABOUT_APPLICATIONNAME"
	aboutW->SetVersionNumber(APP_VERSION); // "ABOUT_VERSIONNUMBER"
	BString copyrightStr(_T("Copyright")); // "ABOUT_COPYRIGHTSTRING"
	copyrightStr += " ";	// Initial space - please leave.
	copyrightStr += "Maurice Michalski, Mark Hogben";
	aboutW->SetCopyrightString(copyrightStr.String());

	BString aboutTextStr = _T("Helios is freeware. Many thanks to (in alphabetical order):");	// "ABOUT_TEXT"
	aboutTextStr += " ";	// Initial space - please leave.
	aboutTextStr += "Bruno G. Albuquerque, Stephan Aßmus, Rémy Hessels, ";
	aboutTextStr += "Philippe Houdoin, Cedric Maison, Atilla Ötztürk, ";
	aboutTextStr += "Gregor Rosenauer, Otto Schmälzle, Shard, Stefano, Sutter, ";
	aboutTextStr +=  "Jens Tinz, Mattia Tristo, yellowTAB Team, Bihari Zsolt.";
	aboutW->SetText(aboutTextStr.String());

	aboutW->Show();
}


// check whether there is a project loaded or not and create one if not.
bool
Application1::IsProject(const bool show_alert = true)
{
	const bool projectExists = FileExists(base_folder.String());
	if (!projectExists) {
		be_app->PostMessage(NEWPROJECT_MIMSG);
	}
	return projectExists;
}


void
Application1::SetHelpBubbles()
{
//	extern BubbleHelper *HLP;

//	HLP->SetHelp(window1->view1->FindView("bar_newproject"), 	_T("New project")); // "TIP:B:New Project"
//	HLP->SetHelp(window1->view1->FindView("bar_openproject"), 	_T("Open project")); // "TIP:B:Open Project"
//	HLP->SetHelp(window1->view1->FindView("bar_saveproject"), 	_T("Save project")); // "TIP:B:Save Project"
//	HLP->SetHelp(window1->view1->FindView("bar_preferences"), 	_T("Preferences")); // "TIP:B:Preferences"
//	HLP->SetHelp(window1->view1->FindView("bar_burnnow"),		_T("Burn!")); // "TIP:B:Burn Disc"
//	HLP->SetHelp(window1->view1->FindView("bar_newfolder"), 	_T("New folder")); // "TIP:B:New Folder"
//	HLP->SetHelp(window1->view1->FindView("bar_deleteitem"), 	_T("Delete selected item")); // "TIP:B:Delete Item"
//	HLP->SetHelp(window1->view1->FindView("bar_path"), 			_T("Current path")); // "TIP:TC:Current Virtual Path"
//	HLP->SetHelp(window1->view1->FindView("bar_parent"), 		_T("Parent folder")); // "TIP:B:Parent Folder"
//	HLP->SetHelp(window1->view1->FindView("bar_update"), 		_T("Refresh")); // "TIP:B:Refresh"
//	HLP->EnableHelp();
}


Application1::Application1() :
	BApplication("application/x-HeliosBurner.application"),
	CM(NULL)
{
	ASSERT(streambuffer_sem = create_sem(1, "streambuffer_sem"));
	quitandsave_sem = create_sem(0, "quitandsave_sem");
	fileadder_sem = create_sem(1, "fileadder_sem");
	fileremover_sem = create_sem(1, "fileremover_sem");
	filename_col = 0;
	save_and_quit = false;
	project_has_changed = false;
	is_all_showing = false;

	bool exit = false;

	CM = new ConfigManager("/boot/home/config/settings/Helios.settings", true);

//	extern BubbleHelper *HLP;
//	HLP=new BubbleHelper();

#if defined(_BEOS_R5_BUILD_) || defined(_BEOS_HAIKU_BUILD_)
	// Load language manager
	extern MSHLanguageMgr* gMSHLangMgr;
	gMSHLangMgr = new MSHLanguageMgr();
	gMSHLangMgr->LoadLanguageFiles("Language/Dictionaries/Helios");

	BString currLangTag = "enUS";
	if (CM->HasData(CURRENT_LANG_TAG)) {
		currLangTag = *CM->GetString(CURRENT_LANG_TAG);
	}
	gMSHLangMgr->SelectLanguageByName(currLangTag);
#endif

	fStatusWindow = new StatusWindow();

	window1 = new Window1();
	window2 = new Window2();
	configW = new ConfigWindow(_T("Settings")); // "TSETTINGSWINDOWTITLE"

	BRect windowRect = configW->GetConfigViewFrame();
	BRect windowSizeRect(0, 0, windowRect.Width(), windowRect.Height());

	audioW = NULL;
	configW->AddTopic(		_T("Burning")); 															// "OLV_GENERALOPTIONS"
	configW->AddConfigView(	_T("Cdrecord switches"), 	(heliosCV =			new HeliosConfigView(windowSizeRect))); 	// "OLV_RECORDING"
	configW->AddConfigView(	_T("Devices"), 				(deviceCV =			new DeviceConfigView(windowSizeRect))); 	// "OLV_DEVICES"
	configW->AddConfigView(	_T("Files & Folders"), 		(pathCV =			new PathConfigView(windowSizeRect))); 	// "OLV_FILESANDFOLDERS"
	configW->AddConfigView(	_T("Images"), 				(imageCV =			new ImageConfigView(windowSizeRect))); 	// "OLV_IMAGES"
	configW->AddConfigView(	_T("Filesystem"), 			(fspanelCV =		new FSPanel(windowSizeRect))); 			// "OLV_FILESYSTEM"
	configW->AddTopic(		_T("Project")); 															// "OLV_PROJECTOPTIONS"
	configW->AddConfigView(	_T("New project"), 			(standardCV =		new StandardConfigView(windowSizeRect))); // "OLV_NEWPROJECT"
	configW->AddTopic(		_T("Application")); 														// "OLV_APPLICATIONOPTIONS"
	configW->AddConfigView(	_T("Timeouts"), 			(applicationCV =	new ApplicationConfigView(windowSizeRect))); // "OLV_TIMEOUTS"
	configW->AddConfigView(	_T("Sounds"), 				(soundCV =			new SoundConfigView(windowSizeRect))); 	// "OLV_SOUND"
	configW->AddConfigView(	_T("General"), 				(saveCV = 			new SaveConfigView(windowSizeRect))); 	// "OLV_GENERAL"
	configW->AddConfigView(	_T("Colors"), 				(colorCV =			new ColorConfigView(windowSizeRect))); 	// "OLV_COLORS"
#if defined(_BEOS_R5_BUILD_) || defined(_BEOS_HAIKU_BUILD_)
	langCV = new LanguageConfigView(windowSizeRect, currLangTag);
	configW->AddConfigView( _T("Language"), langCV);
#endif
	configW->AddConfigView(	_T("Version info"), 		(versionCV =		new VersionConfigView(windowSizeRect))); 	// "OLV_VERSION"
	cdrecord_output=new ProtocolWindow(BRect(0,0,50,50), "cdrecord", B_DOCUMENT_WINDOW, -1);
	mkisofs_output=new ProtocolWindow(BRect(0,0,50,50), "mkisofs", B_DOCUMENT_WINDOW, -1);

	ReadSettings();
	SetHelpBubbles();
	
	// check for command line tools
	BString		string;
	// mkisofs
	string=CheckCommandlineTool(CM->GetString(PATH_TO_MKISOFS)->String(), "mkisofs");
	if (string.Length()!=0) CM->SetString(PATH_TO_MKISOFS, &string);
	else exit=true;
	// cdrecord
	string=CheckCommandlineTool(CM->GetString(PATH_TO_CDRECORD)->String(), "cdrecord");
	if (string.Length()!=0) CM->SetString(PATH_TO_CDRECORD, &string);
	else exit=true;
	// readcd
	string=CheckCommandlineTool(CM->GetString(PATH_TO_READCD)->String(), "readcd");
	if (string.Length()!=0) CM->SetString(PATH_TO_READCD, &string);
	else exit=true;
	// cdda2wav
	string=CheckCommandlineTool(CM->GetString(PATH_TO_CDDA2WAV)->String(), "cdda2wav");
	if (string.Length()!=0) CM->SetString(PATH_TO_CDDA2WAV, &string);
	else exit=true;
#ifdef _BEOS_HAIKU_BUILD_
	// mkfs	
	string=CheckCommandlineTool(CM->GetString(PATH_TO_MKBFS)->String(), "mkfs");
	if (string.Length()!=0) CM->SetString(PATH_TO_MKBFS, &string);
	else exit=true;
#else
	// mkbfs	
	string=CheckCommandlineTool(CM->GetString(PATH_TO_MKBFS)->String(), "mkbfs");
	if (string.Length()!=0) CM->SetString(PATH_TO_MKBFS, &string);
	else exit=true;
#endif

#ifdef _SUTTER_TEST_VERSION_
	ErrorBox *alert=new ErrorBox(E_RED_COLOR, "",	"WARNING: This is a pre-release test for Mr Sutter - DO NOT RELEASE. There will be NO support of any kind for this version. Do you agree to use it?",
																								_T("Cancel"),
																								_T("I Agree!"));
	const int agreed = alert->Go();
	if (agreed == 0) {
		exit = true;
	}
#endif

	if (!exit)  {
		ShowRecentProjects();
		if ((CM->GetString(PATH_TO_LAST_PROJECT)->Length()>0) && (saveCV->IsSavedWhenQuit())) {
			BEntry	*entry=new BEntry(CM->GetString(PATH_TO_LAST_PROJECT)->String(), true);
			if (entry->Exists()) {
				LoadProject(entry);
				ShowFolder();
			} else
				PostMessage(NEWPROJECT_MIMSG);
			delete entry;
		} else {
			PostMessage(NEWPROJECT_MIMSG);
		}

		if (window1->Lock()) {
			window1->view1->UpdatePanels();
			window1->Unlock();
		}
	
		
		// starts the BLooper() of configW
		if (configW->Lock()) {
			float	left=configW->Frame().left;
			float	top=configW->Frame().top;
			configW->MoveTo(-10000, -10000);
			configW->Show();
			
			// select the first view in the prefs instead of the last one
			// unsafe but should go ok
			configW->SelectView( 1 );
			configW->Hide();
			configW->MoveTo(left, top);
			configW->Unlock();
		}

		if (mkisofs_output->Lock()) {
			float	left=mkisofs_output->Frame().left;
			float	top=mkisofs_output->Frame().top;
			mkisofs_output->MoveTo(-10000, -10000);
			mkisofs_output->Show();
			mkisofs_output->Hide();
			mkisofs_output->MoveTo(left, top);
			mkisofs_output->Unlock();
		}
	
		if (cdrecord_output->Lock()) {
			float	left=cdrecord_output->Frame().left;
			float	top=cdrecord_output->Frame().top;
			cdrecord_output->MoveTo(-10000, -10000);
			cdrecord_output->Show();
			cdrecord_output->Hide();
			cdrecord_output->MoveTo(left, top);
			cdrecord_output->Unlock();
		}
	
	// Add Tooltips...
//	window1TT=new ToolTip(window1, "window1TT");
//	window1TT->SetText(window1->view1->iconBAR->ChildViewAt("bar_update"), "Update display");
//	window1TT->SetText(window1->view1->mainBAR->ChildViewAt("bar_burnnow"), "Burn CD/DVD");
//	HLP=new BubbleHelper();
//	HLP->SetHelp(window1->view1->iconBAR->ChildViewAt("bar_update"), "Update display");
//	HLP->EnableHelp(true);

		FileDialog::fmainWindow = window1;
		window1->Show();
	}

	just_quit = exit;
	if (exit) {
		PostMessage(B_QUIT_REQUESTED);
	}
}


Application1::~Application1(void)
{
#if defined(_BEOS_R5_BUILD_) || defined(_BEOS_HAIKU_BUILD_)
	extern MSHLanguageMgr* gMSHLangMgr;
	delete gMSHLangMgr;
	gMSHLangMgr = NULL;
#endif

	delete_sem(streambuffer_sem);
	delete_sem(quitandsave_sem);
	delete_sem(fileadder_sem);
	delete_sem(fileremover_sem);
}


void
Application1::ReadyToRun()
{
	BString		str;
	app_info	ai;

	if (configW->Lock()) {
		versionCV->SetCDRecordVersion(VersionControl::GetToolVersion(CM->GetString(PATH_TO_CDRECORD)->String()), GetIcon(CM->GetString(PATH_TO_CDRECORD)->String()));
		versionCV->SetReadCDVersion(VersionControl::GetToolVersion(CM->GetString(PATH_TO_READCD)->String()), GetIcon(CM->GetString(PATH_TO_READCD)->String()));
		versionCV->SetMkISOfsVersion(VersionControl::GetToolVersion(CM->GetString(PATH_TO_MKISOFS)->String()), GetIcon(CM->GetString(PATH_TO_MKISOFS)->String()));
		versionCV->SetCDDA2WAVVersion(VersionControl::GetToolVersion(CM->GetString(PATH_TO_CDDA2WAV)->String()), GetIcon(CM->GetString(PATH_TO_CDDA2WAV)->String()));

		str=_T("Helios"); // "ABOUT_APPLICATIONNAME"
		str << " " << APP_VERSION; // "ABOUT_VERSIONNUMBER"
		GetAppInfo(&ai);
		versionCV->SetHeliosVersion(str.String(), GetIcon(&ai.ref));
		configW->Unlock();
	}

	// create project folder if it does not exist
	BString	projectFolderPath = pathCV->GetProjectPath();
	AppendTrailingSlash(&projectFolderPath);
	projectFolderPath.Append(STANDARDPROJECTPATH);

	BDirectory HeliosDir;
	if (HeliosDir.SetTo(projectFolderPath.String()) != B_OK) {
		create_directory(projectFolderPath.String(), 0777);
	}
}


bool
Application1::QuitRequested(void)
{
	if (NULL != applicationCV) {
		if (applicationCV->IsAutoUpdating()) {
			if (window1->Lock()) {
				window1->view1->box2->ExitUpdateThread();
				window1->Unlock();
			}
		}
	}

	SaveSettings();

	while ((save_and_quit) && (project_has_changed)) {
		snooze(50000);
 	};

	if (!just_quit) {
		switch(saveCV->GetExitMode()) {
			case EM_SAVE_PROJECT: {
				if ((project_has_changed) && (IsProject(false))) {
					resume_thread(spawn_thread(QuitAndSaveThread, "QuitAndSaveThread", 1, this));
					be_app->PostMessage(SAVEPROJECT_MIMSG);
					return false;
				}
				break;
			}
			
			case EM_ASK_USER: {
				if ((project_has_changed) && (IsProject(false))) {
					ErrorBox	*eb=new ErrorBox(E_BLUE_COLOR, "",	_T("Save changes of current project?"), // "TASKFORSAVINGCHANGES"
																	_T("Yes"),
																	_T("No"),
																	_T("Cancel")); 
					eb->SetShortcut(2, B_ESCAPE);
					switch (eb->Go()) {
						case 0: {
							resume_thread(spawn_thread(QuitAndSaveThread, "QuitAndSaveThread", 1, this));
							be_app->PostMessage(SAVEPROJECT_MIMSG);
							return false;
						}
						
						case 1: {
							break;
						}
						
						case 2: {
							return false;
						}
					}
				}
			}
			
			case EM_DO_NOTHING: {
				break;
			}
		}
	}

	delete CM;

	BEntry *entry = new BEntry(pathCV->GetDefaultImageName(), true);
	if (entry->InitCheck() == B_OK) {
		entry->Remove();
		entry->Unset();
	}
	delete entry;
	
	if (configW->Lock()) configW->Quit();
	if (cdrecord_output->Lock()) cdrecord_output->Quit();
	if (mkisofs_output->Lock()) mkisofs_output->Quit();
	if (window2->Lock()) window2->Quit();
	if (window1->Lock()) window1->Quit();

	return true;
}


void
Application1::SetWorking(bool state) {
	static int count=0;

	if (state) {
		count++;
	}	else {
		count--;
	}

	if ((count==1) || (count==0)) {
		if (window1->Lock()) {
			((BButton *)window1->view1->iconBAR->ChildViewAt("bar_update"))->SetEnabled(!state);
			((BButton *)window1->view1->mainBAR->ChildViewAt("bar_burnnow"))->SetEnabled(!state);
			window1->Unlock();
		}
	}
}

void
Application1::MessageReceived(BMessage *message)
{
	if (applicationCV->IsAutoUpdating()) {
		window1->view1->box2->StopUpdating();
	}

	switch (message->what) {

		case FINISHED_EDITING_MSG: {
			void *pointer;

			message->FindPointer("row", &pointer);

			FolderRow *selected = (FolderRow *)(pointer);

			if (selected == NULL) {
				return;
			}

			window1->Lock();
			if (BString(selected->GetFilename()).Compare("..")!=0) {	     		
				BEntry file;
				BString str = "";
				str << current_folder.String() << selected->GetFilename();

				if (file.SetTo(str.String(),false)==B_OK) {
					str="";
				  str << current_folder.String() << ((BFileField *)selected->GetField(FILENAME_FIELD))->String();
				  file.Rename(str.String(),false);
				  selected->SetFilename(((BFileField *)selected->GetField(FILENAME_FIELD))->String());
					project_has_changed=true;
				 }
				file.Unset();
			}

			window1->Unlock();
			break;
		}

		case READAUDIOCD_MIMSG: {
			status_t error=B_OK;
			
			error=ReadAudioCD();
			
			if (error<0) {
				ErrorBox	*eb=new ErrorBox(E_RED_COLOR, errtype.String(), errstring.String(), _T("Ok")); // "TOK"
				PlayErrorSound();
				eb->Go();
			}
			break;
		}


		case B_SILENT_RELAUNCH: {
			if (window1->Lock()) {
				window1->view1->statusBAR->SetText(_T("Helios can be launched only once."), 4000000); // "L:Launch once"
				window1->Unlock();
			}
			break;
		}

		case B_SIMPLE_DATA: {
			resume_thread(spawn_thread(addFiles, "File Adder", 10, new BMessage(*message)));
	 		project_has_changed = true;
			break;
		}
		
		case 'mm01': // update display
		{
			window1->view1->box2->Refresh();
			beep();
	
			break;
		}

		case PREFERENCES_MIMSG:	{
			configW->Lock();
			if (configW->IsHidden()) {
				configW->Show();
			} else {
				configW->Activate();
			}
			configW->Unlock();
			break;
		}

		case DELETE_FILE_MSG: // remove selected items
		{
			window1->view1->listview1->EndEditing();	// MSH: Avoids certain crashes :-)
			resume_thread(spawn_thread(removeFiles, "File Remover", 10, new BMessage(*message)));
	 		project_has_changed = true;
			break;
		}

		case MOVE_UP_MSG: {
			MoveUp();
			UpdateTrackNumbers();
			break;
		}

		case MOVE_DOWN_MSG:	{
			MoveDown();
			UpdateTrackNumbers();
			break;
		}
		
		case 'mm40': {
			break;
		}


		// this message contains information about the indices of an audio track
		case AUDIOWINDOW_RETURN_MSG: {
			audioW = NULL;
			break;
		}
		
		case GOTOPARENT_MSG: {
			BString str="";

			if (current_folder.Length()<=base_folder.Length())
				break;
			str=current_folder;

			if (str[str.Length()-1]=='/') str.RemoveLast("/");
			str.Remove(str.FindLast("/"),str.Length()-str.FindLast("/"));

			AppendTrailingSlash(&str);
			current_folder=str;

			ShowFolder();
			break;
		}

		case 'mm03': // doppelklick auf listitem
		{
			// MSH: If an item was double clicked, stop editing. This avoids user confusion when
			// the item changes (and also avoids crashes related to this!)
			window1->view1->listview1->EndEditing();

			// auf audio-datei geklickt?
			if ((window1->view1->CDType()==AUDIOCD_INDEX) || (window1->view1->CDType()==CDEXTRA_INDEX)) {
				// edit audio file properties
				if (audioW==NULL) {
					AudioRow	*ar=(AudioRow *)window1->view1->listview1->CurrentSelection();
					BString		str=base_folder;
					str.Append(ar->GetFilename());
					audioW=new AudioWindow(str.String(), ar);
					audioW->SetTitle(ar->GetFilename());
					audioW->SetIndexList(ar->GetIndexList());
					audioW->SetCDText(ar->GetCDTitle());
					audioW->SetPregap(ar->GetPregap());
					if (CM->HasData(AUDIOWINDOW_RECT))
						audioW->MoveTo(CM->GetRect(AUDIOWINDOW_RECT)->LeftTop());
					audioW->Show();
				}
				break;
			}
		
			FolderRow *cs = (FolderRow *)window1->view1->listview1->CurrentSelection();
			if (cs == NULL) {
				break;
			}

			// clicked a normal file
			if (!cs->IsFolder()) {
				BString	s = current_folder;
				s.Append(cs->GetFilename());

				entry_ref	documentRef;
				BEntry *entry = new BEntry(s.String());
				entry->GetRef(&documentRef);
				entry->Unset();
				delete entry;
				entry = NULL;

				if (be_roster->Launch(&documentRef, 0, NULL, NULL)==B_OK|B_ALREADY_RUNNING) {
					if (window1->Lock()) {
						char	text[200];
						sprintf(text, "%s %s", cs->GetFilename(), _T("opened.")); // "L:opened."
						window1->view1->statusBAR->SetText(text, 4000000);
						window1->Unlock();
					}
				}

				break;
			}

			BString str="";
			str = current_folder;

			if (BString(cs->GetFilename()).Compare("..") == 0) {
				if (str[str.Length()-1]=='/') {
					str.RemoveLast("/");
				}
				str.Remove(str.FindLast("/"),str.Length()-str.FindLast("/"));
			} else {
				str.Append(cs->GetFilename());
			}

			BEntry *entry = new BEntry();
			entry->SetTo(str.String());
			if (entry->IsDirectory()) {
				AppendTrailingSlash(&str);
				current_folder=str;
				//printf("ENTERED %s\t%s\n",current_folder.String(),si->Text());
			}
			entry->Unset();
			delete entry;
			entry = NULL;

			ShowFolder();
			break;
		}
		
		case 'mm04': // Liste updaten
		{
			ShowFolder();
			break;
		}

		case 'mm05': // Pathdisplay updaten
		{
			//ShowFolder();
			break;
		}

		case 'mm07': {
			Update_DATA_AUDIO();
			break;
		}

		case PAUSECHANGE_MSG: {
			ChangePauses();
			break;
		}

		case DATAUDIO_MSG: {
			window1->Lock();
			window1->view1->UpdatePanels();
			window1->Unlock();
			Update_DATA_AUDIO();
			ShowFolder(true);		// update audio, too!
			if (saveCV->AutoUpdate()) {
				// update info box when switching between disc modes
				window1->view1->box2->Refresh();
			}
			window1->Lock();
			window1->Unlock();
			break;
		}

		case UPDATEINFO_MSG: {
			if (window1->Lock()) {
				BString tmpstring;
				tmpstring << _T("Updating") << B_UTF8_ELLIPSIS; // "L:Updating..."
				window1->view1->statusBAR->SetText( tmpstring.String(), 0);
				window1->Unlock();
			}
			if (!IsProject()) {
				break;
			}

			// just clicked the "update" button...
			window1->view1->box2->Refresh();
			ShowFolder();
			if (window1->Lock()) {
				window1->view1->statusBAR->SetText(_T("Updated."), 4000000); // "L:Updated."
				window1->Unlock();
			}
			break;
		}

		case BURN_MSG: {
			status_t	error = B_OK;
			if (!IsProject()) {
				break;
			}

			if (!CheckCDSize()) {
				ErrorBox *alert=new ErrorBox(E_BLUE_COLOR,	_T("Error"),	// "TERROR"
															_T("The project data size is greater than the maximum size of a CD. Remove some files or select 'Overburning' in the preferences panel."),	// "TOVERSIZED"
															_T("Ok"),		// "TOK"
															_T("Ignore"));	// "TIGNORE"
				PlayErrorSound();
				if (alert->Go() == 0) {
					break;
				}
			}

			fStatusWindow->Lock();
			if (heliosCV->IsSimulation()) {
				fStatusWindow->SetTitle(_T("Helios - SIMULATION")); // "TSTATUSWINDOWTITLESIMULATION"
			} else {
				fStatusWindow->SetTitle(_T("Helios")); // "TSTATUSWINDOWTITLE"
			}
			fStatusWindow->Unlock();

			switch(window1->view1->CDType()) {
				case	BOOTABLECD_INDEX:
				case	DATACD_INDEX: {
					if ((!saveCV->IsStreamed())
							||
						/* bfs? */ (window1->view1->GetFilesystem()==2)) {
						error=MakeImage_NOSTREAMING();
					} else {
						if (saveCV->IsStreamed()) {
							error=MakeImage();
						}
					}
					break;
				}

				case	CDEXTRA_INDEX: {
					error=BurnCDExtra();
					break;
				}

				case	AUDIOCD_INDEX: {
					error=BurnAudio(false);
					break;
				}

				case	DVDVIDEO_INDEX: {
					if ((!saveCV->IsStreamed())
							||
						/* bfs? */ (window1->view1->GetFilesystemDVDVideo()==2)) {
						error=MakeImage_NOSTREAMING();
					} else {
						if (saveCV->IsStreamed()) {
							error=MakeImage();
						}
					}
					break;
				}

				default: {
					error = B_ERROR;
					break;
				}
			}

			if (error == B_OK) {
				PlayFinishedBurningSound();
			} else {
				ErrorBox	*eb=new ErrorBox(E_RED_COLOR, errtype.String(), errstring.String(), _T("Ok"));
				PlayErrorSound();
				eb->Go();
			}

			fStatusWindow->Lock();
			fStatusWindow->SetTitle(_T("Helios")); // "TSTATUSWINDOWTITLE"
			fStatusWindow->Unlock();
			break;
		}

		case FILESYSTEM_MSG: {
	//	GetImageSize();
		break;
		}

		case RENAME_FILE_MSG: {
			FolderRow *selected=(FolderRow *)window1->view1->listview1->CurrentSelection();
			if (selected==NULL) break;
			window2->Lock();
			window2->todo=1;
			window2->SetTitle(_T("Rename")); // "TRENAMEITEM"
			window2->view2->textcontrol3->SetLabel(_T("New name:")); // "TNEWNAME"
			window2->view2->textcontrol3->SetText(selected->GetFilename());
			window2->Unlock();
			window2->Show();
			break;
		}
	

		case SETCDTEXTTITLE_MSG: {
			window2->Lock();
			window2->todo=5;
			window2->SetTitle(_T("CD text title")); // "TCDTEXTTITLE"
			window2->view2->textcontrol3->SetLabel(_T("Title:")); // "T___CDTEXTTITLE"
			AudioRow *selected=(AudioRow *)(window1->view1->listview1->CurrentSelection());
			window2->view2->textcontrol3->SetText(selected->GetCDTitle());
			window2->Unlock();
			window2->Show();
			break;
		}
		
		case 'mm15': // cd-text title
		{
			BString str;
			window1->Lock();
			str.SetTo(window2->view2->textcontrol3->Text());
			AudioRow *selected=(AudioRow *)(window1->view1->listview1->CurrentSelection());
			selected->SetCDTitle(str.String());
			//has_project_changed=true;
			window1->Unlock();
			break;
		}


		case CDRECORDPROTOCOL_MIMSG: {
			if (cdrecord_output->Lock()) {
				if (cdrecord_output->IsHidden()) {
					cdrecord_output->Show();
					if (window1->Lock()) {
						window1->view1->cdrecord_outputMI->SetMarked(true);
						window1->Unlock();
					}
				} else {
					cdrecord_output->Hide();
					if (window1->Lock()) {
						window1->view1->cdrecord_outputMI->SetMarked(false);
						window1->Unlock();
					}
				}
				cdrecord_output->Unlock();
			}
			break;
		}

		case MKISOFSPROTOCOL_MIMSG: {
			if (mkisofs_output->Lock()) {
				if (mkisofs_output->IsHidden()) {
					mkisofs_output->Show();
					if (window1->Lock()) {
						window1->view1->mkisofs_outputMI->SetMarked(true);
						window1->Unlock();
					}
				} else {
					mkisofs_output->Hide();
					if (window1->Lock()) {
						window1->view1->mkisofs_outputMI->SetMarked(false);
						window1->Unlock();
					}
				}
				mkisofs_output->Unlock();
			}
			break;
		}

		case PROTOCOL_WINDOW_CLOSED_MSG: {
			// Update menu ticks according to log window state.
			if (mkisofs_output->IsHidden()) {
				if (window1->Lock()) {
					window1->view1->mkisofs_outputMI->SetMarked(false);
					window1->Unlock();
				}
			}
			if (cdrecord_output->IsHidden()) {
				if (window1->Lock()) {
					window1->view1->cdrecord_outputMI->SetMarked(false);
					window1->Unlock();
				}
			}
			break;
		}

		case 'mm11': // markierten eintrag umbenennen
		{
			FolderRow *selected=(FolderRow *)(window1->view1->listview1->CurrentSelection());
			if (selected==NULL) {
				return;
			}

			window1->Lock();
			if (BString(selected->GetFilename()).Compare("..") != 0) {	     		
				BString str = "";
				str << current_folder.String() << selected->GetFilename();

				BEntry file;
				if (file.SetTo(str.String(),false) == B_OK) {
					str="";
				  str << current_folder.String() << window2->view2->textcontrol3->Text();
				  file.Rename(str.String(),false);
				  selected->SetFilename(window2->view2->textcontrol3->Text());
					project_has_changed=true;
				}
				file.Unset();
			}

			window1->Unlock();
			break;
		}

		case NEW_FOLDER_MSG: // Create new folder
		{
			// Stop all name editing first.
			window1->view1->listview1->EndEditing();

			BDirectory newdir;
			BString str="";
			int	count=0;
			char dirname[2048];

			window1->Lock();
			
			bool notCreatedYet = true;
			do {
				count++;
				if (count==1) {
					sprintf(dirname, "New Folder");
				} else {
					sprintf(dirname, "New Folder %d", count);
				}
				str="";
				str.Append(current_folder.String()).Append(dirname);
				notCreatedYet = (newdir.CreateDirectory(str.String(), NULL) == B_FILE_EXISTS);
				newdir.Unset();
			} while (notCreatedYet);

			project_has_changed = true;

			ShowFolder();
			window1->Unlock();
			break;
		}

		case NEWPROJECT_MIMSG: {
			NewProject();
			currentproject="";
			ShowFolder();
			break;
		}

		case OPENPROJECT_MIMSG: {
			BEntry *entry = FileDialog::OpenDialog(NULL, pathCV->GetProjectPath());
			if (entry->InitCheck() == B_OK) {
				if (window1->Lock()) {
					BString tmpstring;
					tmpstring << _T("Opening project") << B_UTF8_ELLIPSIS; // "L:Opening Project..."
					window1->view1->statusBAR->SetText( tmpstring.String(), 0);
					window1->Unlock();
				}

				LoadProject(entry);
				ShowFolder();
				if (window1->Lock()) {
					window1->view1->statusBAR->SetText(_T("Project opened."), 4000000); // "L:Project opened."
					window1->Unlock();
				}
				save_and_quit=false;
			} else {
				if (window1->Lock()) {
					window1->view1->statusBAR->SetText(_T("Operation cancelled."), 4000000); // "L:Operation cancelled."
					window1->Unlock();
				}
			}
			entry->Unset();
			delete entry;
			break;
		}

		case SAVEPROJECTAS_MIMSG: {
			if (!IsProject()) {
				break;
			}

			BString	original_base_folder=base_folder;
			char		statustext[128];
			char		cmdline[4096];

			BEntry *entry = FileDialog::SaveDialog(NULL, pathCV->GetProjectPath(), SuggestProjectName());
			if (entry->InitCheck() == B_OK) {
				if (window1->Lock()) {
					BString tmpstring;
					tmpstring << _T("Saving project") << B_UTF8_ELLIPSIS; // "L:Saving Project..."
					window1->view1->statusBAR->SetText( tmpstring.String(), 0);
					window1->Unlock();
				}
				SaveProject(entry, true);
				AppendTrailingSlash(&original_base_folder);
				sprintf(cmdline, "cp -R -d \"%s\"* \"%s\"", original_base_folder.String(), base_folder.String());
				if (window1->Lock()) {
					BString tmpstring;
					tmpstring << _T("Copying files") << B_UTF8_ELLIPSIS; // "TCOPYINGFILES"
					window1->view1->statusBAR->SetText( tmpstring.String(), 0);
					window1->Unlock();
				}
				system(cmdline);
				//printf("new path=%s\n", base_folder.String());

				sprintf(statustext, _T("Project saved.")); // "L:Project saved."
				save_and_quit=false;
			} else {
				sprintf(statustext, _T("Operation cancelled.")); // "L:Operation cancelled."
			}

			entry->Unset();
			delete entry;
			if (window1->Lock()) {
				window1->view1->statusBAR->SetText(statustext, 4000000);
				window1->Unlock();
			}
			break;
		}

		case REMOVEPROJECT_MIMSG: {
			if (!IsProject()) {
				break;
			}

			BString	bf = base_folder;
			bf = *RemoveTrailingSlash(bf);

			printf("bf=%s\n", bf.String());
			BEntry	*folder_of_project = new BEntry(bf.String(), false);
			BEntry	*projectfile = new BEntry(currentproject.String(), true);
			printf("cp=%s\n", currentproject.String());
 
			ErrorBox	*eb=new ErrorBox(E_BLUE_COLOR, "",	_T("This will erase the project file and all of the links and folders the project contains. Select 'Move to Trash' if you are not certain. What shall happen with your project files?"), // "TAREYOUSURE"
															_T("Cancel"), // "TCANCEL"
															_T("To trash"), // "TMOVETOTRASH"
															_T("Erase")); // "TERASE"
			switch (eb->Go()) {
				case 2: {
					{
						BDirectory removeDir(bf.String());
						DeleteAllIn(removeDir);
						removeDir.Unset();
					}
					
					folder_of_project->Remove();
					projectfile->Remove();
					CM->SetString(PATH_TO_LAST_PROJECT, "");
					currentproject="";
					base_folder="";
					current_folder="";
					ShowFolder();
					window1->SetTitle(_T("Helios")); // "TWINDOWTITLE"
					PostMessage(NEWPROJECT_MIMSG);
					ShowRecentProjects();
					break;
				}

				case 1: {
					// first, find the trash directory of this volume
					char	path_buffer[B_FILE_NAME_LENGTH];
#if !defined(_BEOS_R5_BUILD_) && !defined(_BEOS_HAIKU_BUILD_)
					if (find_directory(B_TRASH_DIRECTORY, dev_for_path(currentproject.String()), true, path_buffer, B_FILE_NAME_LENGTH)!=B_OK) {
						ErrorBox *eb = new ErrorBox(E_RED_COLOR,	_T("Error"), // "TERROR"
																	_T("Could not move the project files into trash."), // "Error:Removing Project"
																	_T("Ok")); // "TOK"
						eb->Go();
						break;
					}
#endif

					folder_of_project->MoveTo(new BDirectory(path_buffer));
					projectfile->MoveTo(new BDirectory(path_buffer));
					CM->SetString(PATH_TO_LAST_PROJECT, "");
					currentproject="";
					base_folder="";
					current_folder="";
					ShowFolder();
					window1->SetTitle(_T("Helios")); // "TWINDOWTITLE"
					PostMessage(NEWPROJECT_MIMSG);
					ShowRecentProjects();
					break;
				}
				
				case 0: {
					break;
				}
			};

			folder_of_project->Unset();
			delete folder_of_project;
			projectfile->Unset();
			delete projectfile;
			break;
		}

		case OPEN_FPMSG: {	
			entry_ref ref;
			if (message->FindRef("refs",  &ref) != B_OK) break;
			
			if (window1->Lock()) {
				BString tmpstring;
				tmpstring << _T("Opening project") << B_UTF8_ELLIPSIS; // "L:Opening Project..."
				window1->view1->statusBAR->SetText( tmpstring.String(), 0);
				window1->Unlock();
			}
			
			BEntry *entry = new BEntry(&ref, true);
			LoadProject(entry);
			ShowFolder();
			entry->Unset();
			delete entry;
			if (window1->Lock()) {
				window1->view1->statusBAR->SetText(_T("Project opened."), 4000000); // "L:Project opened."
				window1->Unlock();
			}
			break;
		}

		case FOLDER_FPMSG: {	
			entry_ref ref;
			if (message->FindRef("refs",  &ref) != B_OK) {
				break;
			}

			BPath	path;
			BEntry *entry = new BEntry(&ref, true);
			entry->GetPath(&path);
			entry->Unset();
			delete entry;
			entry = NULL;

			BDirectory HeliosDir;
			BString bf = path.Path();
			if (bf[bf.Length()-1]=='/') bf.RemoveLast("/");
			if (HeliosDir.SetTo(path.Path())!=B_OK)
				HeliosDir.CreateDirectory(bf.String(),&HeliosDir);
			HeliosDir.Unset();

			base_folder = path.Path();
			path.Unset();

			AppendTrailingSlash(&base_folder);
			current_folder = base_folder;

			NewProject();
			ShowFolder();
			break;
		}

		case SAVEPROJECT_MIMSG: {
			if (!IsProject()) break;
			BEntry *entry=new BEntry(currentproject.String(), true);
			char	statustext[128];
			printf("MessageReceived():currentproject=%s\n", currentproject.String());
			if (!entry->Exists()) {
				entry->Unset();
				delete entry;
				entry=FileDialog::SaveDialog(NULL, pathCV->GetProjectPath(), SuggestProjectName());
			}
			if (entry->InitCheck()==B_OK) {
				if (window1->Lock()) {
					BString tmpstring;
					tmpstring << _T("Saving project") << B_UTF8_ELLIPSIS; // "L:Saving Project..."
					window1->view1->statusBAR->SetText( tmpstring.String(), 0);
					window1->Unlock();
				}
				SaveProject(entry);
				sprintf(statustext, _T("Project saved.")); // "L:Project saved."
				save_and_quit=false;
			} else {
				sprintf(statustext, _T("Operation canceled.")); // "L:Operation cancelled."
			}
				
			entry->Unset();
			delete entry;
			if (window1->Lock()) {
				window1->view1->statusBAR->SetText(statustext, 4000000);
				window1->Unlock();
			}
			break;
		}
		
		case STANDARDPROJECT_MIMSG: {
			if (!IsProject()) break;
			SetAsStandardProject();
			break;
		}

		case FASTBLANK_MIMSG: {
			if (BlankRW("-blank=fast")==B_OK)
				PlayFinishedBurningSound();
			else {
				ErrorBox	*eb=new ErrorBox(E_RED_COLOR, errtype.String(), errstring.String(), _T("Ok"));
				PlayErrorSound();
				eb->Go();
			}
			break;
		}

		case COMPLETEBLANK_MIMSG: {
			if (BlankRW("-blank=all")==B_OK)
				PlayFinishedBurningSound();
			else {
				ErrorBox	*eb=new ErrorBox(E_RED_COLOR, errtype.String(), errstring.String(), _T("Ok"));
				PlayErrorSound();
				eb->Go();
			}
			break;
		}

		case UNCLOSESESSION_MIMSG: {
			if (BlankRW("-blank=unclose")==B_OK)
				PlayFinishedBurningSound();
			else {
				ErrorBox	*eb=new ErrorBox(E_RED_COLOR, errtype.String(), errstring.String(), _T("Ok"));
				PlayErrorSound();
				eb->Go();
			}
			break;
		}

		case BLANKSESSION_MIMSG: {
			if (BlankRW("-blank=session")==B_OK)
				PlayFinishedBurningSound();
			else {
				ErrorBox	*eb=new ErrorBox(E_RED_COLOR, errtype.String(), errstring.String(), _T("Ok"));
				PlayErrorSound();
				eb->Go();
			}
			break;
		}
		

		case EXPORT_M3U_MIMSG: {
			if (!IsProject()) break;
			
			char	name[1024];
			sprintf(name, "%s.m3u", SuggestProjectName());
			
			BEntry	*entry=FileDialog::SaveDialog(_T("Export"), pathCV->GetProjectPath(), name); // "FP:Export"

			if (entry->InitCheck()==B_OK) {
				if (window1->Lock()) {
					BString tmpstring;
					tmpstring << _T("Exporting project") << B_UTF8_ELLIPSIS; // "L:Exporting project..."
					window1->view1->statusBAR->SetText( tmpstring.String(), 0);
					window1->Unlock();
				}
				ExportAsM3U(entry);
				if (window1->Lock()) {
					window1->view1->statusBAR->SetText( _T("Project exported."), 4000000); // "L:Project exported."
					window1->Unlock();
				}
				entry->Unset();
			} else {
				if (window1->Lock()) {
					window1->view1->statusBAR->SetText( _T("Operation canceled."), 4000000); // "L:Operation cancelled."
					window1->Unlock();
				}
			}
			delete entry;
			break;
		}
		

		case EXPORT_APML_MIMSG: {
			if (!IsProject()) break;
			
			char	name[1024];
			sprintf(name, "%s.apml", SuggestProjectName());
			
			BEntry	*entry=FileDialog::SaveDialog(_T("Export"), pathCV->GetProjectPath(), name); // "FP:Export"
	
			if (entry->InitCheck()==B_OK) {
				if (window1->Lock()) {
					BString tmpstring;
					tmpstring << _T("Exporting project") << B_UTF8_ELLIPSIS; // "L:Exporting project..."
					window1->view1->statusBAR->SetText( tmpstring.String(), 0);
					window1->Unlock();
				}
				ExportAsAPML(entry);
				if (window1->Lock()) {
					window1->view1->statusBAR->SetText(_T("Project exported."), 4000000); // "L:Project exported."
					window1->Unlock();
				}
				entry->Unset();
			} else {
				if (window1->Lock()) {
					window1->view1->statusBAR->SetText(_T("Operation canceled."), 4000000); // "L:Operation cancelled."
					window1->Unlock();
				}
			}
			delete entry;
			break;
		}
		

		case EXPORT_FILELIST_MIMSG: {
			if (!IsProject()) break;
			
			char	name[1024];
			sprintf(name, "%s.txt", SuggestProjectName());
			
			BEntry	*entry=FileDialog::SaveDialog(_T("FP:Export"), pathCV->GetProjectPath(), name);

			if (entry->InitCheck()==B_OK) {
				if (window1->Lock()) {
					BString tmpstring;
					tmpstring << _T("Exporting project") << B_UTF8_ELLIPSIS; // "L:Exporting project..."
					window1->view1->statusBAR->SetText( tmpstring.String(), 0);
					window1->Unlock();
				}
				ExportAsFileList(entry);
				if (window1->Lock()) {
					window1->view1->statusBAR->SetText(_T("Project exported."), 4000000); // "L:Project exported."
					window1->Unlock();
				}
				entry->Unset();
			} else {
				if (window1->Lock()) {
					window1->view1->statusBAR->SetText(_T("Operation canceled."), 4000000); // "L:Operation cancelled."
					window1->Unlock();
				}
			}
			delete entry;
			break;
		}
		

		case EXPORT_TRACKERTEMPLATE_MIMSG: {
			if (!IsProject()) break;
			BEntry *entry=FileDialog::SaveDialog(	_T("Export"), // "FP:Export"
													"/boot/home/config/settings/Tracker/Tracker New Templates",
													SuggestProjectName());
			if (entry->InitCheck()==B_OK) {
				SaveProject(entry, true);
				entry->Unset();
			}
			delete entry;
			break;
		}
		

		case FOLDERMENU_MSG: {	// user clicked on statusview's folders-popupmenu
			BString folder;
			if (message->FindString("folder", &folder)==B_OK) {
				AppendTrailingSlash(&folder);
				current_folder=folder;
				ShowFolder();
			}
			break;
		}

		case IMAGE_MSG: {
			break;
		}
		
		
		case BURNIMAGEFILE_MIMSG: {
			status_t	error=B_OK;
			BEntry 		*entry;
			BPath		path;
			
			if (!CheckCDSize()) {
				ErrorBox *alert=new ErrorBox(E_BLUE_COLOR,	_T("Error"), 	// "TERROR"
															_T("The project data size is greater than the maximum size of a CD. Remove some files or select 'Overburning' in the preferences panel."), // "TOVERSIZED"
															_T("Ok"), // "TOK"
															_T("Ignore")); // "TIGNORE"
				PlayErrorSound();
				if (alert->Go()==0)
					break;
			}
			entry = FileDialog::OpenDialog(_T("Select image file")); // "FP:Select Image File"
			entry->GetPath(&path);
			if (!entry->Exists()) {
/*				ErrorBox *alert=new ErrorBox(E_BLUE_COLOR,	_T("Error"),// "TERROR"
															_T("File does not exist."), // "TNOTEXIST_ALERT"
															_T("Ok")); // "TOK"
				PlayErrorSound();
				//alert->SetShortcut(0,B_ENTER);
				alert->Go();*/
			} else {
				error = BurnImage(BString(path.Path()));
				if (error==B_OK) {
					PlayFinishedBurningSound();
					ErrorBox *alert=new ErrorBox(E_BLUE_COLOR, "",	_T("Delete the image file?"), // "TDELETEIMAGEFILE"
																	_T("No"), // "TNO"
																	_T("Yes")); // "TYES"
					alert->SetShortcut(0,B_ESCAPE);
					path.Unset();
					if (alert->Go()==1) {
						entry->Remove();
					}
				} else {
					ErrorBox	*eb=new ErrorBox(E_RED_COLOR, errtype.String(), errstring.String(), _T("Ok"));
					PlayErrorSound();
					eb->Go();
				}
			}
			path.Unset();
			entry->Unset();
			delete entry;
			break;
		}

		case READCDIMAGE_MIMSG: {
			status_t	error = B_OK;

			BEntry *entry = FileDialog::SaveDialog(_T("Enter new image file")); // "FP:Enter New Image File"
			if (entry->InitCheck() == B_OK) {
				BPath	path;
				entry->GetPath(&path);
				entry->Unset();
				delete entry;

				error = ReadImage(new BString(path.Path()));
				path.Unset();

				if (error == B_OK) {
					PlayFinishedImageSound();
				} else {
					ErrorBox	*eb=new ErrorBox(E_RED_COLOR, errtype.String(), errstring.String(), _T("Ok"));
					PlayErrorSound();
					eb->Go();
				}
			}
			break;
		}

		case READFLOPPYIMAGE_MIMSG: {
			status_t error = B_OK;
			BEntry  *entry = FileDialog::SaveDialog(_T("Enter new image file")); // "FP:Enter New Image File"
			if (entry->InitCheck()==B_OK) {
				BPath		path;
				entry->GetPath(&path);
				entry->Unset();
				delete entry;

				error = ReadFloppyImage(path.Path());
				path.Unset();

				if (error==B_OK) {
					PlayFinishedImageSound();
				} else {
					ErrorBox	*eb=new ErrorBox(E_RED_COLOR, errtype.String(), errstring.String(), _T("Ok"));
					PlayErrorSound();
					eb->Go();
				}
			}
			break;
		}

		case COPYDATACD_MIMSG: {
			status_t error = B_OK;
			if (deviceCV->IsTwoDriveCopy()) {
				error = CopyOnTheFly();
			} else {
				error = CopyDataCD();
			}
			if (error == B_OK) {
				PlayFinishedImageSound();
			} else {
				ErrorBox	*eb=new ErrorBox(E_RED_COLOR, errtype.String(), errstring.String(), _T("Ok"));
				PlayErrorSound();
				eb->Go();
			}
			break;
		}

		case COPYAUDIOCD_MIMSG: {
			status_t	error;

			if (deviceCV->IsTwoDriveCopy()) {
				error = CopyOnTheFlyAudio();
			} else {
				error = CopyAudioCD();
			}
				
			if (error>=0) {
				PlayFinishedImageSound();
			} else {
				ErrorBox	*eb=new ErrorBox(E_RED_COLOR, errtype.String(), errstring.String(), _T("Ok"));
				PlayErrorSound();
				eb->Go();
				break;
			}

			break;
		}

		case CREATEFILEIMAGE_MIMSG: {
			status_t	error=B_OK;
			BEntry 		*entry;

			if (!IsProject()) break;
			entry=FileDialog::SaveDialog(_T("Enter new image file")); // "FP:Enter New Image File"
			if (entry->InitCheck()==B_OK) {
				BPath		path;
				entry->GetPath(&path);
				entry->Unset();
				delete entry;

				error = MakeImageOnly(path.Path());
				path.Unset();

				if (error==B_OK) {
					PlayFinishedImageSound();
				} else {
					ErrorBox	*eb=new ErrorBox(E_RED_COLOR, errtype.String(), errstring.String(), _T("Ok"));
					PlayErrorSound();
					eb->Go();
				}
			}
			errtype="";
			errstring="";
			break;
		}

		case BURNSPEED_MSG: {
			Calculate_time();
			break;
		}

		case SPLIT_TRACK_MSG: {
			BString str="";
			window2->Lock();
			window2->todo=4;
			window2->SetTitle(_T("Split title")); // "TSPLITTRACK"
			window2->view2->textcontrol3->SetLabel(_T("Split at:")); // "TSPLITOFFSET"
			AudioRow *selected=(AudioRow *)(window1->view1->listview1->CurrentSelection());
			// display the half size of track as a suggestion for the split offset
			str << selected->GetLength()/2 << "s";
			window2->view2->textcontrol3->SetText(str.String());
			window2->Unlock();
			window2->Show();
			break;
		}
		
		case 'mm14': // splits the selected track into two parts
		{
			AudioRow *ai=(AudioRow *)(window1->view1->listview1->CurrentSelection());
			BString str=window2->view2->textcontrol3->Text();
			size_t splitoffset; // in bytes
			
			if (str.FindFirst("f") == B_ERROR) {
				// typed in as seconds
				str.RemoveAll("s");
				splitoffset=atol(str.String())*176400;			
			} else {
				// typed in as frames
				str.RemoveAll("f");
				splitoffset=176400*atol(str.String())/75;
			}
			window1->Lock();
			window1->view1->listview1->AddRow(new AudioRow(ai->GetTrackNumber()+1, ai->GetFilename(),0,ai->GetBytes()-splitoffset, (ai->GetBytes()-splitoffset)*176400), window1->view1->listview1->IndexOf(ai)+1);
			ai->SetBytes(splitoffset);
			if (!ai->IsSplitted()) {ai->SetSplitIndex(0);ai->SetSplitted(true);}
			project_has_changed=true;
			UpdateTrackNumbers();
			window1->Unlock();
			break;
		}

		case FIXATE_CHKBOX_MSG: {
			Calculate_time();
			break;
		}

		case SAVE_SETTINGS: {
			SaveSettings();
			break;
		}

		case HELIOS_LANGUAGE_UPDATE_MSG: {
			BWindow	*window;
			
			for (int32 w=0; w<CountWindows(); w++) {
				window=WindowAt(w);
				
					window->Lock();
				for (int32 v=0; v<window->CountChildren(); v++) {
					window->ChildAt(v)->Looper()->PostMessage(HELIOS_LANGUAGE_UPDATE_MSG, window->ChildAt(v));
				}
					window->Unlock();
			}
			break;
		}
		
		default: {
			BApplication::MessageReceived(message);
			break;
		}
	}

	if (applicationCV->IsAutoUpdating()) {
		window1->view1->box2->StartUpdating();
	}
}

void
Application1::RefsReceived(BMessage *message)
{

	uint32 		type;
	int32 		count;
	BEntry		*entry = new BEntry();
	entry_ref	ref;

	message->GetInfo("refs", &type, &count);
	
	if (type != B_REF_TYPE) {
		entry->Unset();
		delete entry;
		return;
	}
	if (message->FindRef("refs", 0, &ref) != B_OK) {
		if (window1->Alert("Error at BMessage.FindRef()\n\nProbably you didn't drop a project file.")==true) { delete entry; return;}
	} else	{
		if (entry->SetTo(&ref,true)==B_OK) {
			LoadProject(entry);
		}
	}
	entry->Unset();
	delete entry;	

	ShowFolder();
}


/****************************************************************************
 *
 *	Shows content of current folder in file window.
 *
 ***************************************************************************/
void
Application1::ShowFolder(bool audio) {
	BDirectory dir;
	BEntry de;
	BPath path;
	int tracks=0;
	entry_ref	ref;
	
	
	dir.SetTo(current_folder.String());
	dir.Rewind();
	window1->Lock();
	if ((audio) || ((window1->view1->CDType()==DATACD_INDEX) || (window1->view1->CDType()==DVDVIDEO_INDEX) || (window1->view1->CDType()==BOOTABLECD_INDEX))) {
		while (window1->view1->listview1->CountRows()>0) {
			BRow	*row=window1->view1->listview1->RowAt(0);
			window1->view1->listview1->BColumnListView::RemoveRow(row);
			delete row;
		}
	}
	if (current_folder!=base_folder) {
		FolderRow	*frROW=new FolderRow("..", true, NULL, true);
		window1->view1->listview1->AddRow(frROW ,(int32)0);
		frROW->GetField(FILENAME_FIELD)->SetEditable(false);
	}
	window1->Unlock();

			while (dir.GetNextEntry(&de, false)==B_OK) {
				if (de.IsDirectory()) {
					de.GetPath(&path);
					if ((window1->view1->CDType()==DATACD_INDEX) ||
						(window1->view1->CDType()==BOOTABLECD_INDEX) ||
						(window1->view1->CDType()==DVDVIDEO_INDEX)) {	
							BFile file(&de, B_READ_ONLY);
							off_t filesize;
							file.GetSize(&filesize);
							de.GetRef(&ref);
							if (window1->Lock()) {
								window1->view1->listview1->AddRow(new FolderRow(path.Leaf(), 0, true, GetIcon(&ref)),(int32)0);
								window1->Unlock();
							}
							file.Unset();
					}
				}
			}
			dir.Rewind();
	
			while (dir.GetNextEntry(&de,false)==B_OK)
			{
			if ((de.IsSymLink()) || (de.IsFile()))
				{
				de.GetPath(&path);
				if ((window1->view1->CDType()==DATACD_INDEX) ||
					(window1->view1->CDType()==BOOTABLECD_INDEX) ||
					(window1->view1->CDType()==DVDVIDEO_INDEX))
					{
					BFile file(&de, B_READ_ONLY);
					off_t filesize;
					if (file.GetSize(&filesize)!=B_OK) filesize=0;
					de.GetRef(&ref);
					if (window1->Lock()) {
						window1->view1->listview1->AddRow(new FolderRow(path.Leaf(),filesize, false, GetIcon(&ref)), window1->view1->listview1->CountRows());
						window1->Unlock();
					}
					file.Unset();
					} else
					if (audio)
					{
					float size;

					
					TrackLength *info=new TrackLength(new BEntry(path.Path(), true));
					size=info->GetBytes();

					if (size>0) {
						if (window1->Lock()) {
							AudioRow	*ar;
							window1->view1->listview1->AddRow((ar=new AudioRow(++tracks, path.Leaf(),0,size, size/176400)));
							ar->SetStartFrame((int64)0);
							ar->SetEndFrame((int64)(75.0F*(float)size/176400.0F));
							ar->SetLength((int64)(75.0F*(float)size/176400.0F));
							if (tracks==1)
								ar->SetPregap(150);
							window1->Unlock();
						}
					}
					delete info;
					
					}
				}

			}
	dir.Unset();
	de.Unset();
	path.Unset();

	window1->Lock();
	BString	cp=current_folder;
	cp.ReplaceFirst(base_folder.String(), "");
	cp.Prepend("/");
	window1->view1->SetCurrentPath(cp.String());
	if (window1->view1->listview1->CountRows()>0) {
		window1->view1->listview1->ScrollTo(window1->view1->listview1->RowAt(0));
	}
	PopulateFoldersMenu();
	window1->Unlock();
}


bool
Application1::GetImageSize()
{
		int arguments=0;
		thread_id Thread;
		const char **args;
		BString str="";
		BString filesystem="";
		int in, out, err;
		size_t	sz=0;

		switch(window1->view1->CDType()) {
			case AUDIOCD_INDEX: {
				float time=0;
				for (int i=0; i<window1->view1->listview1->CountRows();i++) {
					AudioRow *si=(AudioRow *)window1->view1->listview1->RowAt(i);
	
					time+=si->GetLength();
					time+=(float)si->GetPregap();
				}
				window1->view1->box2->SetCDSize((size_t)((time/75.0F)*153600.0F));
				return true;
				break;
			}

			case DATACD_INDEX: {
				BList	*list=window1->view1->datapanel->GetFilesystem();
				if (list->CountItems()==1)
					if (((BString *)list->ItemAt(0))->Compare("BFS")==0) {

						off_t imgsize = 0;
						imgsize = GetBFSImageSize(base_folder.String());

						window1->view1->box2->SetCDSize((off_t)imgsize);
						delete ((BString *)list->RemoveItem(0L));
						delete list;
						return true;
					}
					
				for (int i=list->CountItems()-1; i>=0; i--)
					delete ((BString *)list->RemoveItem(i));
				delete list;
				
				break;
			}

			case BOOTABLECD_INDEX: {
//				if (window1->view1->GetFilesystemBootCD()==2) {
//					off_t imgsize = 0;
//					imgsize = GetBFSImageSize(base_folder.String());
//					window1->view1->box2->SetCDSize((off_t)imgsize);
//					return true;
//				}
				break;
			}

			case CDEXTRA_INDEX: {
				float time=0;
		
				for (int i=0; i<window1->view1->listview1->CountRows();i++) {
					AudioRow *si=(AudioRow *)window1->view1->listview1->RowAt(i);
					time+=si->GetLength();
					time+=(float)si->GetPregap();
				}
				sz=(size_t)((float)time/75.0F*153600.0F);

				BList	*list=window1->view1->cdextrapanel->GetFilesystem();
				if (list->CountItems()==1)
					if (((BString *)list->ItemAt(0))->Compare("BFS")==0) {

						off_t imgsize = 0;
						imgsize = GetBFSImageSize(base_folder.String());

						window1->view1->box2->SetCDSize((off_t)imgsize+sz);
						delete ((BString *)list->RemoveItem(0L));
						delete list;
						return true;
					}
				for (int i=list->CountItems()-1; i>=0; i--)
					delete ((BString *)list->RemoveItem(i));
				delete list;
				break;
			}

			case DVDVIDEO_INDEX: {
				BList	*list=window1->view1->dvdvideopanel->GetFilesystem();
				if (list->CountItems()==1)
					if (((BString *)list->ItemAt(0))->Compare("BFS")==0) {

						off_t imgsize = 0;
						imgsize = GetBFSImageSize(base_folder.String());

						window1->view1->box2->SetCDSize((off_t)imgsize);
						delete ((BString *)list->RemoveItem(0L));
						delete list;
						return true;
					}
					
				for (int i=list->CountItems()-1; i>=0; i--)
					delete ((BString *)list->RemoveItem(i));
				delete list;
				
				break;
			}
			
			default: {
				break;
			}
		}


		args = (const char **)malloc(sizeof(char *) * (100));
		if (MKISOFS_CommandLine(args, &arguments, true) != B_OK) {
			return false;
		}

		Thread=pipe_command(arguments, args, in, out, err);
		set_thread_priority(Thread,2);
		if (Thread<0) {
			errtype= _T("Error"); // "TERROR"
			errstring="mkisofs\n\n";
			errstring << _T("Could not load this command line tool. Make sure it is in the correct place."); // "TCOULDNOTLOAD"
			free(args);
			HideStatus();
			return B_ERROR;
		}
		rename_thread(Thread,MKISOFSTHREADNAME);
		size_t bytesread=0;
		char buffer[10000];
		bool gotit=false;

		resume_thread(Thread);
		do {
			bytesread=read(err,(void *)buffer, 10000);
			buffer[bytesread]=0;

			// output it to the log-window
			 mkisofs_output->AddText(buffer);

			if (bytesread>0) {
				if (strstr(buffer,"to be written =")) {
					BString s;
					s=strstr(buffer,"to be written =");
					s.RemoveFirst("to be written =");
					window1->view1->box2->SetCDSize((sz+atol(s.String())*2048));
					cdextra_datasize=atol(s.String())*2048;
					gotit=true;
				} // strstr			
			}
			snooze(10000);
		} while ((Thread==find_thread(MKISOFSTHREADNAME)) || (bytesread>0)/*|| (!gotit)*/);

		close(in);close(out);close(err);
			
		free(args);
	return true;
}

/*
 *	Thread function for data transport mkisofs <-> cdrecord
 */
typedef struct {
	int in;
	int out;
	int32 buffersize;
	bool *working;
} bufferfunc;


int32
DataBuffer(void *data) {
	char *buffer1(NULL);
	size_t written=0;
	size_t bytesread1=1;
	bufferfunc *d=(bufferfunc *)data;
	buffer1=new char[d->buffersize];
	printf("HELIOS: Buffer Thread Started.\n");
	while ((d->working) && (bytesread1 >0) && (written>=0)) {
		bytesread1=read(d->in,buffer1, d->buffersize);
		if (bytesread1>0)
			written=write(d->out,buffer1,bytesread1);
	}

	delete buffer1;
	printf("HELIOS: Exit Buffer Thread Without Errors.\n");
	return 0;
}


status_t
Application1::WaitForDisc(const char *device) {
		if (!applicationCV->IsTimeout()) return B_OK;

		char 	info[4];
		int	timeout=applicationCV->GetTimeoutLength()*10;

		BString tmpstring;
		tmpstring << _T("Waiting for disc") << B_UTF8_ELLIPSIS;
		ShowStatus( tmpstring.String(),0); // "TWAITINGFORDISC"
		while (DeviceStatus(device)!=E_NO_ERROR) {
			snooze(100000);
			timeout--;
			if ((timeout==0) || (fStatusWindow->interrupted)) {
				HideStatus();
				return B_TIMEOUT;
			}

			sprintf(info,"%d\"",timeout/10);
			SetStatusInfo((const char *)info);
			SetPercentage((float)timeout*100/((float)applicationCV->GetTimeoutLength()*10));
		}
		return B_OK;
}


void
Application1::ReadOutputOfCdrecord(int outstream, thread_id thread, int tracks) {
	size_t bytesread=0;
	char *buffer=new char[65536];
	int speed=0;
	int lasttrack=0, track=1;
	float length=0;	// in MB
	float op=0,progressed=0;
	char info[40], info2[40];
	BString s;

	do {
		BString tmpstring;
		bytesread = 0;
		bytesread = read(outstream,(void *)buffer, 65536);

		if (bytesread > 0) {
			buffer[bytesread]=0;
		
			cdrecord_output->AddText(buffer);

			// get speed
			if (speed==0)
			if (strstr(buffer,"at speed")) {
				s=strstr(buffer,"at speed");
				s.Remove(0,8);
				s.Remove(s.FindFirst("in"),s.Length()-s.FindFirst("in"));
				speed=atoi(s.String());
				sprintf(info,"%dx (%0.1f MB/s)",speed,153600.0*(float)speed/(1024.0*1024.0));
				SetStatusInfo(info);
			}
			
			// Performing OPC...
			if (strstr(buffer,"Performing OPC...")) {
				tmpstring << _T("Adjusting laser power")<< B_UTF8_ELLIPSIS; // "TPERFORMINGOPC"
				SetStatus( tmpstring.String(),-1);
			}
				// finished fixating...
			if (strstr(buffer,"Fixating time:")) {
				SetStatus(_T("Finished writing CD."),-1); // "TFINISHEDWRITING"
			}
				// start burning...
			if (strstr(buffer,"Starting new track")) {
				tmpstring << _T("Burning CD")<< B_UTF8_ELLIPSIS; // "TBURNING"				
				SetStatus( tmpstring.String(),-1);
			}
				// Fixating...
			if (strstr(buffer,"Fixating...")) {
				tmpstring << _T("Fixating disc")<< B_UTF8_ELLIPSIS; // "TFIXATING"				
				SetStatus( tmpstring.String(),-1);
				SetPercentage(100.0);
			}
				// get track number
			if (strstr(buffer,"Track")) {
				s=strstr(buffer,"Track");
				s.RemoveFirst("Track");
				s.Remove(s.FindFirst(":"),s.Length()-s.FindFirst(":"));
				track=atoi(s.String());
			}	
				
			if (lasttrack!=track) {
				lasttrack=track;
				op=0;
				progressed=0;
			}
				// get track length
			if (strstr(buffer,"of")) {
				s=strstr(buffer,"of");
				s.RemoveFirst("of");
				s.Remove(s.FindFirst("MB"),s.Length()-s.FindFirst("MB"));
				length=atof(s.String());
			}		

			// get write position
			if (strstr(buffer,"Track")) {
				s=strstr(buffer,"Track");
				s.Remove(0,9);
				s.Remove(s.FindFirst("of"),s.Length()-s.FindFirst("of"));
				progressed=atof(s.String());
				if (op<progressed) {
					op=progressed;
					if (length>0) {
						sprintf(info,"%0.1f%%",100.0*(float)progressed/(float)length);
						sprintf(info2,"%s %d/%d",_T("Track"),track, tracks); // "TTRACK"
						SetStatusInfo(info, info2);
						SetPercentage(100.0*(float)progressed/(float)length);
					}
				}
			}		
		}
		snooze(250000);
	}while (thread==find_thread(CDRECORDTHREADNAME));
	
	delete [] buffer;
}	


status_t
Application1::MakeImage()
{
		int arguments=0;
		static thread_id buffer_Thread=-1, mkisofs_Thread=-1, cdrecord_Thread=-1;
		const char **args;
		BString str="";
		BString device="dev=";
		BString filesystem="";
		bool working;
		bufferfunc	bf;
		int mkisofs_in, mkisofs_out, mkisofs_err, cdrecord_in, cdrecord_out, cdrecord_err;
		BString	lastarg, tmpstring;

		fStatusWindow->interrupted=false;
		if (find_thread(CDRECORDTHREADNAME)==cdrecord_Thread) kill_thread(cdrecord_Thread);
		if (find_thread(BUFFERTHREADNAME)==buffer_Thread) kill_thread(buffer_Thread);
		if (find_thread(MKISOFSTHREADNAME)==mkisofs_Thread) kill_thread(mkisofs_Thread);

		// wait until a disc is in the drive and drive is ready
		if (WaitForDisc(deviceCV->GetWriterDevicePath())==B_TIMEOUT) {
			errtype=_T("Timeout"); // "TTIMEOUTTYPE"
			errstring=_T("The active process had to wait too long for an event."); // "TTIMEOUTERROR"
			HideStatus();
			return B_ERROR;
		}


		//									//
		//   begin to produce the options for mkisofs...	//
		//									//

		if (window1->Lock()) {
			window1->view1->box2->Refresh();
			window1->Unlock();
		}
		device << deviceCV->GetWriterDeviceNumber();
		args = (const char **)malloc(sizeof(char *) * (50));
		// Make MKISOFS options
		
		if (MKISOFS_CommandLine(args, &arguments)!=B_OK) {
			HideStatus();
			free(args);
			return B_ERROR;
		}


		printArguments("MKISOFS Arguments",args);


		// launch mkisofs thread now
		mkisofs_Thread=pipe_command(arguments, args, mkisofs_in, mkisofs_out, mkisofs_err);
		set_thread_priority(mkisofs_Thread,MKISOFS_PRIORITY);
		if (mkisofs_Thread<0) {
			errtype= _T("Error"); // "TERROR"
			errstring="mkisofs\n\n";
			errstring << _T("Could not load this command line tool. Make sure it is in the correct place."); // "TCOULDNOTLOAD"
			free(args);
			HideStatus();
			return B_ERROR;
		}
		tmpstring = "";
		tmpstring << _T("Waiting")<< B_UTF8_ELLIPSIS; // "TWAITING"
		SetStatus( tmpstring.String(),mkisofs_Thread);
		rename_thread(mkisofs_Thread,MKISOFSTHREADNAME);

		// Make CDRECORD options
		arguments=0;
		CDRECORD_CommandLine(args, &arguments, window1->view1->IsMultisession(),
				true /*isStreaming*/);
		
		args[arguments++] = strdup("tsize=");
		str.SetTo("");
		if (window1->view1->CDType()==CDEXTRA_INDEX) {
			str << cdextra_datasize;
		} else {
			str << window1->view1->box2->GetCDSize();
		}

		args[arguments++] = strdup(str.String());
		args[arguments++] = strdup("-data");
		args[arguments++] = strdup("-");
		args[arguments]=NULL;
		
		printArguments("cdrecord streaming args",args);

		// launch cdrecord thread now
		cdrecord_Thread=pipe_command(arguments, args, cdrecord_in, cdrecord_out, cdrecord_err);
		set_thread_priority(cdrecord_Thread, saveCV->GetBurnPriority()/*CDRECORD_PRIORITY*/);
		if (cdrecord_Thread<0) {
			errtype=_T("Error"); // "TERROR"
			errstring="cdrecord\n\n";
			errstring << _T("Could not load this command line tool. Make sure it is in the correct place."); // "TCOULDNOTLOAD"
			free(args);
			HideStatus();
			return B_ERROR;
		}
		tmpstring = "";
		tmpstring << _T("Waiting")<< B_UTF8_ELLIPSIS; // "TWAITING"
		SetStatus(tmpstring.String(),cdrecord_Thread);
		rename_thread(cdrecord_Thread,CDRECORDTHREADNAME);
		working=true;
		bf.working=&working;
		bf.buffersize=4*1024;
		bf.in=mkisofs_out;
		bf.out=cdrecord_in;
		
		buffer_Thread=spawn_thread(DataBuffer,BUFFERTHREADNAME,BUFFER_PRIORITY,(void *)(&bf));
		resume_thread(buffer_Thread);	
		resume_thread(mkisofs_Thread);	
		resume_thread(cdrecord_Thread);	
	
		mkisofs_output->Start(mkisofs_err);
		
	
		ReadOutputOfCdrecord(cdrecord_out, cdrecord_Thread);		
		
		
		working=false;
		bf.working=&working;
		HideStatus();
		mkisofs_output->Stop();
		//ioctl(mkisofs_out, 26 , cdrecord_in);
		if (find_thread(BUFFERTHREADNAME)==buffer_Thread) kill_thread(buffer_Thread);
		if (find_thread(MKISOFSTHREADNAME)==mkisofs_Thread) kill_thread(mkisofs_Thread);
		close(mkisofs_in);close(mkisofs_out);close(mkisofs_err);
		close(cdrecord_in);close(cdrecord_out);close(cdrecord_err);
		snooze(10000);
//		kill_thread(reader_Thread);
//		kill_thread(writer_Thread);
		if (fStatusWindow->interrupted) {
			errtype=_T("Error"); // "TERROR"
			errstring=_T("You just interrupted and terminated the process."); // "TINTERRUPTEDBYUSER"
			free(args);
			return B_ERROR;
		}
		if (deviceCV->IsWriterEjected())
			eject_media(deviceCV->GetWriterDevicePath());
		free(args);
		return B_OK;
}


status_t
Application1::MakeImage_NOSTREAMING()
{
		BString temp=*CM->GetString(TEMP_PATH);
		status_t	retvalue=B_ERROR;
		AppendTrailingSlash(&temp);
		temp.Append(pathCV->GetDefaultImageName());
		switch(window1->view1->CDType()) {
			case DATACD_INDEX: {
				BList	*list=window1->view1->datapanel->GetFilesystem();
				if (list->CountItems()==1) {

					if (((BString *)list->ItemAt(0))->Compare("BFS")==0) {
						retvalue = MakeBFSImage(temp.String());
						for (int i=list->CountItems()-1; i>=0; i--) {
							delete ((BString *)list->RemoveItem(i));
						}
						delete list;
						break;
					}
				} else {
					retvalue=MakeMKISOImage(temp.String());
					for (int i=list->CountItems()-1; i>=0; i--) {
						delete ((BString *)list->RemoveItem(i));
					}
					delete list;
					break;
				}
			}
		}

		if (retvalue==B_OK) {
			retvalue=BurnImage(temp.String());
		}

		return retvalue;
}


int
Application1::ReadAudioCD() {
		const char **args;
		int arguments=0;
		thread_id Thread;
		BString str="", tmpstring;
		int in, out, err;
				
		if (WaitForDisc(deviceCV->GetReaderDevicePath())==B_TIMEOUT) {
			errtype=_T("Timeout"); // "TTIMEOUTTYPE"
			errstring=_T("The active process had to wait too long for an event."); // "TTIMEOUTERROR"
			HideStatus();
			return B_ERROR;
		}

		// allocate some memory for the command line array
		args = (const char **)malloc(sizeof(char *) * (23));

		// fill array with arguments
		args[arguments++] = strdup(CM->GetString(PATH_TO_CDDA2WAV)->String());
		args[arguments++] = strdup("-D");
		args[arguments++] = strdup(deviceCV->GetReaderDeviceNumber());
		args[arguments++] = strdup("-x");
		args[arguments++] = strdup("-B");
		args[arguments++] = strdup("-g");
		args[arguments++] = strdup("-t");
		args[arguments++] = strdup("1");
		str=base_folder;
		str.Append("Track");
		args[arguments++] = strdup(str.String());
		args[arguments] = NULL;


		// and _please_ launch a new thread!
		Thread=pipe_command(arguments, args, in, out, err);
		set_thread_priority(Thread,CDDA2WAV_PRIORITY);
		if (Thread<0) {
			errtype= _T("Error"); // "TERROR"
			errstring="cdda2wav\n\n";
			errstring << _T("Could not load this command line tool. Make sure it is in the correct place."); // "TCOULDNOTLOAD"
			free(args);
			HideStatus();
			return -1;
		}
		tmpstring << _T("Reading audio tracks") << B_UTF8_ELLIPSIS; // "TREADINGAUDIO"
		ShowStatus( tmpstring.String(),Thread);
		rename_thread(Thread,CDDA2WAVTHREADNAME);
		resume_thread(Thread);
		
		size_t bytesread=0;
		char buffer[10000];
		char info[40], info2[40];
		float percentage, oldpercentage=1000;
		BString	per="";
		int	trackno=0;
		
		while (Thread==find_thread(CDDA2WAVTHREADNAME)) {
			bytesread=read(err,(void *)buffer, 1000);
			buffer[bytesread]=0;
			if (bytesread>0) {
				if (strstr(buffer,"%")) {
					per=buffer;
					per.Remove(per.FindFirst("%"), per.Length()-per.FindFirst("%"));
					while (per.Length()>3) per.Remove(0,1);
					percentage=atof(per.String());
					if (percentage<oldpercentage) {
						trackno++;
						if (trackno>1) {
							BString	path, leaf="Track_";
							off_t size;			
				
							leaf << ((trackno-1<10)?"0":"") << trackno-1 << ".wav";
							path=base_folder;
							path.Append(leaf);
		
							TrackLength *info=new TrackLength(new BEntry(path.String(), true));
							size=info->GetBytes();
							if (size>0) {
								ShowFolder(true);
								if (window1->Lock()) {
									//window1->view1->listview1->AddRow(new AudioRow(window1->view1->listview1->iCountItems()+1, leaf.String(), 0, size, size/176400));
									window1->Unlock();
								}
							}
							delete info;
						}						
					}
					sprintf(info,"%0.0f%%", percentage);
					sprintf(info2,"%s %d",_T("Track"), trackno); // "TTRACK"
					SetStatusInfo(info, info2);
					SetPercentage(percentage);
					oldpercentage=percentage;
				} // strstr			
			}
			snooze(250000);
		}
		close(in);close(out);close(err);
		ShowFolder(true);
		HideStatus();
		if (fStatusWindow->interrupted) {
			errtype=_T("Error"); // "TERROR"
			errstring=_T("You just interrupted and terminated the process."); // "TINTERRUPTEDBYUSER"
			free(args);
			return -1;
		}
		if (deviceCV->IsReaderEjected())
			eject_media(deviceCV->GetReaderDevicePath());
			
		free(args);
		return trackno;
}


// Status window functions - need to move into separate class.
void
Application1::ShowStatus(const char *text, thread_id Thread)
{
		if (fStatusWindow->Lock()) {
			fStatusWindow->Thread=Thread;
			fStatusWindow->EnableControls();
			fStatusWindow->interrupted=false;
			fStatusWindow->fStatusView->stringview12->SetText(text);
			fStatusWindow->MoveTo(window1->Frame().left+(window1->Bounds().Width()-fStatusWindow->Bounds().Width())/2,
						window1->Frame().top+(window1->Bounds().Height()-fStatusWindow->Bounds().Height())/2);
			fStatusWindow->SetWorkspaces(B_CURRENT_WORKSPACE);
			fStatusWindow->fStatusView->progressSB->Reset();
			if (fStatusWindow->IsHidden()) fStatusWindow->Show();
			fStatusWindow->Unlock();
		}
}

void
Application1::SetStatus(const char *text, thread_id Thread)
{
		if (fStatusWindow->Lock()) {
			if (Thread!=-1) {
				fStatusWindow->Thread=Thread;
				fStatusWindow->fStatusView->progressSB->Reset();
			}
			fStatusWindow->EnableControls();
			fStatusWindow->interrupted=false;
			fStatusWindow->fStatusView->stringview12->SetText(text);
			if (fStatusWindow->IsHidden()) fStatusWindow->Show();
			fStatusWindow->Unlock();
		}
}

void
Application1::HideStatus()
{
		if (fStatusWindow->Lock()) {
			if (!fStatusWindow->IsHidden()) fStatusWindow->Hide();
			fStatusWindow->Unlock();
		}
}

void
Application1::SetStatusInfo(const char *text, const char *leading)
{
		BString s=text;
		if (fStatusWindow->Lock()) {
			if (s.Compare(fStatusWindow->fStatusView->progressSB->TrailingText())!=0)
			fStatusWindow->fStatusView->progressSB->SetTrailingText(text);
			s=leading;
			if (s.Compare(fStatusWindow->fStatusView->progressSB->Text())!=0)
			fStatusWindow->fStatusView->progressSB->SetText(leading);
			fStatusWindow->Unlock();
		}
}

void
Application1::SetPercentage(float percentage) {
	if (fStatusWindow->Lock()) {
		if (percentage>100) percentage=100;
		if (percentage<0) percentage=0;
		fStatusWindow->fStatusView->progressSB->Update(percentage-fStatusWindow->fStatusView->progressSB->CurrentValue());
		fStatusWindow->Unlock();
	}
}
// *** END OF STATUS WINDOW FUNCTIONS


void
Application1::ChangePauses()
{
	float pause=0;
	
	window1->Lock();
	pause=atof(window1->view1->textcontrol2->Text());
	if (pause>4800) pause=4800;
	if (pause<0) pause=0;
	//sprintf(bla,"%.2f",pause);
	for (int i=0; i<window1->view1->listview1->iCountItems();i++)
				{
				AudioRow *si=(AudioRow *)window1->view1->listview1->RowAt(i);
				if (window1->view1->listview1->IsSelected(i))
					{
					si->SetPregap(pause);
					//window1->view1->listview1->InvalidateRow(i);
					//si->Select();
					}
				}
	window1->view1->textcontrol2->SetText("");
	//window1->view1->textcontrol2->SetText(bla);
	//window1->view1->textcontrol2->MakeFocus(false);
	window1->Unlock();
}


void
Application1::DoAudioMenu()
{
	window1->Lock();

	while (window1->view1->listview1->cmenu->CountItems()>0)
		window1->view1->listview1->cmenu->RemoveItem(0L);
	window1->view1->listview1->cmenu->AddItem(window1->view1->listview1->deleteItem);
	window1->view1->listview1->cmenu->AddSeparatorItem();
	window1->view1->listview1->cmenu->AddItem(window1->view1->listview1->moveupItem);
	window1->view1->listview1->cmenu->AddItem(window1->view1->listview1->movedownItem);
	window1->view1->listview1->cmenu->AddSeparatorItem();
	window1->view1->listview1->cmenu->AddItem(window1->view1->listview1->indexItem);
	window1->view1->listview1->cmenu->AddItem(window1->view1->listview1->splitItem);
//	window1->view1->listview1->cmenu->AddSeparatorItem();
//	window1->view1->listview1->cmenu->AddItem(window1->view1->listview1->CDTEXTTitleItem);
	window1->view1->listview1->cmenu->SetTargetForItems(be_app_messenger);
	window1->Unlock();
}


void
Application1::DoDataMenu()
{
	if (window1->view1->listview1->cmenu==NULL) return;
	window1->Lock();

	while (window1->view1->listview1->cmenu->CountItems()>0)
		window1->view1->listview1->cmenu->RemoveItem(0L);
	window1->view1->listview1->cmenu->AddItem(window1->view1->listview1->deleteItem);
	window1->view1->listview1->cmenu->AddItem(window1->view1->listview1->newfolderItem);
	window1->view1->listview1->cmenu->AddItem(window1->view1->listview1->renamefileItem);
	window1->view1->listview1->cmenu->SetTargetForItems(be_app_messenger);
	window1->Unlock();
}


void
Application1::Update_DATA_AUDIO()
{
	if (window1->view1->listview1->cmenu==NULL) return;
	window1->Lock();
	//window1->view1->stringview1->Clear();
		if ((window1->view1->CDType()==DATACD_INDEX) ||
			(window1->view1->CDType()==BOOTABLECD_INDEX) ||
			(window1->view1->CDType()==DVDVIDEO_INDEX))
			// auf DATA
			{
			window1->view1->textcontrol2->SetEnabled(false);
			//window1->view1->textcontrol1->SetEnabled(true);
			window1->view1->export_M3U->SetEnabled(false);
			window1->view1->export_APML->SetEnabled(false);
			//window1->view1->menufield2->SetEnabled(true);
			DoDataMenu();
			} else
			// auf AUDIO
		if ((window1->view1->CDType()==AUDIOCD_INDEX) || (window1->view1->CDType()==CDEXTRA_INDEX))
			{
			current_folder=base_folder;
			window1->view1->textcontrol2->SetEnabled(true);
			//window1->view1->textcontrol1->SetEnabled(false);
			window1->view1->export_M3U->SetEnabled(true);
			window1->view1->export_APML->SetEnabled(true);
			//window1->view1->menufield2->SetEnabled(false);
			DoAudioMenu();
			}
	window1->view1->listview1->SetListMode(window1->view1->CDType());
	window1->Unlock();
}


bigtime_t
xyround(float f)
{
	if ((float)(f-(bigtime_t)f)>0.000F) return ((bigtime_t)f)+1; else return (bigtime_t)f;
}


// Decodes compressed audio data to raw audio
bool
ReadData(BMediaFile *mfile, int fd, int64 start, int64 end) {
	media_multi_audio_format raw;
	int64 framesRead;
	media_format mf;
	BMediaTrack *track;
	int64 j=0;
	int64 length=0;
	const int buffersize=2*1024*1024;
	char *audioData(NULL);

	if (mfile->CountTracks()!=1) return false;
	track=mfile->TrackAt(0);
		if (track->InitCheck()!=B_OK) return false;
	if (track->EncodedFormat(&mf)==B_OK)
		printf("HELIOS: audio input is %0.0fHz/%ld channels\n", mf.u.raw_audio.frame_rate, mf.u.raw_audio.channel_count);
	mf.type=B_MEDIA_RAW_AUDIO;
	mf.u.raw_audio.frame_rate=44100.0F;					// 44.1kHz
	mf.u.raw_audio.channel_count=2;						// stereo
	mf.u.raw_audio.format=raw.B_AUDIO_SHORT; 				// 16 bit
	mf.u.raw_audio.byte_order=B_MEDIA_LITTLE_ENDIAN;			// Intel
	mf.u.raw_audio.buffer_size=buffersize;
	if (track->DecodedFormat(&mf)==B_OK)
		printf("HELIOS: audio output is %0.0fHz/%ld channels\n", mf.u.raw_audio.frame_rate, mf.u.raw_audio.channel_count);

	length=2352LL*(end-start);
	//printf("PREBUFFER: %lld/%lld\n", j, length);

	audioData = (char*)malloc(buffersize);
	framesRead=1;
	track->SeekToFrame(&start);
	while ((j<length) && (framesRead>0)) {
		track->ReadFrames(audioData, &framesRead);
		if (framesRead*4>length-j) framesRead=(length-j)/4;
		if (framesRead>0)
			write(fd, audioData,framesRead*4); 
		j+=framesRead*4;
		//printf("BUFFER: %lld/%lld\n", j, length);
	}
	for (int i=0; i<buffersize;i++) audioData[i]=0;
	while (j<length) {
		if (length-j>buffersize) framesRead=buffersize; else framesRead=length-j;
		write(fd, audioData,framesRead);
		j+=framesRead;
	}
	mfile->ReleaseAllTracks();
	free(audioData);
	return true;
}


// opens the media file
// returns true if successful, false if not
bool
OpenFile(const char *filename, int fd, int64 start, int64 end) {
	entry_ref 	ref;
	BMediaFile 	*mfile;
	BEntry *file = new BEntry(filename, true);
	bool ok;

	if (file->InitCheck()!=B_OK) {delete file; return false;}
	if (file->GetRef(&ref)!=B_OK) {file->Unset();delete file; return false;}
	file->Unset();
	delete file; 

	mfile = new BMediaFile(&ref); 
	if (mfile->InitCheck()!=B_OK) {
		OutPutRAW(filename, fd);
		delete mfile;
		return true;
	}
	ok=ReadData(mfile, fd, start, end);
	delete mfile;
	return ok;
}


typedef struct 
	audio_parameters {
		char 	*filename[99];
		int	trackcount;
		int	fd;
		int64	start[99];
		int64	end[99];
};

/*
 *	The following thread-function steps through the file list to process all items
 */
int32 AudioBufferProducer(void *data) {
	audio_parameters	*ap=(audio_parameters *)data;
	
	for (int i=0; i<ap->trackcount; i++) {
		printf("HELIOS filename[%d/%d]: %s start: %lld stop: %lld\n", i,ap->trackcount, ap->filename[i], ap->start[i], ap->end[i]);
		OpenFile(ap->filename[i],ap->fd, ap->start[i], ap->end[i]);
	}
	return 0;
}


status_t
Application1::BurnAudio(bool multisession=false)
{
		int arguments=0;
		thread_id buffer_Thread, cdrecord_Thread;
		const char **args;
		BString str="";
		BString toraw="";
		BString temp="";
		BString device="dev=";
		BString endstr="";
		BString fixate="";
		BString bproof="";

		audio_parameters	ap;
		int  cdrecord_in, cdrecord_out, cdrecord_err;

		// wait till a disc is in the drive and drive is ready
		if (WaitForDisc(deviceCV->GetWriterDevicePath())==B_TIMEOUT) {
			errtype=_T("Timeout"); // "TTIMEOUTTYPE"
			errstring=_T("The active process had to wait too long for an event."); // "TTIMEOUTERROR"
			HideStatus();
			return B_ERROR;
		}

		args = (const char **)malloc(sizeof(char *) * (700));
		arguments=0;

		// Make CDRECORD options
		CDRECORD_CommandLine(args, &arguments, multisession, false /*isStreaming*/);

		if (window1->view1->WriteCDText()) {
			BString		cdt_path=base_folder;
			char		albumtitle[161];
		
			cdt_path << "cdtext.data";
			CDTextFile	*cdt=new CDTextFile(cdt_path.String(), B_READ_WRITE | B_CREATE_FILE);
		
			switch(window1->view1->CDType()) {
				case AUDIOCD_INDEX: {
					sprintf(albumtitle, "%s",window1->view1->GetCDAlbum());
					break;
				}
				case CDEXTRA_INDEX: {
					sprintf(albumtitle, "%s",window1->view1->GetCDAlbumCDExtra());
					break;
				}
				default: {
					albumtitle[0]=0;
					break;
				}
			}
			if (strlen(albumtitle)>0)
				cdt->SetAlbum(albumtitle);
			for (int i=0; i<window1->view1->listview1->iCountItems();i++) {
				AudioRow *si=(AudioRow *)window1->view1->listview1->RowAt(i);
				if (strlen(si->GetCDTitle())>0)
					cdt->SetTrack(i+1, si->GetCDTitle());
			}
		
			cdt->SetCharacterCode(0x00);
			cdt->SetLanguage(0x09);	// english
			cdt->SetTracks(window1->view1->listview1->CountRows());
			cdt->FlushAll();
			delete cdt;
	
			args[arguments++] = strdup("-text");
		}
		
		
		if (window1->view1->WriteCDText()) {
			//args[arguments++] = strdup("-text");
			args[arguments++] = strdup("-useinfo");
			SaveCDTEXTAlbum();
		}
		for (int i=0; i<window1->view1->listview1->iCountItems();i++)
			{
			AudioRow *si=(AudioRow *)window1->view1->listview1->RowAt(i);
			str.SetTo("pregap=");
			str << /*(i==0)?(150):*/((int)(si->GetPregap()));
			args[arguments++] = strdup(str.String());

			args[arguments++] = strdup("-swab");
			args[arguments++] = strdup("-audio");
			args[arguments++] = strdup("-pad");
			
			/*if (window1->view1->WriteCDText()) {
				str.SetTo("textfile=");
				str << GenerateInfFilename(i);
				args[arguments++] = strdup(str.String());
			}*/
			
//			args[arguments++] = strdup("textfile=");
//			args[arguments++] = strdup("-useinfo");
			str.SetTo("tsize=");

			str << si->GetBytes();
			args[arguments++] = strdup(str.String());
			if (strlen(si->GetCdrecordReadyList())>1) {
				str.SetTo("index=");
				str.Append(si->GetCdrecordReadyList());
				args[arguments++] = strdup(str.String());
			}
			
			args[arguments++] = strdup("-");
			}
		args[arguments]=NULL;
	
		printArguments("cdrecord non-streaming args",args);
		//SaveCDTEXTAlbum();

		// launch cdrecord thread now
		cdrecord_Thread=pipe_command(arguments, args, cdrecord_in, cdrecord_out, cdrecord_err);
		if (cdrecord_Thread<0) {
			errtype=_T("Error"); // "TERROR"
			errstring="cdrecord\n\n";
			errstring << _T("Could not load this command line tool. Make sure it is in the correct place."); // "TCOULDNOTLOAD"
			free(args);
			HideStatus();
			return B_ERROR;
		}
		BString tmpstring;
		tmpstring << _T("Waiting") << B_UTF8_ELLIPSIS; // "TWAITING"
		SetStatus( tmpstring.String(),cdrecord_Thread);
		set_thread_priority(cdrecord_Thread,saveCV->GetBurnPriority()/*CDRECORD_PRIORITY*/);
		rename_thread(cdrecord_Thread,CDRECORDTHREADNAME);
//		working=true;
//		bf.working=&working;
//		bf.buffersize=4096000;
//		bf.in=toraw_out;
//		bf.out=cdrecord_in;
		
//		//Parameter für "toRAW" erstellen
		arguments=0;
		AudioRow *si;
		
//		toraw="";
//		toraw.Append(base_folder).append("cdtextfile.cdt");
//		CDTextFile *textfile=new CDTextFile(new BEntry(toraw.String()));
//		textfile->SetAlbum("");
		
		
		
		for (int i=0; i<window1->view1->listview1->iCountItems();i++)
			{
			si=(AudioRow *)window1->view1->listview1->RowAt(i);
			toraw.SetTo(base_folder);
			toraw.Append(si->GetFilename());
			if (!si->IsSplitted())
				ap.start[arguments]=si->GetStartFrame();
				ap.end[arguments]=si->GetEndFrame();
				ap.filename[arguments++] = strdup(toraw.String());
			}
		ap.trackcount=arguments;
		ap.fd=cdrecord_in;

	
	

		// Check files
		BEntry		entry;
		entry_ref	ref;
		BMediaFile	*mfile;
		BString		st;
	
		for (int i=0; i<ap.trackcount; i++) {
			entry.SetTo(ap.filename[i], true);
			entry.GetRef(&ref);
			mfile=new BMediaFile(&ref); 
			char nam[B_FILE_NAME_LENGTH];
			entry.GetName(nam);
			st=nam;
			printf("name=%s\n", ap.filename[i]);
			if ((entry.InitCheck()!=B_OK) || (mfile->InitCheck()!=B_OK)) {
				if (st.Length()>40) st.Remove(39, st.Length()-40);
				st << "\n\n" << _T("This audio file is not valid and cannot be written to CD. Please check the source volume and the media type."); // "TAUDIOFILEERROR"
				ErrorBox	*eb=new ErrorBox(E_RED_COLOR,	_T("Error"), // "TERROR"
															st.String(),
															_T("Abort"), // "TABORT"
															_T("Continue")); // "TCONTINUE"
				PlayErrorSound();
				if (eb->Go()==0) {
					delete mfile;
					entry.Unset();
					kill_thread(cdrecord_Thread);
					free(args);
					HideStatus();
					errtype=_T("Error"); // "TERROR"
					errstring=_T("You just interrupted and terminated the process."); // "TINTERRUPTEDBYUSER"
					return B_ERROR;
				}
			} else {
				int error=0;
				media_format mf;
				BMediaTrack *track;
				ErrorBox *eb=NULL;

				if (mfile->CountTracks()==1) {
					track=mfile->TrackAt(0);
					if (track->InitCheck()==B_OK) {
						if (track->EncodedFormat(&mf)==B_OK) {
							//printf("HELIOS: audio input is %0.0fHz/%ld channels\n", mf.u.raw_audio.frame_rate, mf.u.raw_audio.channel_count);
							if (mf.u.raw_audio.frame_rate==44100) {
								
							} else error=-1;
						} else error=-2;
					} else error=-3;
				} else error=-4;
				
				if (error<0) {
				
					switch(error) {
						case -1: {
							st << "\n\n" << _T("This audio file does not contain audio data sampled with 44100Hz."); // "TFRAMERATEERROR"
							break;
						}
						case -2: {
							st << "\n\n" << _T("The format of this file could not be recognized."); // "TFORMATREADERROR"
							break;
						}
						case -3: {
							st << "\n\n" << _T("The track of this file could not be read."); // "TTRACKREADERROR"
							break;
						}
						case -4: {
							st << "\n\n" << _T("This file contains no tracks or too much tracks."); // "TTRACKNUMBERERROR"
							break;
						}
						default: break;
					}
					eb=new ErrorBox(E_RED_COLOR, 	_T("Error"), // "TERROR"
													st.String(),
													_T("Abort"), // "TABORT"
													_T("Continue")); // "TCONTINUE"
					PlayErrorSound();
					if (eb->Go()==0) {
						delete mfile;
						entry.Unset();
						kill_thread(cdrecord_Thread);
						free(args);
						HideStatus();
						errtype=_T("Error"); // "TERROR"
						errstring=_T("You just interrupted and terminated the process."); // "TINTERRUPTEDBYUSER"
						return B_ERROR;
					}
				}
			}
			delete mfile;
			entry.Unset();
		}
		// ok. all files are ok..



		buffer_Thread=spawn_thread(AudioBufferProducer,"Buffer Thread", BUFFER_PRIORITY,(void *)(&ap));
		rename_thread(buffer_Thread,BUFFERTHREADNAME);

		arguments=0;
		while (find_thread(CDRECORDTHREADNAME)!=cdrecord_Thread) {
			arguments++;
			snooze(1000);
			if (arguments>10000) break;
		}
		resume_thread(buffer_Thread);	
		resume_thread(cdrecord_Thread);	

		ReadOutputOfCdrecord(cdrecord_out, cdrecord_Thread, window1->view1->listview1->iCountItems());

		RemoveInfFiles();
		snooze(100000);
		if (find_thread(BUFFERTHREADNAME)==buffer_Thread) kill_thread(buffer_Thread);
//		if (find_thread(TORAWTHREADNAME)==toraw_Thread) kill_thread(toraw_Thread);
//		close(toraw_in);close(toraw_out);close(toraw_err);
		close(cdrecord_in);close(cdrecord_out);close(cdrecord_err);

		if (fStatusWindow->interrupted) {
			errtype=_T("Error"); // "TERROR"
			errstring=_T("You just interrupted and terminated the process."); // "TINTERRUPTEDBYUSER"
			free(args);
			HideStatus();
			return B_ERROR;
		}

		if (deviceCV->IsWriterEjected())
			eject_media(deviceCV->GetWriterDevicePath());
		free(args);
		HideStatus();
		return B_OK;
}


status_t Application1::BlankRW(char *mode)
{
		int arguments=0;
		thread_id Thread;
		const char **args;
		BString str="";
		BString device="dev=";
		BString endstr="";
		BString tmpstring;
		int in, out, err;
		char info[40];
		// wait till a disc is in the drive and drive is ready
	
		fStatusWindow->Lock();
		if (heliosCV->IsSimulation())
			fStatusWindow->SetTitle(_T("Helios - SIMULATION")); // "TSTATUSWINDOWTITLESIMULATION"
		else
			fStatusWindow->SetTitle(_T("Helios")); // "TSTATUSWINDOWTITLE"
			
		fStatusWindow->Unlock();

		tmpstring << _T("Waiting for disc") << B_UTF8_ELLIPSIS; // "TWAITINGFORDISC"
		ShowStatus( tmpstring.String(),0);
		if (WaitForDisc(deviceCV->GetWriterDevicePath())==B_TIMEOUT) {
			errtype=_T("Timeout"); // "TTIMEOUTTYPE"
			errstring=_T("The active process had to wait too long for an event."); // "TTIMEOUTERROR"
			HideStatus();
			return B_ERROR;
		}
		
		device << deviceCV->GetWriterDeviceNumber();
		args = (const char **)malloc(sizeof(char *) * (22));
		args[arguments++] = strdup(CM->GetString(PATH_TO_CDRECORD)->String());
		args[arguments++] = strdup(device.String());
		if (heliosCV->IsSimulation()) args[arguments++] = strdup("-dummy");
		if (heliosCV->IsForcedErasing()) args[arguments++] = strdup("-force");
		args[arguments++] = strdup(mode);
		args[arguments++] = strdup("-v");
		endstr << "-speed=" << heliosCV->GetWriterSpeed();
		args[arguments++] = strdup(endstr.String());
		args[arguments]=NULL;
		
		printArguments("cdrecord non-streaming args",args);


		Thread=pipe_command(arguments, args, in, out, err);
		set_thread_priority(Thread,saveCV->GetBurnPriority()/*CDRECORD_PRIORITY*/);
		if (Thread<0) {
			errtype=_T("Error"); // "TERROR"
			errstring="cdrecord\n\n";
			errstring << _T("Could not load this command line tool. Make sure it is in the correct place."); // "TCOULDNOTLOAD"
			free(args);
			HideStatus();
			return B_ERROR;
		}
		tmpstring << _T("Waiting") << B_UTF8_ELLIPSIS; // "TWAITING"
		SetStatus( tmpstring.String(),Thread);
		rename_thread(Thread,CDRECORDTHREADNAME);
		resume_thread(Thread);
		
		size_t bytesread=0;
		char buffer[10000];
		int speed=0;
		BString s;
		
		while (Thread==find_thread(CDRECORDTHREADNAME)) {
			bytesread=read(out,(void *)buffer, 10000);
			buffer[bytesread]=0;
			
			
			cdrecord_output->AddText(buffer);
			
			
			if (bytesread>0) {

				// get speed
				if (speed==0)
				if (strstr(buffer,"at speed")) {
					s=strstr(buffer,"at speed");
					s.Remove(0,8);
					s.Remove(s.FindFirst("in"),s.Length()-s.FindFirst("in"));
					speed=atoi(s.String());
					sprintf(info,"%dx (%0.1f MB/s)",speed,176400.0*(float)speed/(1024.0*1024.0));
					SetStatusInfo(info);
				}
				

				// Fixating...
				if (strstr(buffer,"Blanking time:")) {
					SetStatus(_T("Finished Blanking CD."),-1); // Finished Blanking CD.
					SetPercentage(100.0);
				} else
				// start blanking...
				if (strstr(buffer,"Blanking")) {
					tmpstring = "";
					tmpstring << _T("Blanking CD-RW") << B_UTF8_ELLIPSIS; // "TBLANKINGCDRW"
					SetStatus( tmpstring.String(),-1);
					SetPercentage(33.33);
				}
			}
			snooze(250000);
		}
		close(in);close(out);close(err);
		fStatusWindow->Lock();
		fStatusWindow->SetTitle(_T("Helios")); // "TSTATUSWINDOWTITLE"
		fStatusWindow->Unlock();
		if (fStatusWindow->interrupted) {
			errtype=_T("Error"); // "TERROR"
			errstring=_T("You just interrupted and terminated the process."); // "TINTERRUPTEDBYUSER"
			free(args);
			HideStatus();
			return B_ERROR;
		}
		
		if (deviceCV->IsWriterEjected())
			eject_media(deviceCV->GetWriterDevicePath());

		HideStatus();
		free(args);
		return B_OK;
}


// verschiebt einen Audio-eintrag um eine position nach oben
void Application1::MoveUp()
{
/*
has_project_changed=true;
int32 to, from;
//printf("%d\n",window1->view1->listview1->CountItems());
window1->Lock();
		for (int32 i=1; i<window1->view1->listview1->iCountItems();i++)
 			if (window1->view1->listview1->IsSelected(i)) {
 				to=i;
 				from=i;
  				to--;
				while ((((AudioRow *)(window1->view1->listview1->RowAt(to)))->IsSplitted()) ||
 					(((AudioRow *)(window1->view1->listview1->RowAt(to)))->GetSplitIndex()==0)) {
   					to--;
				};
			 	// move item when it is NOT a part of a splitted track
				if (!((((AudioRow *)(window1->view1->listview1->RowAt(from)))->IsSplitted()) ||
 					(((AudioRow *)(window1->view1->listview1->RowAt(from)))->GetSplitIndex()==0)))
					if (to>=0) window1->view1->listview1->MoveRow(from,to);
			}
window1->view1->listview1->Invalidate();
window1->Unlock();
*/
}

// verschiebt einen Audio-eintrag um eine position nach unten
void Application1::MoveDown()
{
/*
has_project_changed=true;
int32 max=window1->view1->listview1->CountItems();
int32 to;
window1->Lock();
		  for (int32 i=window1->view1->listview1->CountItems()-2;i>=0;i--)
 			if (window1->view1->listview1->IsItemSelected(i)) {
 				to=i;
  				to++;
				while ((((AudioItem *)(window1->view1->listview1->ItemAt(to)))->IsSplitted()) ||
 					(((AudioItem *)(window1->view1->listview1->ItemAt(to)))->IsParentSplitterTrack())) {
  				to++;
				};
			 	// move item when it is NOT a part of a splitted track
				if (!((((AudioItem *)(window1->view1->listview1->ItemAt(i)))->IsSplitted()) ||
 					(((AudioItem *)(window1->view1->listview1->ItemAt(i)))->IsParentSplitterTrack())))
					if (to<max) window1->view1->listview1->MoveItem(i,to);
			} 
window1->view1->listview1->Invalidate();
window1->Unlock();
*/
}


status_t
Application1::ReadImage(BString *filename, bool alloweject) {

		int in, out, err;
		thread_id Thread;
		const char **args;
		BString str="f=";
		BString device="dev=";
		BString endstr="";
		int arguments=0;
		BEntry entry;
		char info[40];
		BString tmpstring;

		// wait till a disc is in the drive and drive is ready
	
		tmpstring << _T("Waiting for disc") << B_UTF8_ELLIPSIS; // "TWAITINGFORDISC"
		ShowStatus( tmpstring.String(),0); 
		if (WaitForDisc(deviceCV->GetReaderDevicePath())==B_TIMEOUT) {
			errtype=_T("Timeout"); // "TTIMEOUTTYPE"
			errstring=_T("The active process had to wait too long for an event."); // "TTIMEOUTERROR"
			HideStatus();
			return B_ERROR;
		}

		device << deviceCV->GetReaderDeviceNumber();
		args = (const char **)malloc(sizeof(char *) * (22));
		args[arguments++] = strdup(CM->GetString(PATH_TO_READCD)->String());
		args[arguments++] = strdup(device.String());
		str << filename->String();
		args[arguments++] = strdup(str.String());
//		args[arguments++] = strdup("-v");
		args[arguments]=NULL;

		printArguments("readcd args",args);

		// delete image file if it exists
		entry.SetTo(filename->String(), true);
		if (entry.Exists()) {
			entry.Remove();
		}
		entry.Unset();

		Thread=pipe_command(arguments, args, in, out, err);
		set_thread_priority(Thread,16);
		if (Thread<0) {
			errtype=_T("Error"); // "TERROR"
			errstring="readcd\n\n";
			errstring << _T("Could not load this command line tool. Make sure it is in the correct place."); // "TCOULDNOTLOAD"
			free(args);
			HideStatus();
			return B_ERROR;
		}
		tmpstring = "";
		tmpstring << _T("Reading CD image") << B_UTF8_ELLIPSIS; // "TREADINGCD"
		SetStatus( tmpstring.String(),Thread);
		rename_thread(Thread,READCDTHREADNAME);
		resume_thread(Thread);
		
		size_t bytesread=0;
		char buffer[10000];
		float length=-1;
		float progressed, olddone=0;
		BString s;
		BStopWatch *watch=new BStopWatch("MBps",true);

		SetStatusInfo("--- MB/s");
		
		do {
			bytesread=read(err,(void *)buffer, 10000);
			buffer[bytesread]=0;
			if (bytesread>0) {
//				printf(">>>%s\n",buffer);
				if (length<0)
				if (strstr(buffer,"end:")) {
					s=strstr(buffer,"end:");
					s.RemoveFirst("end:");
					//s.Remove(s.FindFirst("Blocks"),s.Length());
					length=atof(s.String());
				} // strstr			

				if (strstr(buffer,"addr:")) {
					s=strstr(buffer,"addr:");
					s.Remove(0,5);
					s.Remove(s.FindFirst("cnt:"),s.Length()-s.FindFirst("cnt:"));
					progressed=atof(s.String());
					if (length>0) {
						if (progressed>olddone+3072) {
							sprintf(info,"%0.1f MB/s",2048.0*(progressed-olddone)/(watch->ElapsedTime()*1.048576));
							olddone=progressed;
							SetStatusInfo(info);
							watch->Reset();
						}
						SetPercentage(100*progressed/length);
					}
				} // strstr			
			}
			snooze(100000);
		} while (Thread==find_thread(READCDTHREADNAME)) ;

		close(in);close(out);close(err);

		delete watch;
		if (fStatusWindow->interrupted) {
			errtype=_T("Error"); // "TERROR"
			errstring = _T("You just interrupted and terminated the process."); // "TINTERRUPTEDBYUSER"
			free(args);
			HideStatus();
			return B_ERROR;
		}
		if (alloweject)
			if (deviceCV->IsReaderEjected())
				eject_media(deviceCV->GetReaderDevicePath());
			
		free(args);
		HideStatus();
		//delete filename;
		return B_OK;
}


status_t
Application1::MakeImageOnly(const char *filename) {
	status_t	error;
	BList		*list;
	bool		bfs=false;
	
	switch(window1->view1->CDType()) {
		case DATACD_INDEX: {
			list=window1->view1->datapanel->GetFilesystem();
			if (list->CountItems()==1)
				if (((BString *)list->ItemAt(0))->Compare("BFS")==0)
					bfs=true;
			
			for (int i=list->CountItems()-1; i>=0; i--)
				delete ((BString *)list->RemoveItem(i));
			delete list;
			break;
		}

		case BOOTABLECD_INDEX: {
			list=window1->view1->bootablepanel->GetFilesystem();
			if (list->CountItems()==1)
				if (((BString *)list->ItemAt(0))->Compare("BFS")==0)
					bfs=true;
			
			for (int i=list->CountItems()-1; i>=0; i--)
				delete ((BString *)list->RemoveItem(i));
			delete list;
			break;
		}
		case DVDVIDEO_INDEX: {
			list=window1->view1->dvdvideopanel->GetFilesystem();
			if (list->CountItems()==1)
				if (((BString *)list->ItemAt(0))->Compare("BFS")==0)
					bfs=true;
			
			for (int i=list->CountItems()-1; i>=0; i--)
				delete ((BString *)list->RemoveItem(i));
			delete list;
			break;
		}
		default:break;
	}
	
	if (bfs) {
		error=MakeBFSImage(strdup(filename));
	} else {
		error=MakeMKISOImage(strdup(filename));
	}

	return error;
}


void
Application1::eject_media(const char * dev)
{
	int 		fd;
	status_t	status;

	fd = open(dev, 0);
	// printf("ejecting media from device %s\n", dev);
	status = ioctl(fd, B_EJECT_DEVICE, 0, 0);
	close(fd);
}


void
Application1::load_media(const char * dev)
{
	int 		fd;
	status_t	status;

	fd = open(dev, 0);
	// printf("loading media from device %s\n", dev);
	status = ioctl(fd, B_LOAD_MEDIA, 0, 0);
	close(fd);
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
status_t
Application1::CopyAudioCD() {
//	BString		oldbase=base_folder;
	
//	base_folder=pathCV->GetTemporaryPath();
//	base_folder=&AppendTrailingSlash(base_folder);
	if (ReadAudioCD()>0) {
		eject_media(deviceCV->GetReaderDevicePath());
		
		if (window1->view1->CDType()!=AUDIOCD_INDEX)		
			if (window1->Lock()) {
				window1->view1->SetCDType(AUDIOCD_INDEX);
				window1->Unlock();
				ShowFolder(true);
			}
		
		return BurnAudio();
	} else {
		errtype=_T("Error"); // "TERROR"
		errstring=_T("Error while copying a file."); // "TCOPYERROR"
		return -1;
	}
//	base_folder=oldbase;
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


status_t
Application1::CopyDataCD()
{
	status_t returnValue = B_OK;

	if (WaitForDisc(deviceCV->GetWriterDevicePath())==B_TIMEOUT) {
		errtype=_T("Error"); // "TERROR"
		errstring=_T("No CD in the drive!");	// "TNOCDINDRIVE"
		return B_ERROR;
	}

	fStatusWindow->Lock();
	if (heliosCV->IsSimulation()) {
		fStatusWindow->SetTitle(_T("Helios - SIMULATION")); // "TSTATUSWINDOWTITLESIMULATION"
	} else {
		fStatusWindow->SetTitle(_T("Helios")); // "TSTATUSWINDOWTITLE"
	}
	fStatusWindow->Unlock();

	BString filename = pathCV->GetTemporaryPath();
	AppendTrailingSlash(&filename);
	filename.Append(pathCV->GetDefaultImageName());

	const size_t cdImageSize = GetDeviceSize(deviceCV->GetReaderDevicePath());
	if (!FileCopy(deviceCV->GetReaderDevicePath(),filename.String(), cdImageSize)) {
		HideStatus();
		errtype=_T("Error"); // "TERROR"
		errstring=_T("Error while reading the CD image."); // "TCDIMAGEREADERROR"
		return B_ERROR;
	}

	// Eject the disk (if permitted).
	if (deviceCV->IsReaderEjected()) {
		eject_media(deviceCV->GetReaderDevicePath());
	}

	// wait till door has been opened
	BString tmpstring;
	tmpstring << _T("Waiting") << B_UTF8_ELLIPSIS; // "TWAITING"
	SetStatus( tmpstring.String(),0);
	char info[40];
	int	timeout = applicationCV->GetTimeoutLength() * 50;
	if ((deviceCV->GetWriterNumber()==deviceCV->GetReaderNumber()) && (applicationCV->IsTimeout())) {
		while (DeviceStatus(deviceCV->GetReaderDevicePath()) != E_DOOR_OPEN) {
			snooze(100000);
			timeout--;
			if (timeout==0) {
				HideStatus();
				errtype=_T("Time out!"); // "TTIMEOUT"
				errstring=_T("The active process had to wait too long for an event."); // "TTIMEOUTERROR"
				return B_ERROR;
			}
			if (fStatusWindow->interrupted) {
				HideStatus();
				errtype=_T("Error"); // "TERROR"
				errstring=_T("You just interrupted and terminated the process."); // "TINTERRUPTEDBYUSER"
				return B_ERROR;
			}
			sprintf(info,"%d\"",timeout/10);
			SetStatusInfo(info);
			SetPercentage((float)timeout*100/((float)applicationCV->GetTimeoutLength()*10));
		};
	}

	HideStatus();

	bool okToBurn = true;
	if (deviceCV->GetWriterNumber()==deviceCV->GetReaderNumber()) {
		// Ask the user to insert the disk to be written to.
		ErrorBox *eb = new ErrorBox(E_BLUE_COLOR, "",	_T("Please insert the disc to burn"),	// "TPLEASEINSERTDISC"
														_T("Ok"),
														_T("Cancel")); 
		eb->SetShortcut(1, B_ESCAPE);
		okToBurn = (eb->Go() == 0);
	}

	if (okToBurn) {
		returnValue = BurnImage(filename);
	}

	return returnValue;
}

status_t
Application1::CopyOnTheFlyAudio()
{
		int arguments=0;
		thread_id cdrecord_Thread, cdda2wav_Thread;
		const char **args;
		BString str="";
		BString device="dev=";
		BString filesystem="";
		int cdrecord_in, cdrecord_out, cdrecord_err, cdda2wav_in, cdda2wav_out, cdda2wav_err;

		// wait till a disc is in the drive and drive is ready
		if (WaitForDisc(deviceCV->GetWriterDevicePath())==B_TIMEOUT) {
			errtype=_T("Timeout"); // "TTIMEOUTTYPE"
			errstring=_T("The active process had to wait too long for an event."); // "TTIMEOUTERROR"
			HideStatus();
			return B_ERROR;
		}
		if (WaitForDisc(deviceCV->GetReaderDevicePath())==B_TIMEOUT) {
			errtype=_T("Timeout"); // "TTIMEOUTTYPE"
			errstring=_T("The active process had to wait too long for an event."); // "TTIMEOUTERROR"
			HideStatus();
			return B_ERROR;
		}
		
		fStatusWindow->Lock();
		if (heliosCV->IsSimulation()) {
			fStatusWindow->SetTitle(_T("Helios - SIMULATION")); // "TSTATUSWINDOWTITLESIMULATION"
		} else {
			fStatusWindow->SetTitle(_T("Helios")); // "TSTATUSWINDOWTITLE"
		}
		fStatusWindow->Unlock();

		args = (const char **)malloc(sizeof(char *) * (36));
		arguments=0;

		// Make CDRECORD options
		CDRECORD_CommandLine(args, &arguments, window1->view1->IsMultisession(),
				false /*isStreaming*/);

		size_t	volumesize=GetDeviceSize(deviceCV->GetReaderDevicePath());
		str << "tsize=" << volumesize;
		args[arguments++] = strdup(str.String());
		printf("HELIOS: IMAGESIZE IS %s\n",str.String());
		args[arguments++] = strdup("-data");
		args[arguments++] = strdup(deviceCV->GetReaderDevicePath());
		args[arguments]=NULL;

		printArguments("cdrecord non-streaming args",args);

		// launch cdrecord thread now
		cdrecord_Thread=pipe_command(arguments, args, cdrecord_in, cdrecord_out, cdrecord_err);
		set_thread_priority(cdrecord_Thread,saveCV->GetBurnPriority()/*CDRECORD_PRIORITY*/);
		if (cdrecord_Thread<0) {
			errtype=_T("Error"); // "TERROR"
			errstring="cdrecord\n\n";
			errstring << _T("Could not load this command line tool. Make sure it is in the correct place."); // "TCOULDNOTLOAD"
			free(args);
			HideStatus();
			return B_ERROR;
		}
		
		BString tmpstring;
		tmpstring << _T("Waiting") << B_UTF8_ELLIPSIS; // "TWAITING"
		SetStatus( tmpstring.String(),cdrecord_Thread);
		rename_thread(cdrecord_Thread,CDRECORDTHREADNAME);


		// Make CDRECORD options
		device="";
		device << deviceCV->GetReaderDeviceNumber();
		arguments=0;
		args[arguments++] = strdup(CM->GetString(PATH_TO_CDDA2WAV)->String());
		args[arguments++] = strdup("-D");
		args[arguments++] = strdup(device.String());

		args[arguments++] = strdup("-t");
		args[arguments++] = strdup("1+");

		args[arguments++] = strdup("-O");
		args[arguments++] = strdup("cdr");

		args[arguments]=NULL;

		printArguments("cdda2wav streaming args",args);

		// launch cdda2wav thread now
		cdda2wav_Thread=pipe_command(arguments, args, cdda2wav_in, cdda2wav_out, cdda2wav_err);
		set_thread_priority(cdda2wav_Thread,saveCV->GetBurnPriority()/*CDRECORD_PRIORITY*/);
		if (cdda2wav_Thread<0) {
			errtype=_T("Error"); // "TERROR"
			errstring="cdda2wav\n\n";
			errstring << _T("Could not load this command line tool. Make sure it is in the correct place."); // "TCOULDNOTLOAD"
			free(args);
			HideStatus();
			return B_ERROR;
		}
		rename_thread(cdda2wav_Thread,CDDA2WAVTHREADNAME);
		resume_thread(cdrecord_Thread);	

		resume_thread(cdda2wav_Thread);	


		ReadOutputOfCdrecord(cdrecord_out, cdrecord_Thread);
		close(cdrecord_in);close(cdrecord_out);close(cdrecord_err);

		if (fStatusWindow->interrupted) {
			errtype=_T("Error"); // "TERROR"
			errstring=_T("You just interrupted and terminated the process."); // "TINTERRUPTEDBYUSER"
			free(args);
			HideStatus();
			return B_ERROR;
		}
			
		if (deviceCV->IsWriterEjected())
			eject_media(deviceCV->GetWriterDevicePath());
		if (deviceCV->IsReaderEjected())
			eject_media(deviceCV->GetReaderDevicePath());
		free(args);
		HideStatus();
		return B_OK;
}

status_t
Application1::CopyOnTheFly()
{
	// wait till a disc is in the drive and drive is ready
	if (WaitForDisc(deviceCV->GetWriterDevicePath())==B_TIMEOUT) {
		errtype=_T("Timeout"); // "TTIMEOUTTYPE"
		errstring=_T("The active process had to wait too long for an event."); // "TTIMEOUTERROR"
		HideStatus();
		return B_ERROR;
	}
	if (WaitForDisc(deviceCV->GetReaderDevicePath())==B_TIMEOUT) {
		errtype=_T("Timeout"); // "TTIMEOUTTYPE"
		errstring=_T("The active process had to wait too long for an event."); // "TTIMEOUTERROR"
		HideStatus();
		return B_ERROR;
	}

	fStatusWindow->Lock();
	if (heliosCV->IsSimulation()) {
		fStatusWindow->SetTitle(_T("Helios - SIMULATION")); // "TSTATUSWINDOWTITLESIMULATION"
	} else {
		fStatusWindow->SetTitle(_T("Helios")); // "TSTATUSWINDOWTITLE"
	}
	fStatusWindow->Unlock();

	const char **args = (const char **)malloc(sizeof(char *) * (36));
	int arguments=0;

	// Make CDRECORD options
	CDRECORD_CommandLine(args, &arguments, window1->view1->IsMultisession(),
			false /*isStreaming*/);

	const size_t volumesize = GetDeviceSize(deviceCV->GetReaderDevicePath());
	BString str = "";
	str << "tsize=" << volumesize;
	args[arguments++] = strdup(str.String());
	printf("HELIOS: IMAGESIZE IS %s\n",str.String());
	args[arguments++] = strdup("-data");
	args[arguments++] = strdup(deviceCV->GetReaderDevicePath());
	args[arguments] = NULL;

	printArguments("cdrecord non-streaming args",args);

	// launch cdrecord thread now
	int cdrecord_in, cdrecord_out, cdrecord_err;
	thread_id cdrecord_Thread = pipe_command(arguments, args, cdrecord_in, cdrecord_out, cdrecord_err);
	set_thread_priority(cdrecord_Thread, saveCV->GetBurnPriority()/*CDRECORD_PRIORITY*/);
	if (cdrecord_Thread < 0) {
		errtype=_T("Error"); // "TERROR"
		errstring="cdrecord\n\n";
		errstring << _T("Could not load this command line tool. Make sure it is in the correct place."); // "TCOULDNOTLOAD"
		free(args);
		HideStatus();
		return B_ERROR;
	}

	{
		BString tmpstring;
		tmpstring << _T("Waiting") << B_UTF8_ELLIPSIS; // "TWAITING"
		SetStatus(tmpstring.String(), cdrecord_Thread);
	}

	rename_thread(cdrecord_Thread, CDRECORDTHREADNAME);
	resume_thread(cdrecord_Thread);	

	size_t bytesread = 0;
	char buffer[10000];
	int speed=0;
	int track=1;
	float length=0;	// in MB
	float op=0,progressed=0;
	char info[40];
	char info2[40];
	BString s;

	do {
		bytesread = read(cdrecord_out,(void *)buffer, 10000);
		//printf("HELIOSBUFFER: >%s<\n",buffer);

		if (bytesread > 0) {

			buffer[bytesread] = 0;
			cdrecord_output->AddText(buffer);
			// get speed
			if (speed==0) {
				if (strstr(buffer,"at speed")) {
					s=strstr(buffer,"at speed");
					s.Remove(0,8);
					s.Remove(s.FindFirst("in"),s.Length()-s.FindFirst("in"));
					speed=atoi(s.String());
					sprintf(info,"%dx (%0.1f MB/s)",speed,153600.0*(float)speed/(1024.0*1024.0));
					SetStatusInfo(info);
				}
			}

			// Performing OPC...
			if (strstr(buffer,"Performing OPC...")) {
				BString tmpstring = "";
				tmpstring << _T("Adjusting laser power") << B_UTF8_ELLIPSIS; // "TPERFORMINGOPC"
				SetStatus( tmpstring.String(),-1);
			}

			// finished fixating...
			if (strstr(buffer,"Fixating time:")) {
				SetStatus(_T("Finished writing CD."),-1); // "TFINISHEDWRITING"
			}

			// start burning...
			if (strstr(buffer,"Starting new track")) {
				BString tmpstring = "";
				tmpstring << _T("Burning data CD") << B_UTF8_ELLIPSIS; // "TBURNINGDATACD"
				SetStatus( tmpstring.String(),-1); // "TBURNINGDATACD"
			}

			// Fixating...
			if (strstr(buffer,"Fixating...")) {
				BString tmpstring = "";
				tmpstring << _T("Fixating disc") << B_UTF8_ELLIPSIS; // "TFIXATING"
				SetStatus( tmpstring.String(),-1);
			}

			// get track number
			if (strstr(buffer,"Track")) {
				s=strstr(buffer,"Track");
				s.RemoveFirst("Track");
				s.Remove(s.FindFirst(":"),s.Length()-s.FindFirst(":"));
				track=atoi(s.String());
			}		

			// get track length
			if (length==0)
			if (strstr(buffer,"of")) {
				s=strstr(buffer,"of");
				s.RemoveFirst("of");
				s.Remove(s.FindFirst("MB"),s.Length()-s.FindFirst("MB"));
				length=atof(s.String());
			}		

			// get write position
			if (strstr(buffer,"Track")) {
				s=strstr(buffer,"Track");
				s.Remove(0,9);
				s.Remove(s.FindFirst("of"),s.Length()-s.FindFirst("of"));
				progressed=atof(s.String());
				if (op<progressed) {
					op=progressed;
					if (length>0) {
						sprintf(info,"%0.1f%%",100.0*(float)progressed/(float)length);
						sprintf(info2,"%s %d",_T("Track"),track); // "TTRACK"
						SetStatusInfo(info, info2);
						SetPercentage(100.0*(float)progressed/(float)length);
					}
				}
			}		
		}

		snooze(250000);
	} while (cdrecord_Thread==find_thread(CDRECORDTHREADNAME));

	close(cdrecord_in);close(cdrecord_out);close(cdrecord_err);
	if (fStatusWindow->interrupted) {
		errtype=_T("Error"); // "TERROR"
		errstring=_T("You just interrupted and terminated the process."); // "TINTERRUPTEDBYUSER"
		free(args);
		HideStatus();
		return B_ERROR;
	}

	if (deviceCV->IsWriterEjected()) {
		eject_media(deviceCV->GetWriterDevicePath());
	}
	if (deviceCV->IsReaderEjected()) {
		eject_media(deviceCV->GetReaderDevicePath());
	}
	free(args);
	HideStatus();
	return B_OK;
}


// Returns size of free space of a volume by passing the path of the device only.
size_t
FreeVolumeSpace(const char *path)
{
	fs_info	info;
	dev_t	device;

	device=dev_for_path(path);
	if (device >= 0) {
		if (fs_stat_dev(device, &info) == 0) {
			return (info.free_blocks * info.block_size);
		}
	}
	return 0;	
}


status_t
Application1::MakeMKISOImage(const char *filename)
{
		const char **args;
		int arguments=0;
		thread_id Thread;
		BString lastarg, str="";
		BString filesystem="";
		int in, out, err;

		// allocate some memory for the command line array
		args = (const char **)malloc(sizeof(char *) * (50));
		// fill array with arguments
		if (MKISOFS_CommandLine(args, &arguments, false, filename)!=B_OK) {
			free(args);
			HideStatus();
			return B_ERROR;
		}

		fStatusWindow->Lock();
		fStatusWindow->interrupted=false;
		fStatusWindow->Unlock();

		// and _please_ launch a new thread!
		Thread=pipe_command(arguments, args, in, out, err);
		set_thread_priority(Thread,16);
		if (Thread<0) {
			errtype=_T("Error"); // "TERROR"
			errstring="mkisofs\n\n";
			errstring << _T("Could not load this command line tool. Make sure it is in the correct place."); // "TCOULDNOTLOAD"
			free(args);
			HideStatus();
			return B_ERROR;
		}
		BString tmpstring;
		tmpstring << _T("Writing image file") << B_UTF8_ELLIPSIS; // "TCREATINGIMAGEFILE"
		ShowStatus( tmpstring.String(),Thread);
		rename_thread(Thread,MKISOFSTHREADNAME);
		resume_thread(Thread);
		
		size_t bytesread=0;
		char buffer[10000];
		char prev;
		char info[40];
		int32 length;
		float percentage;
		
		while (Thread==find_thread(MKISOFSTHREADNAME)) {
			bytesread=read(err,(void *)buffer, 10000);
			buffer[bytesread]=0;
			if (bytesread>0) {
				mkisofs_output->AddText(buffer);
				if (strstr(buffer," done, estimate")) {
					buffer[(length=(bytesread-strlen(strstr(buffer," done, estimate"))-1))]=0;
					if (buffer[length-1]==' ') {
						prev=buffer[length-2];	
						buffer[length-2]='0';
						buffer[length-1]=prev;
					}
					percentage=atof(buffer);
					sprintf(info,"%0.1f%%",percentage);
					SetStatusInfo(info);
					SetPercentage(percentage);
				} // strstr			
			}
			snooze(100000);
		}
		close(in);close(out);close(err);
		free(args);
		HideStatus();

		if (fStatusWindow->interrupted) {
			errtype=_T("Error"); // "TERROR"
			errstring=_T("You just interrupted and terminated the process."); // "TINTERRUPTEDBYUSER"
			return B_ERROR;
		}
			
		return B_OK;
}


void
Application1::SaveSettings() {
	BMessage	*msg=new BMessage();

	configW->Lock();
	window1->Lock();

	// HeliosConfigView
	heliosCV->GetSettings(msg);
	
	CM->SetMessage("Preferences", msg);
	
	// DeviceConfigView
	CM->SetBool(EJECT_WRITER, deviceCV->IsWriterEjected());
	CM->SetBool(EJECT_READER, deviceCV->IsReaderEjected());
	CM->SetBool(TWODRIVECOPY, deviceCV->IsTwoDriveCopy());
	CM->SetInt8(WRITERDEVICE, deviceCV->GetWriterNumber());
	CM->SetInt8(READERDEVICE, deviceCV->GetReaderNumber());
	CM->SetInt8(SCANNINGMETHOD, deviceCV->GetScanMethod());
	CM->SetBool(SHOWALLDEVICES, deviceCV->GetShowsAllDevices());
	
	// PathConfigView
	CM->SetString(TEMPORARY_PATH, pathCV->GetTemporaryPath());
	CM->SetString(PROJECT_PATH, pathCV->GetProjectPath());
	CM->SetString(DEFIMAGENAME, pathCV->GetDefaultImageName());
	CM->SetString(DEFMOUNTPOINT, pathCV->GetDefaultMountPoint());
	
	// ImageConfigView
	CM->SetString(I_PUBLISHER, imageCV->GetPublisher());
	CM->SetString(I_PREPARER, imageCV->GetPreparer());
	CM->SetString(I_APPLICATION, imageCV->GetApplication());
	
	// FilesystemConfigView
	CM->SetBool(F_ISO9660_CB, fspanelCV->GetISO9660());
	CM->SetBool(F_JOLIET_CB, fspanelCV->GetJoliet());
	CM->SetBool(F_RR_CB, fspanelCV->GetRockRidge());
	CM->SetBool(F_HFS_CB, fspanelCV->GetHFS());
	CM->SetBool(F_UDF_CB, fspanelCV->GetUDF());
	
	CM->SetBool(F_83FN_RB, fspanelCV->GetISO96608Dot3());
	CM->SetBool(F_31FN_RB, fspanelCV->GetISO966031());
	CM->SetBool(F_37FN_RB, fspanelCV->GetISO966037());
	CM->SetBool(F_JOLDEF_RB, fspanelCV->GetJolietDefault());
	CM->SetBool(F_JOLUNI_RB, fspanelCV->GetJolietUnicode());
	CM->SetBool(F_RRDEF_RB, fspanelCV->GetRockRidgeDefault());
	CM->SetBool(F_RRRAT_RB, fspanelCV->GetRockRidgeRationalized());
	CM->SetBool(F_UDFDEF_RB, fspanelCV->GetUDFDefault());
	CM->SetBool(F_UDFDVD_RB, fspanelCV->GetUDFDVDVideo());
	CM->SetBool(F_MAPPING_CB, fspanelCV->GetMapping());
	CM->SetString(F_MAPPING_TC, fspanelCV->GetMappingFile());
		
	// StandardConfigView
	CM->SetInt32(S_FILESYSTEM, standardCV->GetFilesystem());
	CM->SetString(S_VOLUMENAME, standardCV->GetVolumeName());
	CM->SetInt8(S_CDTYPE, standardCV->GetCDType());
	CM->SetString(S_PUBLISHER, standardCV->GetPublisher());
	CM->SetString(S_PREPARER, standardCV->GetPreparer());
	CM->SetString(S_APPLICATION, standardCV->GetApplication());
	//CM->SetString(S_TEMPPATH, standardCV->GetTemporaryPath());
	//CM->SetString(S_PROJECTPATH, standardCV->GetProjectPath());
	
	// ApplicationConfigView
	CM->SetBool(APP_TIMEOUT, applicationCV->IsTimeout());
	CM->SetInt16(APP_TIMEOUTLENGTH, applicationCV->GetTimeoutLength());
	CM->SetBool(APP_AUTOUPDATE, applicationCV->IsAutoUpdating());
	CM->SetInt16(APP_UPDATEFREQUENCY, applicationCV->GetUpdateFrequency());
	
	// SoundConfigView
	CM->SetBool(BEEPWHENWRITTEN, soundCV->HasFinishedWritingSound());
	CM->SetBool(BEEPWHENIMAGE, soundCV->HasFinishedImageSound());
	CM->SetBool(BEEPWHENERROR, soundCV->HasErrorSound());
	
	// SaveConfigView
	CM->SetBool(STREAMING_MODE, saveCV->IsStreamed());
	CM->SetBool(SAVEWHENQUIT, saveCV->IsSavedWhenQuit());
	CM->SetInt8(EXITMODE, (int8)saveCV->GetExitMode());
	CM->SetBool(AUTOUPDATE, saveCV->AutoUpdate());
//MM	CM->SetInt32(LANGUAGE_ID, saveCV->GetLanguage());
//MM	CM->SetBool(BUBBLEHELP, saveCV->BalloonHelp());
	CM->SetInt32(BURNPRIORITY, saveCV->GetBurnPriority());

	// DATA-CD
	CM->SetInt32(DATACD_FILESYSTEM, window1->view1->GetFilesystem());
	CM->SetString(DATACD_VOLUMENAME, window1->view1->GetVolumeName());
	CM->SetBool(DATACD_MULTISESSION, window1->view1->IsMultisession());
	// AUDIO-CD
	CM->SetBool(AUDIOCD_WRITECDTEXT, window1->view1->WriteCDText());
	CM->SetBool(AUDIOCD_SCMS, window1->view1->ProtectAudioCD());
	CM->SetString(AUDIOCD_ALBUMNAME, window1->view1->GetCDAlbum());
	// BOOTABLE-CD
	CM->SetString(BOOTCD_VOLUMENAME, window1->view1->GetVolumeNameBootCD());
	CM->SetString(BOOTCD_FLOPPYIMAGE, window1->view1->GetImageNameBootCD());
	CM->SetInt32(BOOTCD_FILESYSTEM, window1->view1->GetFilesystemBootCD());
	// CD-EXTRA
	CM->SetString(CDEXTRA_DATAIMAGE, window1->view1->GetImageNameCDExtra());
	CM->SetString(CDEXTRA_ALBUMNAME, window1->view1->GetCDAlbumCDExtra());
	CM->SetBool(CDEXTRA_WRITECDTEXT, window1->view1->WriteCDTextCDExtra());
	CM->SetInt32(CDEXTRA_FILESYSTEM, window1->view1->GetFilesystemCDExtra());
	CM->SetString(CDEXTRA_VOLUMENAME, window1->view1->GetVolumeNameCDExtra());
	// DVDVIDEO
	CM->SetInt32(DVDVIDEO_FILESYSTEM, window1->view1->GetFilesystemDVDVideo());
	CM->SetString(DVDVIDEO_VOLUMENAME, window1->view1->GetVolumeNameDVDVideo());
	
	// WINDOW POSITIONS AND SIZES
	CM->SetRect(MAINWINDOW_RECT, window1->Frame());
	CM->SetRect(WINDOW2_RECT, window2->Frame());
	CM->SetRect(PREFERENCESWINDOW_RECT, configW->Frame());
	CM->SetRect(CDRECORDLOGWINDOW_RECT, cdrecord_output->Frame());
	CM->SetRect(MKISOFSLOGWINDOW_RECT, mkisofs_output->Frame());
	
	// COLORS
	CM->SetColor(BURNING_STATUS_BAR_COLOR, colorCV->GetColor(0));
		if (fStatusWindow->Lock()) {
			fStatusWindow->SetBarColor(colorCV->GetColor(0));
			fStatusWindow->Unlock();
		}
	CM->SetColor(CDSIZE_STATUS_BAR_COLOR, colorCV->GetColor(1));
		if (window1->Lock()) {
			window1->view1->box2->SetBarColor(colorCV->GetColor(1));
			window1->Unlock();
		}
	CM->SetColor(LOGWIN_BG_COLOR, colorCV->GetColor(2));
	CM->SetColor(LOGWIN_FG_COLOR, colorCV->GetColor(3));
		if (cdrecord_output->Lock()) {
			cdrecord_output->SetColors(colorCV->GetColor(3), colorCV->GetColor(2));
			cdrecord_output->Unlock();
		}
		if (mkisofs_output->Lock()) {
			mkisofs_output->SetColors(colorCV->GetColor(3), colorCV->GetColor(2));
			mkisofs_output->Unlock();
		}
	CM->SetColor(LISTSELECTION_COLOR, colorCV->GetColor(4));
	if (window1->Lock()) {
		window1->view1->listview1->SetSelectionColor(colorCV->GetColor(4));
		window1->Unlock();
	}
	CM->SetColor(LISTBACKGROUND_COLOR, colorCV->GetColor(5));
	if (window1->Lock()) {
		window1->view1->listview1->SetBackgroundColor(colorCV->GetColor(5));
		window1->Unlock();
	}


	window1->view1->listview1->SaveAllStates();
	
	// ProtocolWindow Settings
	if (cdrecord_output->Lock()) {
		CM->SetBool(CDRECORD_LOGWIN_VISIBLE, !cdrecord_output->IsHidden());
		CM->SetBool(CDRECORD_LOG, cdrecord_output->DoesLogToFile());
		CM->SetString(CDRECORD_LOGFILENAME, cdrecord_output->GetLogFileName());
		cdrecord_output->Unlock();
	}
	
	if (mkisofs_output->Lock()) {
		CM->SetBool(MKISOFS_LOGWIN_VISIBLE, !mkisofs_output->IsHidden());
		CM->SetBool(MKISOFS_LOG, mkisofs_output->DoesLogToFile());
		CM->SetString(MKISOFS_LOGFILENAME, mkisofs_output->GetLogFileName());
		mkisofs_output->Unlock();
	}
	
	// Language settings.
#if defined(_BEOS_R5_BUILD_) || defined(_BEOS_HAIKU_BUILD_)
	BString langTag = langCV->GetLanguageTag();
	CM->SetString(CURRENT_LANG_TAG, &langTag);
#endif

	CM->WriteConfiguration();
	configW->Unlock();
	window1->Unlock();
	
	delete msg;
	return;

}



void
Application1::ReadSettings() {
	BMessage *msg;
	
	msg=CM->GetMessage("Preferences");

	// Set default values...

	// Paths of command line tools
	if (!CM->HasData(PATH_TO_MKISOFS))		CM->SetString(PATH_TO_MKISOFS, "/boot/home/config/bin/mkisofs");
	if (!CM->HasData(PATH_TO_CDRECORD))	CM->SetString(PATH_TO_CDRECORD, "/boot/home/config/bin/cdrecord");
	if (!CM->HasData(PATH_TO_MKBFS))		CM->SetString(PATH_TO_MKBFS, "/boot/beos/bin/mkbfs");
	if (!CM->HasData(PATH_TO_READCD))		CM->SetString(PATH_TO_READCD, "/boot/home/config/bin/readcd");
	if (!CM->HasData(PATH_TO_CDDA2WAV))	CM->SetString(PATH_TO_CDDA2WAV, "/boot/home/config/bin/cdda2wav");
	if (!CM->HasData(TEMP_PATH))			CM->SetString(TEMP_PATH, "/boot/Helios/");

	// HeliosConfigView
	/*
	if (!CM->HasData(BURNPROOF))		CM->SetBool(BURNPROOF, false);
	if (!CM->HasData(SIMULATION))		CM->SetBool(SIMULATION, false);
	if (!CM->HasData(FIXATE_DISC))		CM->SetBool(FIXATE_DISC, true);
	if (!CM->HasData(USE_DAO))		CM->SetBool(USE_DAO, true);
	if (!CM->HasData(OVERBURNING))		CM->SetBool(OVERBURNING, false);
	if (!CM->HasData(FORCEDERASING))	CM->SetBool(FORCEDERASING, false);
	if (!CM->HasData(WRITINGSPEED))	CM->SetInt32(WRITINGSPEED, 10);
	if (!CM->HasData(FIFOBUFFERSIZE))	CM->SetInt8(FIFOBUFFERSIZE, 4);
*/
	// DeviceConfigView
	if (!CM->HasData(EJECT_WRITER))	CM->SetBool(EJECT_WRITER, true);
	if (!CM->HasData(EJECT_READER))	CM->SetBool(EJECT_READER, true);
	if (!CM->HasData(WRITERDEVICE))	CM->SetInt8(WRITERDEVICE, 0);
	if (!CM->HasData(READERDEVICE))	CM->SetInt8(READERDEVICE, 0);
	if (!CM->HasData(TWODRIVECOPY))	CM->SetBool(TWODRIVECOPY, false);
	if (!CM->HasData(SCANNINGMETHOD))	CM->SetInt8(SCANNINGMETHOD, DCV_DEFAULT_METHOD);
	if (!CM->HasData(SHOWALLDEVICES))	CM->SetBool(SHOWALLDEVICES, false);
	
	// PathConfigView
	if (!CM->HasData(TEMPORARY_PATH))	CM->SetString(TEMPORARY_PATH, "/boot/home/");
	if (!CM->HasData(PROJECT_PATH))	CM->SetString(PROJECT_PATH, "/boot/Helios/");
	if (!CM->HasData(DEFIMAGENAME))	CM->SetString(DEFIMAGENAME, "Helios.imagefile");
	if (!CM->HasData(DEFMOUNTPOINT))	CM->SetString(DEFMOUNTPOINT, "/HeliosMountPoint");

	// ImageConfigView
	if (!CM->HasData(I_PUBLISHER))		CM->SetString(I_PUBLISHER, "Zeta");
	if (!CM->HasData(I_PREPARER))		CM->SetString(I_PREPARER, "Zeta");
	if (!CM->HasData(I_APPLICATION))	CM->SetString(I_APPLICATION, "Helios");

	// FilesystemConfigView
	if (!CM->HasData(F_ISO9660_CB))	CM->SetBool(F_ISO9660_CB, true);
	if (!CM->HasData(F_JOLIET_CB))		CM->SetBool(F_JOLIET_CB, true);
	if (!CM->HasData(F_RR_CB))		CM->SetBool(F_RR_CB, true);
	if (!CM->HasData(F_HFS_CB))		CM->SetBool(F_HFS_CB, false);
	if (!CM->HasData(F_UDF_CB))		CM->SetBool(F_UDF_CB, false);
	if (!CM->HasData(F_83FN_RB))		CM->SetBool(F_83FN_RB, false);
	if (!CM->HasData(F_31FN_RB))		CM->SetBool(F_31FN_RB, true);
	if (!CM->HasData(F_37FN_RB))		CM->SetBool(F_37FN_RB, false);
	if (!CM->HasData(F_JOLDEF_RB))		CM->SetBool(F_JOLDEF_RB, true);
	if (!CM->HasData(F_JOLUNI_RB))		CM->SetBool(F_JOLUNI_RB, false);
	if (!CM->HasData(F_RRDEF_RB))		CM->SetBool(F_RRDEF_RB, true);
	if (!CM->HasData(F_RRRAT_RB))		CM->SetBool(F_RRRAT_RB, false);
	if (!CM->HasData(F_UDFDEF_RB))		CM->SetBool(F_UDFDEF_RB, true);
	if (!CM->HasData(F_UDFDVD_RB))		CM->SetBool(F_UDFDVD_RB, false);
	if (!CM->HasData(F_MAPPING_CB))	CM->SetBool(F_MAPPING_CB, true);
	if (!CM->HasData(F_MAPPING_TC))	CM->SetString(F_MAPPING_TC, "");

	// StandardConfigView
	if (!CM->HasData(S_FILESYSTEM))		CM->SetInt32(S_FILESYSTEM, 5);	// Joliet by default
	if (!CM->HasData(S_VOLUMENAME))	CM->SetString(S_VOLUMENAME, _T("untitled")); // "L:untitled"
	if (!CM->HasData(S_CDTYPE))		CM->SetInt8(S_CDTYPE, 0);
	if (!CM->HasData(S_PUBLISHER))		CM->SetString(S_PUBLISHER, "Zeta");
	if (!CM->HasData(S_PREPARER))		CM->SetString(S_PREPARER, "Zeta");
	if (!CM->HasData(S_APPLICATION))	CM->SetString(S_APPLICATION, "Helios");
	//if (!CM->HasData(S_TEMPPATH))		CM->SetString(S_TEMPPATH, "/boot/home/");
	//if (!CM->HasData(S_PROJECTPATH))	CM->SetString(S_PROJECTPATH, "/boot/Helios/");
	
	// ApplicationConfigView
	if (!CM->HasData(APP_AUTOUPDATE))		CM->SetBool(APP_AUTOUPDATE, false);
	if (!CM->HasData(APP_UPDATEFREQUENCY))	CM->SetBool(APP_UPDATEFREQUENCY, 60);
	if (!CM->HasData(APP_TIMEOUTLENGTH))	CM->SetBool(APP_TIMEOUTLENGTH, 90);
	if (!CM->HasData(APP_TIMEOUT))			CM->SetBool(APP_TIMEOUT, true);
	
	// SoundConfigView
	if (!CM->HasData(BEEPWHENWRITTEN))		CM->SetBool(BEEPWHENWRITTEN, false);
	if (!CM->HasData(BEEPWHENIMAGE))		CM->SetBool(BEEPWHENIMAGE, false);
	if (!CM->HasData(BEEPWHENERROR))		CM->SetBool(BEEPWHENERROR, false);
	
	// SaveConfigView
	if (!CM->HasData(STREAMING_MODE))	CM->SetBool(STREAMING_MODE, true);
	if (!CM->HasData(SAVEWHENQUIT))	CM->SetBool(SAVEWHENQUIT, true);
	if (!CM->HasData(EXITMODE))		CM->SetInt8(EXITMODE, EM_ASK_USER);
	if (!CM->HasData(INFOBAR_ENABLED))	CM->SetBool(INFOBAR_ENABLED, false);
	if (!CM->HasData(AUTOUPDATE))		CM->SetBool(AUTOUPDATE, false);
//	if (!CM->HasData(LANGUAGE_ID))		CM->SetInt32(LANGUAGE_ID, 0);
//	if (!CM->HasData(BUBBLEHELP))		CM->SetBool(BUBBLEHELP, true);
	if (!CM->HasData(BURNPRIORITY)) {	CM->SetInt32(BURNPRIORITY, 21); }

	// ColorConfigView
	if (!CM->HasData(BURNING_STATUS_BAR_COLOR))	CM->SetColor(BURNING_STATUS_BAR_COLOR, (rgb_color){60,225,60,255});
	if (!CM->HasData(CDSIZE_STATUS_BAR_COLOR))	CM->SetColor(CDSIZE_STATUS_BAR_COLOR, (rgb_color){50, 150, 255,255});
	if (!CM->HasData(LOGWIN_BG_COLOR))			CM->SetColor(LOGWIN_BG_COLOR, (rgb_color){255,255,255,255});
	if (!CM->HasData(LOGWIN_FG_COLOR))			CM->SetColor(LOGWIN_FG_COLOR, (rgb_color){0,0,0,255});
	if (!CM->HasData(LISTSELECTION_COLOR))		CM->SetColor(LISTSELECTION_COLOR, (rgb_color){90,90,90,255});
	if (!CM->HasData(LISTBACKGROUND_COLOR))		CM->SetColor(LISTBACKGROUND_COLOR, (rgb_color){255,255,255,255});


	// CD-MODE-SPECIFIC CONFIGURATION
	// DATA-CD
	if (!CM->HasData(DATACD_FILESYSTEM))	CM->SetInt32(DATACD_FILESYSTEM, CM->GetInt32(S_FILESYSTEM));
	if (!CM->HasData(DATACD_VOLUMENAME))	CM->SetString(DATACD_VOLUMENAME, CM->GetString(S_VOLUMENAME)->String());
	if (!CM->HasData(DATACD_MULTISESSION))	CM->SetBool(DATACD_MULTISESSION, false);
	// AUDIO-CD
	if (!CM->HasData(AUDIOCD_WRITECDTEXT))	CM->SetBool(AUDIOCD_WRITECDTEXT, false);
	if (!CM->HasData(AUDIOCD_SCMS))		CM->SetBool(AUDIOCD_SCMS, false);
	if (!CM->HasData(AUDIOCD_ALBUMNAME))	CM->SetString(AUDIOCD_ALBUMNAME, "");
	// BOOTABLE-CD
	if (!CM->HasData(BOOTCD_FILESYSTEM))	CM->SetInt32(BOOTCD_FILESYSTEM, CM->GetInt32(S_FILESYSTEM));
	if (!CM->HasData(BOOTCD_VOLUMENAME))	CM->SetString(BOOTCD_VOLUMENAME, CM->GetString(S_VOLUMENAME)->String());
	if (!CM->HasData(BOOTCD_FLOPPYIMAGE))	CM->SetString(BOOTCD_FLOPPYIMAGE, "");
	// CD EXTRA
	if (!CM->HasData(CDEXTRA_WRITECDTEXT))	CM->SetBool(CDEXTRA_WRITECDTEXT, false);
	if (!CM->HasData(CDEXTRA_ALBUMNAME))	CM->SetString(CDEXTRA_ALBUMNAME, "");
	if (!CM->HasData(CDEXTRA_DATAIMAGE))	CM->SetString(CDEXTRA_DATAIMAGE, "");
	if (!CM->HasData(CDEXTRA_FILESYSTEM))	CM->SetInt32(CDEXTRA_FILESYSTEM, CM->GetInt32(S_FILESYSTEM));
	if (!CM->HasData(CDEXTRA_VOLUMENAME))	CM->SetString(CDEXTRA_VOLUMENAME, CM->GetString(S_VOLUMENAME)->String());
	// DATA-DVD
	if (!CM->HasData(DVDVIDEO_FILESYSTEM))	CM->SetInt32(DVDVIDEO_FILESYSTEM, CM->GetInt32(S_FILESYSTEM));
	if (!CM->HasData(DVDVIDEO_VOLUMENAME))	CM->SetString(DVDVIDEO_VOLUMENAME, CM->GetString(S_VOLUMENAME)->String());


	if (!CM->HasData(PATHTOTHEIMAGE))		CM->SetString(PATHTOTHEIMAGE, "/boot/home/image.img");
	
	// ProtocolWindow Settings
	if (!CM->HasData(CDRECORD_LOGWIN_VISIBLE))	CM->SetBool(CDRECORD_LOGWIN_VISIBLE, false);
	if (!CM->HasData(MKISOFS_LOGWIN_VISIBLE))		CM->SetBool(MKISOFS_LOGWIN_VISIBLE, false);
	if (!CM->HasData(CDRECORD_LOG))			CM->SetBool(CDRECORD_LOG, false);
	if (!CM->HasData(MKISOFS_LOG))				CM->SetBool(MKISOFS_LOG, false);
	if (!CM->HasData(CDRECORD_LOGFILENAME))		CM->SetString(CDRECORD_LOGFILENAME, "");
	if (!CM->HasData(MKISOFS_LOGFILENAME))		CM->SetString(MKISOFS_LOGFILENAME, "");

	// CLV-column widths
	if (!CM->HasData(FILENAMECOLUMN_WIDTH))		CM->SetInt16(FILENAMECOLUMN_WIDTH, 200);
	if (!CM->HasData(SIZECOLUMN_WIDTH))			CM->SetInt16(SIZECOLUMN_WIDTH, 57);
	if (!CM->HasData(TRACKNUMBERCOLUMN_WIDTH))	CM->SetInt16(TRACKNUMBERCOLUMN_WIDTH, 47);
	if (!CM->HasData(TRACKNAMECOLUMN_WIDTH))	CM->SetInt16(TRACKNAMECOLUMN_WIDTH, 200);
	if (!CM->HasData(LENGTHCOLUMN_WIDTH))		CM->SetInt16(LENGTHCOLUMN_WIDTH, 80);
	if (!CM->HasData(BYTESCOLUMN_WIDTH))		CM->SetInt16(BYTESCOLUMN_WIDTH, 50);
	if (!CM->HasData(INDEXCOLUMN_WIDTH))		CM->SetInt16(INDEXCOLUMN_WIDTH, 150);
	if (!CM->HasData(CDTEXTCOLUMN_WIDTH))		CM->SetInt16(CDTEXTCOLUMN_WIDTH, 150);


	CM->WriteConfiguration();
	CM->ReadConfiguration();
	
	// apply values to GUI elements...
	/*heliosCV->SetBurnproof(CM->GetBool(BURNPROOF));
	heliosCV->SetSimulation(CM->GetBool(SIMULATION));
	heliosCV->SetFixating(CM->GetBool(FIXATE_DISC));
	heliosCV->SetDAO(CM->GetBool(USE_DAO));
	heliosCV->SetOverburning(CM->GetBool(OVERBURNING));
	heliosCV->SetWriterSpeed(CM->GetInt32(WRITINGSPEED));
	heliosCV->SetFIFOSize(CM->GetInt8(FIFOBUFFERSIZE));
	heliosCV->SetForcedErasing(CM->GetBool(FORCEDERASING));*/
	heliosCV->SetSettings(msg);

	deviceCV->SetWriterEjected(CM->GetBool(EJECT_WRITER));
	deviceCV->SetReaderEjected(CM->GetBool(EJECT_READER));
	deviceCV->SetWriterNumber(CM->GetInt8(WRITERDEVICE));
	deviceCV->SetTwoDriveCopy(CM->GetBool(TWODRIVECOPY));
	deviceCV->SetReaderNumber(CM->GetInt8(READERDEVICE));
	deviceCV->SetScanMethod(CM->GetInt8(SCANNINGMETHOD));
	deviceCV->SetShowAllDevices(CM->GetBool(SHOWALLDEVICES));
	
	pathCV->SetTemporaryPath(CM->GetString(TEMPORARY_PATH)->String());
	pathCV->SetProjectPath(CM->GetString(PROJECT_PATH)->String());
	pathCV->SetDefaultImageName(CM->GetString(DEFIMAGENAME)->String());
	pathCV->SetDefaultMountPoint(CM->GetString(DEFMOUNTPOINT)->String());
	
	imageCV->SetPublisher(CM->GetString(I_PUBLISHER)->String());
	imageCV->SetPreparer(CM->GetString(I_PREPARER)->String());
	imageCV->SetApplication(CM->GetString(I_APPLICATION)->String());

	fspanelCV->SetISO9660(CM->GetBool(F_ISO9660_CB));
	fspanelCV->SetJoliet(CM->GetBool(F_JOLIET_CB));
	fspanelCV->SetRockRidge(CM->GetBool(F_RR_CB));
	fspanelCV->SetUDF(CM->GetBool(F_UDF_CB));
	fspanelCV->SetHFS(CM->GetBool(F_HFS_CB));
	fspanelCV->SetISO96608Dot3(CM->GetBool(F_83FN_RB));
	fspanelCV->SetISO966031(CM->GetBool(F_31FN_RB));
	fspanelCV->SetISO966037(CM->GetBool(F_37FN_RB));
	fspanelCV->SetJolietDefault(CM->GetBool(F_JOLDEF_RB));
	fspanelCV->SetJolietUnicode(CM->GetBool(F_JOLUNI_RB));
	fspanelCV->SetRockRidgeDefault(CM->GetBool(F_RRDEF_RB));
	fspanelCV->SetRockRidgeRationalized(CM->GetBool(F_RRRAT_RB));
	fspanelCV->SetUDFDefault(CM->GetBool(F_UDFDEF_RB));
	fspanelCV->SetUDFDVDVideo(CM->GetBool(F_UDFDVD_RB));
	fspanelCV->SetMapping(CM->GetBool(F_MAPPING_CB));
	fspanelCV->SetMappingFile(CM->GetString(F_MAPPING_TC)->String());

	standardCV->SetFilesystem(CM->GetInt32(S_FILESYSTEM));	
	standardCV->SetVolumeName(CM->GetString(S_VOLUMENAME)->String());
	standardCV->SetCDType(CM->GetInt8(S_CDTYPE));
	standardCV->SetPublisher(CM->GetString(S_PUBLISHER)->String());
	standardCV->SetPreparer(CM->GetString(S_PREPARER)->String());
	standardCV->SetApplication(CM->GetString(S_APPLICATION)->String());
	//standardCV->SetTemporaryPath(CM->GetString(S_TEMPPATH)->String());
	//standardCV->SetProjectPath(CM->GetString(S_PROJECTPATH)->String());
	
	applicationCV->SetAutoUpdating(CM->GetBool(APP_AUTOUPDATE));
	applicationCV->SetTimeout(CM->GetBool(APP_TIMEOUT));
	applicationCV->SetUpdateFrequency(CM->GetInt16(APP_UPDATEFREQUENCY));
	applicationCV->SetTimeoutLength(CM->GetInt16(APP_TIMEOUTLENGTH));

	soundCV->SetFinishedWritingSound(CM->GetBool(BEEPWHENWRITTEN));
	soundCV->SetFinishedImageSound(CM->GetBool(BEEPWHENIMAGE));
	soundCV->SetErrorSound(CM->GetBool(BEEPWHENERROR));

	saveCV->SetStreamed(CM->GetBool(STREAMING_MODE));
	saveCV->SetSaveWhenQuit(CM->GetBool(SAVEWHENQUIT));
	saveCV->SetExitMode((exit_modes)CM->GetInt8(EXITMODE));
//	saveCV->SetLanguage(CM->GetInt32(LANGUAGE_ID));
	saveCV->SetAutoUpdate(CM->GetBool(AUTOUPDATE));
//	saveCV->SetBalloonHelp(CM->GetBool(BUBBLEHELP));
	saveCV->SetBurnPriority(CM->GetInt32(BURNPRIORITY));
	
	if (configW->Lock()) {
		colorCV->SetColor(0, CM->GetColor(BURNING_STATUS_BAR_COLOR));
			if (fStatusWindow->Lock()) {
				fStatusWindow->SetBarColor(colorCV->GetColor(0));
				fStatusWindow->Unlock();
			}
		colorCV->SetColor(1, CM->GetColor(CDSIZE_STATUS_BAR_COLOR));
			if (window1->Lock()) {
				window1->view1->box2->SetBarColor(colorCV->GetColor(1));
				window1->Unlock();
			}
		colorCV->SetColor(2, CM->GetColor(LOGWIN_BG_COLOR));
		colorCV->SetColor(3, CM->GetColor(LOGWIN_FG_COLOR));
			if (cdrecord_output->Lock()) {
				cdrecord_output->SetColors(colorCV->GetColor(3), colorCV->GetColor(2));
				cdrecord_output->Unlock();
			}
			if (mkisofs_output->Lock()) {
				mkisofs_output->SetColors(colorCV->GetColor(3), colorCV->GetColor(2));
				mkisofs_output->Unlock();
			}
		colorCV->SetColor(4, CM->GetColor(LISTSELECTION_COLOR));
			if (window1->Lock()) {
				window1->view1->listview1->SetSelectionColor(colorCV->GetColor(4));
				window1->Unlock();
			}
		colorCV->SetColor(5, CM->GetColor(LISTBACKGROUND_COLOR));
			if (window1->Lock()) {
				window1->view1->listview1->SetBackgroundColor(colorCV->GetColor(5));
				window1->Unlock();
			}
		configW->Unlock();
	}
	
	// CD-MODE-SPECIFIC...
	// DATA-CD
	window1->view1->SetVolumeName(CM->GetString(DATACD_VOLUMENAME)->String());
	window1->view1->SetMultisession(CM->GetBool(DATACD_MULTISESSION));
	window1->view1->SetFilesystem(CM->GetInt32(DATACD_FILESYSTEM));
	// AUDIO-CD
	window1->view1->SetCDText(CM->GetBool(AUDIOCD_WRITECDTEXT));
	window1->view1->SetProtection(CM->GetBool(AUDIOCD_SCMS));
	window1->view1->SetCDAlbum(CM->GetString(AUDIOCD_ALBUMNAME)->String());
	// BOOTABLE-CD
	window1->view1->SetVolumeNameBootCD(CM->GetString(BOOTCD_VOLUMENAME)->String());
	window1->view1->SetImageNameBootCD(CM->GetString(BOOTCD_FLOPPYIMAGE)->String());
	window1->view1->SetFilesystemBootCD(CM->GetInt32(BOOTCD_FILESYSTEM));
	// CD EXTRA
	window1->view1->SetImageNameCDExtra(CM->GetString(CDEXTRA_DATAIMAGE)->String());
	window1->view1->SetCDTextCDExtra(CM->GetBool(CDEXTRA_WRITECDTEXT));
	window1->view1->SetCDAlbumCDExtra(CM->GetString(CDEXTRA_ALBUMNAME)->String());
	window1->view1->SetVolumeNameCDExtra(CM->GetString(CDEXTRA_VOLUMENAME)->String());
	window1->view1->SetFilesystemCDExtra(CM->GetInt32(CDEXTRA_FILESYSTEM));
	// DATA-DVD
	window1->view1->SetVolumeNameDVDVideo(CM->GetString(DVDVIDEO_VOLUMENAME)->String());
	window1->view1->SetFilesystemDVDVideo(CM->GetInt32(DVDVIDEO_FILESYSTEM));
	

	if (CM->HasData(MAINWINDOW_RECT)) {
		window1->ResizeTo(CM->GetRect(MAINWINDOW_RECT)->Width(), CM->GetRect(MAINWINDOW_RECT)->Height());
		window1->MoveTo(CM->GetRect(MAINWINDOW_RECT)->left, CM->GetRect(MAINWINDOW_RECT)->top);
	}
	if (CM->HasData(WINDOW2_RECT)) {
		window2->ResizeTo(CM->GetRect(WINDOW2_RECT)->Width(), CM->GetRect(WINDOW2_RECT)->Height());
		window2->MoveTo(CM->GetRect(WINDOW2_RECT)->left, CM->GetRect(WINDOW2_RECT)->top);
	}
	if (CM->HasData(PREFERENCESWINDOW_RECT)) {
		/*
		 * we don't resize the window here. instead just move it to the Left-Top
		 * point of the saved frame. This avoids the case that when settings are saved, and
		 * you change the window size in the code to be wider, that the window gets resized
		 * to the old saved frame. May happen for users which update to a newer Helios
		 * with wider prefs.
		 */
		//configW->ResizeTo(CM->GetRect(PREFERENCESWINDOW_RECT)->Width(), CM->GetRect(PREFERENCESWINDOW_RECT)->Height());
		configW->MoveTo(CM->GetRect(PREFERENCESWINDOW_RECT)->left, CM->GetRect(PREFERENCESWINDOW_RECT)->top);
	}
	if (CM->HasData(CDRECORDLOGWINDOW_RECT)) {
		cdrecord_output->ResizeTo(CM->GetRect(CDRECORDLOGWINDOW_RECT)->Width(), CM->GetRect(CDRECORDLOGWINDOW_RECT)->Height());
		cdrecord_output->MoveTo(CM->GetRect(CDRECORDLOGWINDOW_RECT)->left, CM->GetRect(CDRECORDLOGWINDOW_RECT)->top);
	}
	if (CM->HasData(MKISOFSLOGWINDOW_RECT)) {
		mkisofs_output->ResizeTo(CM->GetRect(MKISOFSLOGWINDOW_RECT)->Width(), CM->GetRect(MKISOFSLOGWINDOW_RECT)->Height());
		mkisofs_output->MoveTo(CM->GetRect(MKISOFSLOGWINDOW_RECT)->left, CM->GetRect(MKISOFSLOGWINDOW_RECT)->top);
	}

	// ProtocolWindow Settings
	if (CM->GetBool(CDRECORD_LOGWIN_VISIBLE)) {
		cdrecord_output->Show();
		if (window1->Lock()) {
			window1->view1->cdrecord_outputMI->SetMarked(true);
			window1->Unlock();
		}
	}
	if (CM->GetBool(MKISOFS_LOGWIN_VISIBLE)) {
		mkisofs_output->Show();
		if (window1->Lock()) {
			window1->view1->mkisofs_outputMI->SetMarked(true);
			window1->Unlock();
		}
	}
	
	// Language settings.
#if defined(_BEOS_R5_BUILD_) || defined(_BEOS_HAIKU_BUILD_)
	if (CM->HasData(CURRENT_LANG_TAG)) {
		BString langTag = *(CM->GetString(CURRENT_LANG_TAG));
		gMSHLangMgr->SelectLanguageByName(langTag);
	}
#endif
	
	if (cdrecord_output->Lock()) {
		cdrecord_output->SetLogToFile(CM->GetBool(CDRECORD_LOG));
		cdrecord_output->SetLogFileName(CM->GetString(CDRECORD_LOGFILENAME)->String());
		cdrecord_output->Unlock();
	}
	if (mkisofs_output->Lock()) {
		mkisofs_output->SetLogToFile(CM->GetBool(MKISOFS_LOG));
		mkisofs_output->SetLogFileName(CM->GetString(MKISOFS_LOGFILENAME)->String());
		mkisofs_output->Unlock();
	}

	return;
}


void
Application1::Calculate_time() {
	off_t size2=window1->view1->box2->GetCDSize();
	if (heliosCV->IsFixating()) size2+=13824000;
	uint64 r=(2352*(uint64)size2/(2048*176400*(uint64)heliosCV->GetWriterSpeed()));
	BString str="";
	r+=10; // 10 Sekunden Wartezeit von cdrecord
	r+=10; // und 10 sekunden wartezeit für spinup/spindown
	r+=10; // und dann nochmal 10 sekunden, für alle fälle...
	uint64 u=r-60*(uint32)(r/60);
	char *s="";
	
	if (u<10) s="0"; else s="";
	str << (off_t)(r/60) << "m " << s << (off_t)u << "s";
	window1->Lock();
	window1->view1->box2->stringview6->SetText(str.String());
	window1->Unlock();
}


void
Application1::UpdateTrackNumbers()
{
//	RemoveFileTrackNumbers();
	window1->Lock();
	if ((window1->view1->CDType()==AUDIOCD_INDEX) || (window1->view1->CDType()==CDEXTRA_INDEX)) {
		for (int i=0; i<window1->view1->listview1->iCountItems();i++) {
			((AudioRow *)(window1->view1->listview1->RowAt(i)))->SetTrackNumber(i+1);
//			window1->view1->listview1->InvalidateItem(i);
		}
		if (window1->view1->listview1->iCountItems()>0)
			((AudioRow *)(window1->view1->listview1->RowAt(0)))->SetPregap(150);
	}
	window1->Unlock();
//	AddFileTrackNumbers();
}


int
Application1::DeviceStatus(const char *path)
{
	if ( path == NULL || strlen(path) == 0 ) {
		return E_ERROR;
	}

	// okay, path point to a device
	int 		fd;
	status_t	status;
	status_t	media_status;

	fd = open(path, 0);
	if (fd < 0 ) {
		return E_ERROR;
	}

	status = ioctl(fd, B_GET_MEDIA_STATUS, &media_status, sizeof(media_status));
	close(fd);

	if ( status != B_OK ) {
		return E_ERROR;
	}

	switch(media_status) {
		case B_NO_ERROR:
			return E_NO_ERROR;
		case B_DEV_NO_MEDIA:
			return E_NO_DISC;
		case B_DEV_NOT_READY:
			return E_NOT_READY;
		case B_DEV_MEDIA_CHANGED:
			return E_MEDIA_CHANGED;
		case B_DEV_DOOR_OPEN:
			return E_DOOR_OPEN;
		default:
			return E_ERROR;
		};
}


void
Application1::AddToRecent(const char *filename)
{
	int	i;
	// check if there is already a project with the same name
	for (i=0; i<CM->CountStrings(RECENTPROJECTS_LIST); i++)
		if (CM->GetStringI(RECENTPROJECTS_LIST, i)->Compare(filename)==0) {
			// ok, it's already there...
			return;
		}
	//TODO: should be moved to the top of the list...

	// shift projects in array
	if (CM->CountStrings(RECENTPROJECTS_LIST)>=9)
		for (i=0; i<CM->CountStrings(RECENTPROJECTS_LIST); i++) {
			CM->SetStringI(RECENTPROJECTS_LIST, CM->GetStringI(RECENTPROJECTS_LIST, i+1), i);
		}
	CM->SetStringI(RECENTPROJECTS_LIST,new BString(filename), CM->CountStrings(RECENTPROJECTS_LIST));
}


void
Application1::ShowRecentProjects()
{
	char	*filename;
	filename=(char *)malloc(B_FILE_NAME_LENGTH);

	window1->Lock();
	while (window1->view1->openrecentSM->CountItems()>0) delete window1->view1->openrecentSM->RemoveItem(0L);
	for (int i=0; i<CM->CountStrings(RECENTPROJECTS_LIST); i++) {
		if (CM->GetStringI(RECENTPROJECTS_LIST, i)->Length()>0) {
			// add if project exists, only
			BEntry entry;
			if (entry.SetTo(CM->GetStringI(RECENTPROJECTS_LIST, i)->String(), true)==B_OK) {
				entry.GetName(filename);
				BMessage *msg = new BMessage(OPEN_FPMSG);		

				entry_ref	*ref = new entry_ref();
				entry.GetRef(ref);
				
				msg->AddRef("refs", ref);
				BMenuItem	*mi = new BMenuItem(filename, msg);
				mi->SetTarget(be_app_messenger);
				if (!entry.Exists()) {
					mi->SetEnabled(false);
				}
				window1->view1->openrecentSM->AddItem(mi, 0);
				entry.Unset();
			}
		}
	}
	window1->Unlock();
	free(filename);
}


const char *
Application1::SuggestProjectName()
{
	switch(window1->view1->CDType()) {
		case DATACD_INDEX: {
			return window1->view1->GetVolumeName();
		}
		
		case AUDIOCD_INDEX: {
			return window1->view1->GetCDAlbum();
		}
		
		case BOOTABLECD_INDEX: {
			return window1->view1->GetVolumeNameBootCD();
		}
		
		case CDEXTRA_INDEX: {
			return window1->view1->GetVolumeNameCDExtra();
		}
		
		case DVDVIDEO_INDEX: {
			return window1->view1->GetVolumeNameDVDVideo();
		}
		
		default: {
			return "";
		}
	}
	return "";
}


void
Application1::LoadProject(BEntry *entry) {
	if (!entry->Exists()) {
		return;
	}

	BFile *file = new BFile(entry, B_READ_ONLY);

	// check for correct file type...
	BNodeInfo *info = new BNodeInfo(file);
	char	type[B_FILE_NAME_LENGTH];
	info->GetType(type);
	delete info;
	info = NULL;

		if (strcmp(type, HELIOSPROJECTMIMETYPE)!=0) {
			ErrorBox	*eb=new ErrorBox(E_ORANGE_COLOR,	_T("Error"), // "TERROR"
															_T("The file you selected is not a Helios project file. Only Helios project files can be loaded with this application."), // "Error:No Project File"
															_T("Ok")); // "TOK"
			eb->SetShortcut(0, B_ENTER);
			eb->Go();
			file->Unset();
			delete file;
			return;
		}
	


	BPath	path;
	int16	cdtype = 1;
	BString	str;
	char	projectname[B_FILE_NAME_LENGTH];
	
	window1->Lock();
	entry->GetPath(&path);

	ConfigManager	*projM=new ConfigManager(path.Path());
	// determine which sort of project we will load
	if (projM->HasData("cdtype")) cdtype=projM->GetInt16("cdtype");
	
	// set new window title (containing the project's name)
	entry->GetName(projectname);
	BString *s=new BString(_T("Helios")); // "TWINDOWTITLE"
	s->Append(" | ");
	s->Append(projectname);
	window1->SetTitle(s->String());
	delete s;

	// load general data
	if (projM->HasData("current_folder"))
		current_folder=projM->GetString("current_folder")->String();
	if (projM->HasData("base_folder"))
		base_folder=projM->GetString("base_folder")->String();
	if (projM->HasData("publisher"))
		imageCV->SetPublisher(projM->GetString("publisher")->String());
	if (projM->HasData("preparer"))
		imageCV->SetPreparer(projM->GetString("preparer")->String());
	if (projM->HasData("application"))
		imageCV->SetApplication(projM->GetString("application")->String());

	
	
	window1->view1->SetCDType(cdtype);
	Update_DATA_AUDIO();
	// load cd-mode-specific data
	switch(cdtype) {
		case DATACD_INDEX: {
			if (projM->HasData("filesystem"))
				window1->view1->SetFilesystem(projM->GetInt16("filesystem"));
			if (projM->HasData("volumename"))
				window1->view1->SetVolumeName(projM->GetString("volumename")->String());
			if (projM->HasData("multisession"))
				window1->view1->SetMultisession(projM->GetBool("multisession"));
			break;
		}
		case AUDIOCD_INDEX: {
			if (projM->HasData("writecdtext"))
				window1->view1->SetCDText(projM->GetBool("writecdtext"));
			if (projM->HasData("copyprotection"))
				window1->view1->SetProtection(projM->GetBool("copyprotection"));
			if (projM->HasData("cdtextalbum"))
				window1->view1->SetCDAlbum(projM->GetString("cdtextalbum")->String());
				
			for (int32 t=0; t<projM->CountStrings("filename"); t++) {
				AudioRow *item=new AudioRow(t+1, projM->GetStringI("filename", t)->String(), *(projM->GetInt64("pause", t)), (off_t)(*(projM->GetInt64("size", t))), (off_t)(*(projM->GetInt64("length", t))));
				if (projM->GetInt64("startF", t)) {
					item->SetStartFrame(*(projM->GetInt64("startF", t)));
				} else {
					item->SetStartFrame((int64)0);
				}
				if (projM->GetInt64("endF", t)) {
					item->SetEndFrame(*(projM->GetInt64("endF", t)));
				} else {
					item->SetEndFrame(75*60*60*4);
				}

				BList	*list=new BList();
				float	*f;
				char 	dataname[50];
				sprintf(dataname, "indices track %ld", t+1);
				for(int32 i=0; (f=projM->GetFloat(dataname, i))!=NULL; i++) {
					list->AddItem((void *)f);
				}
				item->SetIndexList(list);
				delete list;
				item->SetCDTitle(projM->GetStringI("cdtext title", t)->String());
				str=item->GetFilename();
				str.Prepend(base_folder);
				if (FileExists(str.String()))
					window1->view1->listview1->AddRow(item);
				else
					delete item;
			}
			break;
		}
		case BOOTABLECD_INDEX: {
			if (projM->HasData("filesystem"))
				window1->view1->SetFilesystemBootCD(projM->GetInt16("filesystem"));
			if (projM->HasData("volumename"))
				window1->view1->SetVolumeNameBootCD(projM->GetString("volumename")->String());
			if (projM->HasData("imagename"))
				window1->view1->SetImageNameBootCD(projM->GetString("imagename")->String());
			break;
		}
		case CDEXTRA_INDEX: {
			if (projM->HasData("imagename"))
				window1->view1->SetImageNameCDExtra(projM->GetString("imagename")->String());
			if (projM->HasData("writecdtext"))
				window1->view1->SetCDTextCDExtra(projM->GetBool("writecdtext"));
			if (projM->HasData("cdtextalbum"))
				window1->view1->SetCDAlbumCDExtra(projM->GetString("cdtextalbum")->String());
	
			for (int32 t=0; t<projM->CountStrings("filename"); t++) {
				AudioRow *item=new AudioRow(t+1, projM->GetStringI("filename", t)->String(), *(projM->GetInt64("pause", t)), (off_t)(*(projM->GetInt64("size", t))), (off_t)(*(projM->GetInt64("length", t))));
				if (projM->GetInt64("startF", t)) {
					item->SetStartFrame(*(projM->GetInt64("startF", t)));
				} else {
					item->SetStartFrame((int64)0);
				}
				if (projM->GetInt64("endF", t)) {
					item->SetEndFrame(*(projM->GetInt64("endF", t)));
				} else {
					item->SetEndFrame(75*60*60*4);
				}

				BList	*list=new BList();
				float	*f;
				char 	dataname[50];
				sprintf(dataname, "indices track %ld", t+1);
				for(int32 i=0; (f=projM->GetFloat(dataname, i))!=NULL; i++) {
					list->AddItem((void *)f);
				}
				item->SetIndexList(list);
				delete list;
				item->SetCDTitle(projM->GetStringI("cdtext title", t)->String());
				str=item->GetFilename();
				str.Prepend(base_folder);
				if (FileExists(str.String()))
					window1->view1->listview1->AddRow(item);
				else
					delete item;
			}
			break;
		}

#ifdef INCLUDE_DVD

		case DVDVIDEO_INDEX: {
			if (projM->HasData("filesystem"))
				window1->view1->SetFilesystemDVDVideo(projM->GetInt16("filesystem"));
			if (projM->HasData("volumename"))
				window1->view1->SetVolumeNameDVDVideo(projM->GetString("volumename")->String());
			break;
		}

#endif

		default: {
			break;
		}
	}
	
	UpdateTrackNumbers();
	window1->Unlock();

	CM->SetString(PATH_TO_LAST_PROJECT, path.Path());
	AddToRecent(path.Path());
	ShowRecentProjects();
	currentproject=path.Path();
	project_has_changed=false;
	
	printf("currentproject=%s\n", currentproject.String());
	
	if (saveCV->AutoUpdate())
		window1->view1->box2->Refresh();
	
	path.Unset();
	file->Unset();
	delete projM;
	delete file;
}


void
Application1::SaveProject(BEntry *entry, bool export_project) {
	BFile 	*file=new BFile(entry, B_READ_WRITE | B_ERASE_FILE | B_CREATE_FILE);
	BPath path;
	char	projectname[B_FILE_NAME_LENGTH];
	BString	subdir=current_folder;
	
	subdir.RemoveFirst(base_folder);
	
	window1->Lock();
	entry->GetName(projectname);

	// set MIME type
	BNode *node=new BNode(entry);
	BNodeInfo	*nodeinfo=new BNodeInfo(node);
	
	nodeinfo->SetType(HELIOSPROJECTMIMETYPE);

	node->Unset();
	delete nodeinfo;
	delete node;
	
	
	// rename the base folder for the newly saved project
	// and set the base_folder-variable to the new path
	BString bf=base_folder;
	bf.RemoveLast("/");
	BEntry *basef=new BEntry(bf.String(), false);
	BString temp=projectname;
	printf("projectname=%s\n", projectname);
	temp.Prepend("Folder_of_");
	if (!export_project) {
		basef->Rename(temp.String(), true);
		basef->GetPath(&path);
		base_folder=path.Path();
	} else {
		temp.Prepend("/");
		temp.Prepend(pathCV->GetProjectPath());
		//temp=*AppendTrailingSlash(&temp);
		printf("export-folder: [%s]\n", temp.String());
		create_directory(temp.String(), 0777);
		currentproject=pathCV->GetProjectPath();
		AppendTrailingSlash(&currentproject);
		currentproject.Append(projectname);
		base_folder=temp;
	}
	//base_folder=temp;
	//if (base_folder[base_folder.Length()-1]!='/') base_folder.Append("/");
	AppendTrailingSlash(&base_folder);
	basef->Unset();
	delete basef;
	current_folder.SetTo(base_folder.String());
	if (!export_project)
		current_folder.Append(subdir);
	//printf("current_folder=\"%s\"\nbase_folder=\"%s\"\n", current_folder.String(), base_folder.String());
	entry->GetPath(&path);
	CM->SetString(PATH_TO_LAST_PROJECT, path.Path());
	//sprintf(settings._lastprojectfile,"%s",path.Path());
	if (!export_project) {
		AddToRecent(path.Path());
		ShowRecentProjects();
		currentproject=path.Path();
	}
	file->Unset();
	delete file;

	entry->GetName(projectname);
	BString *s=new BString(_T("Helios")); // "TWINDOWTITLE"
	s->Append(" | ");
	s->Append(projectname);
	window1->SetTitle(s->String());
	delete s;
	window1->Unlock();
	project_has_changed=false;


	ConfigManager	*projM=new ConfigManager(path.Path());
	
	projM->SetInt16("cdtype", window1->view1->CDType());
	projM->SetString("current_folder", current_folder.String());
	projM->SetString("base_folder", base_folder.String());
	projM->SetString("publisher", imageCV->GetPublisher());
	projM->SetString("preparer", imageCV->GetPreparer());
	projM->SetString("application", imageCV->GetApplication());


	// save cd-mode-specific data
	switch(window1->view1->CDType()) {
		case DATACD_INDEX: {
			projM->SetInt16("filesystem", (int16)window1->view1->GetFilesystem());
			projM->SetString("volumename", window1->view1->GetVolumeName());
			projM->SetBool("multisession", window1->view1->IsMultisession());
			break;
		}
		case AUDIOCD_INDEX: {
			projM->SetBool("writecdtext", window1->view1->WriteCDText());
			projM->SetBool("copyprotection", window1->view1->ProtectAudioCD());
			projM->SetString("cdtextalbum", window1->view1->GetCDAlbum());
				
			for (int32 i=0; i<window1->view1->listview1->iCountItems(); i++) {
				AudioRow	*ar=(AudioRow *)window1->view1->listview1->RowAt(i);
				
				projM->SetStringI("filename", ar->GetFilename(), i);
				projM->SetStringI("cdtext title", ar->GetCDTitle(), i);
				projM->SetInt64("pause", (int64)ar->GetPregap(), i);
				projM->SetInt64("size", (int64)ar->GetBytes(), i);
				projM->SetInt64("length", (int64)ar->GetLength(), i);
				
				projM->SetInt64("startF", ar->GetStartFrame(), i);
				projM->SetInt64("endF", ar->GetEndFrame(), i);
				
				BList	*list=ar->GetIndexList();
				char 	dataname[50];
				sprintf(dataname, "indices track %ld", i+1);
				
				for (int32 j=0; j<list->CountItems(); j++) {
					projM->SetFloat(dataname, (*((float *)(list->ItemAt(j)))), j);
				}
			}
			break;
		}
		case BOOTABLECD_INDEX: {
			projM->SetInt16("filesystem", (int16)window1->view1->GetFilesystemBootCD());
			projM->SetString("volumename", window1->view1->GetVolumeNameBootCD());
			projM->SetString("imagename", window1->view1->GetImageNameBootCD());
			break;
		}
		case CDEXTRA_INDEX: {
			projM->SetString("imagename", window1->view1->GetImageNameCDExtra());
			projM->SetBool("writecdtext", window1->view1->WriteCDTextCDExtra());
			projM->SetString("cdtextalbum", window1->view1->GetCDAlbumCDExtra());
	
			for (int32 i=0; i<window1->view1->listview1->iCountItems(); i++) {
				AudioRow	*ar=(AudioRow *)window1->view1->listview1->RowAt(i);
				
				projM->SetStringI("filename", ar->GetFilename(), i);
				projM->SetStringI("cdtext title", ar->GetCDTitle(), i);
				projM->SetInt64("pause", (int64)ar->GetPregap(), i);
				projM->SetInt64("size", (int64)ar->GetBytes(), i);
				projM->SetInt64("length", (int64)ar->GetLength(), i);
				
				projM->SetInt64("startF", ar->GetStartFrame(), i);
				projM->SetInt64("endF", ar->GetEndFrame(), i);

				BList	*list=ar->GetIndexList();
				char 	dataname[50];
				sprintf(dataname, "indices track %ld", i+1);
				
				for (int32 j=0; j<list->CountItems(); j++) {
					projM->SetFloat(dataname, (*((float *)(list->ItemAt(j)))), j);
				}
			}
			break;
		}

#ifdef INCLUDE_DVD

		case DVDVIDEO_INDEX: {
			projM->SetInt16("filesystem", (int16)window1->view1->GetFilesystemDVDVideo());
			projM->SetString("volumename", window1->view1->GetVolumeNameDVDVideo());
			break;
		}

#endif

		default: {
			break;
		}
	}

	projM->WriteConfiguration();
	delete projM;
	
	//base_folder=oldbase_folder;
	//current_folder=oldcurrent_folder;
	// saved at quit?
	// so go on quitting Helios...
	release_sem(quitandsave_sem);
}


void
Application1::NewProject()
{
		imageCV->SetPublisher(standardCV->GetPublisher());
		imageCV->SetPreparer(standardCV->GetPreparer());
		imageCV->SetApplication(standardCV->GetApplication());
		
		//settingswindow->heliospathTC->SetText(base_folder.String());
		window1->Lock();
		window1->view1->SetVolumeName(standardCV->GetVolumeName());
		window1->view1->SetVolumeNameBootCD(standardCV->GetVolumeName());
		window1->view1->SetCDType(standardCV->GetCDType());
		window1->view1->SetFilesystemBootCD(standardCV->GetFilesystem());
		window1->view1->SetFilesystem(standardCV->GetFilesystem());

		BString	*p=new BString(pathCV->GetProjectPath());
		
		AppendTrailingSlash(p);

		base_folder=p->String();
		base_folder.Append(STANDARDPROJECTPATH);
		current_folder.SetTo(base_folder.String());

		//printf("base_folder=\"%s\"\n", base_folder.String());
		BDirectory HeliosDir;
		BString bf=base_folder;
		
		//bf=*RemoveTrailingSlash(bf);
		if (HeliosDir.SetTo(bf.String())!=B_OK)
			//HeliosDir.CreateDirectory(bf.String(),&HeliosDir);
			create_directory(bf.String(), 0777);
		BDirectory deletionDir(bf.String());
		DeleteAllIn(deletionDir);
		deletionDir.Unset();

		HeliosDir.Unset();


//		sprintf(currentproj->_currentpath,"%s",base_folder.String());
//		sprintf(currentproj->_basepath,"%s",base_folder.String());
//		currentproj->_filesystem=standardCV->GetFilesystem();
//		currentproj->_cdtype=standardCV->GetCDType();
//		sprintf(currentproj->_publisher,"%s",standardCV->GetPublisher());
//		sprintf(currentproj->_preparer,"%s",standardCV->GetPreparer());
//		sprintf(currentproj->_application,"%s",standardCV->GetApplication());
//		sprintf(currentproj->_volumename,"%s",standardCV->GetVolumeName());

		window1->SetTitle(_T("Helios")); // "TWINDOWTITLE"
		Update_DATA_AUDIO();
		window1->Unlock();
		project_has_changed = false;
}


void
Application1::SetAsStandardProject()
{
/*	standardCV->
	sprintf(settings._s_publisher,"%s",imageCV->GetPublisher());
	sprintf(settings._s_preparer,"%s",imageCV->GetPreparer());
	sprintf(settings._s_application,"%s",imageCV->GetApplication());
	sprintf(settings._s_volumename,"%s",window1->view1->GetVolumeName());
	settings._s_cdtype=window1->view1->CDType();
	settings._s_filesystem=window1->view1->GetFilesystem();
	sprintf(settings._s_currentpath,"%s%s",base_folder.String(), STANDARDPROJECTPATH);
	sprintf(settings._s_basepath,"%s%s",base_folder.String(), STANDARDPROJECTPATH);*/
}


status_t
Application1::GetMultisessionSectors(int32 &start, int32 &end)
{
		int arguments=0;
		thread_id Thread;
		const char **args;
		BString str="";
		BString device="dev=";
		BString endstr="";
		int in, out, err;
	
		//if (WaitForDisc(deviceCV->GetWriterDevicePath())==B_TIMEOUT) return B_TIMEOUT;
		
		device << deviceCV->GetWriterDeviceNumber();
		args = (const char **)malloc(sizeof(char *) * (22));
		args[arguments++] = strdup(CM->GetString(PATH_TO_CDRECORD)->String());
		args[arguments++] = strdup(device.String());
		args[arguments++] = strdup("-msinfo");
		args[arguments]=NULL;
		
		printArguments("cdrecord multisessioninfo", args);

		Thread=pipe_command(arguments, args, in, out, err);
		set_thread_priority(Thread,saveCV->GetBurnPriority()/*CDRECORD_PRIORITY*/);
		if (Thread<0) {
			errtype=_T("Error"); // "TERROR"
			errstring="cdrecord\n\n";
			errstring << _T("Could not load this command line tool. Make sure it is in the correct place."); // "TCOULDNOTLOAD"
			free(args);
			HideStatus();
			return B_ERROR;
		}
		rename_thread(Thread,CDRECORDTHREADNAME);
		resume_thread(Thread);
		
		size_t bytesread=1;
		char buffer[10000];
		BString s, startsec, endsec;
		
		end=-1;
		start=0;
		while ((bytesread>0) || (Thread==find_thread(CDRECORDTHREADNAME))) {
//		while (Thread==find_thread(CDRECORDTHREADNAME)) {
			bytesread=read(out,(void *)buffer, 10000);
			buffer[bytesread]=0;
			if (bytesread>0) {

				// get start and end sector from stdout of cdrecord
				if (strstr(buffer,",")) {
					s=buffer;
					startsec=s.Remove(s.FindFirst(","), s.Length()-s.FindFirst(","));
					start=atol(startsec.String());
					s=buffer;
					endsec=s.Remove(0,s.FindFirst(",")+1);
					end=atol(endsec.String());
					kill_thread(Thread);
				} 

			}
			snooze(10000);
		}
		close(in);close(out);close(err);

		free(args);
		if (end>=start)
		return B_OK; else
		return B_ERROR;
}


void
Application1::ExportAsM3U(BEntry *entry)
{
	BFile 	*file=new BFile(entry, B_READ_WRITE | B_ERASE_FILE | B_CREATE_FILE);
	M3UList *list=new M3UList();
	BString path="";
	BEntry	*musicfile=new BEntry();
	BPath	pathtomusicfile;
	AudioRow	*ai;
	BString		filenumber;
	BString		title;
	BString		time;
	int32		count=0;
	
	list->AddItem("[playlist]");
	window1->Lock();
	if (window1->view1->listview1->iCountItems()>0) {
		for (int32 index=0; index<window1->view1->listview1->iCountItems(); index++) {
			ai=(AudioRow *)window1->view1->listview1->RowAt(index);
			
			if ((!ai->IsSplitted()) || (ai->GetSplitIndex()==0)) {
				// create file number entry
				// like this: "File3"
				count++;
				filenumber="File";
				filenumber << count << "=";
				path.SetTo(ai->GetFilename()).Prepend(base_folder);
				musicfile->SetTo(path.String(),true);
				musicfile->GetPath(&pathtomusicfile);
				// generate Title entry
				title="Title=";
				title.Append(ai->GetFilename());
				title.RemoveLast(".mp3");
				title.RemoveLast(".wav");
				title.RemoveLast(".aiff");
				title.RemoveLast(".aif");
				title.RemoveLast(".WAV");
				title.RemoveLast(".MP3");
				title.RemoveLast(".AIFF");
				title.RemoveLast(".AIF");
				title.RemoveLast(".ogg");
				title.RemoveLast(".OGG");
				// generate Time-line if track is not splitted
				if (!(ai->GetSplitIndex()==0)) {
					time="Time=";
					time << (int)ai->GetLength();
					list->AddItem(new BString(time));
				}
				
				list->AddItem(new BString(title));
				filenumber.Append(pathtomusicfile.Path());
				list->AddItem(new BString(filenumber));
			}
		}
		title.SetTo("NumberOfEntries=");
		title << count;
		list->AddItem(new BString(title));
		list->SaveToFile(entry);
	}
	BNode		*node=new BNode(entry);
	BNodeInfo	*nodeinfo=new BNodeInfo(node);
	nodeinfo->SetType("text/x-playlist");
	delete nodeinfo;
	node->Unset();
	delete node;

	musicfile->Unset();
	file->Unset();
	delete file;
	delete musicfile;
	delete list;
	window1->Unlock();
}


// APML is the APlayer Playlist file format (APlayer Module List)
void
Application1::ExportAsAPML(BEntry *entry)
{
	BFile 	*file=new BFile(entry, B_READ_WRITE | B_ERASE_FILE | B_CREATE_FILE);
	M3UList *list=new M3UList();
	BString path="";
	BEntry	*musicfile=new BEntry();
	BPath	pathtomusicfile;
	AudioRow	*ai;
	BString		filenumber;
	BString		title;
	BString		time;
	BString		lastpath=":";
	BDirectory	dir;
	
	list->AddItem("@*ApML*@");
	window1->Lock();
	if (window1->view1->listview1->iCountItems()>0) {
		for (int32 index=0; index<window1->view1->listview1->iCountItems(); index++) {
			ai=(AudioRow *)window1->view1->listview1->RowAt(index);
			
			// path
			path.SetTo(ai->GetFilename()).Prepend(base_folder);
			musicfile->SetTo(path.String(),true);
			musicfile->GetParent(&dir);
			dir.GetEntry(musicfile);
			musicfile->GetPath(&pathtomusicfile);
			if (lastpath.Compare(pathtomusicfile.Path())!=0) {
				list->AddItem("");
				list->AddItem("@*Path*@");
				lastpath=pathtomusicfile.Path();
				if (lastpath[lastpath.Length()-1]=='/') lastpath.RemoveLast("/");
				list->AddItem(new BString(lastpath));
				list->AddItem("");
				list->AddItem("@*Names*@");
			}
			if ((!ai->IsSplitted()) || (ai->GetSplitIndex()==0))
				list->AddItem(new BString(ai->GetFilename()));
		}
		list->SaveToFile(entry);
	}
	BNode		*node=new BNode(entry);
	BNodeInfo	*nodeinfo=new BNodeInfo(node);
	nodeinfo->SetType("text/x-aplayer-playlist");
	delete nodeinfo;
	node->Unset();
	delete node;

	dir.Unset();
	musicfile->Unset();
	file->Unset();
	delete file;
	delete musicfile;
	delete list;
	window1->Unlock();
}


void
Application1::GetEntryNamesIn(BDirectory dir, M3UList *list)
{
	const int32 c = dir.CountEntries();
	if (c>0) {
		for (int32 i=0; i<c; i++) {
			BEntry entry;
			dir.GetNextEntry(&entry, true);
			if (entry.IsDirectory()) {
				GetEntryNamesIn(BDirectory(&entry), list);
			} else {
				BPath	path;
				entry.GetPath(&path);
				list->AddItem(new BString(path.Path()));
				path.Unset();
			}
			entry.Unset();
		}
	}
}


void
Application1::ExportAsFileList(BEntry *entry)
{
	BFile 	*file=new BFile(entry, B_READ_WRITE | B_ERASE_FILE | B_CREATE_FILE);
	M3UList *list=new M3UList();
	BEntry	*musicfile=new BEntry();
	BString		lastpath=base_folder;
	BDirectory	dir;
	
	if (lastpath[lastpath.Length()-1]=='/') lastpath.RemoveLast("/");
	dir.SetTo(lastpath.String());
	GetEntryNamesIn(dir, list);

	list->SaveToFile(entry);

	BNode		*node=new BNode(entry);
	BNodeInfo	*nodeinfo=new BNodeInfo(node);
	nodeinfo->SetType("text/plain");

	node->Unset();
	dir.Unset();
	musicfile->Unset();
	file->Unset();
	delete nodeinfo;
	delete node;
	delete file;
	delete musicfile;
	delete list;
}


bool
Application1::CheckCDSize()
{
	int64 	maxsize=80;
	int	blocksize;
	int64	maxsize_bytes;
	
	if (heliosCV->IsOverburning())
		maxsize=90;
	switch(window1->view1->CDType()) {
		case DATACD_INDEX: {
			blocksize=2048;
			break;
		}

		case AUDIOCD_INDEX: {
			blocksize=2352;
			break;
		}

		case BOOTABLECD_INDEX: {
			blocksize=2048;
			break;
		}

		case CDEXTRA_INDEX: {
			blocksize=2352;
			break;
		}

		// no checking for DVDs
		case DVDVIDEO_INDEX: {
			return true;
		}

		default: {
			blocksize=2048;
			break;
		}
	}
		
	maxsize_bytes=(int64)((maxsize*60*176400*(int64)blocksize)/2352);
	
	return window1->view1->box2->GetCDSize()<=maxsize_bytes;
}


void Application1::ExportAsInf(const char *filename, int32 index) {

window1->Lock();
	InfoFile		*inf=new InfoFile(filename);
	AudioRow	*ai=(AudioRow *)window1->view1->listview1->RowAt(index);
	char		albumtitle[160];
	

	
	switch(window1->view1->CDType()) {
		case AUDIOCD_INDEX: {
			sprintf(albumtitle, "%s",window1->view1->GetCDAlbum());
			break;
		}
		case CDEXTRA_INDEX: {
			sprintf(albumtitle, "%s",window1->view1->GetCDAlbumCDExtra());
			break;
		}
		default: {
			break;
		}
	}
	
	inf->SetPerformer("");
	inf->SetAlbumtitle(albumtitle);
	inf->SetTracktitle(ai->GetCDTitle());
	inf->Write();
	delete inf;

	window1->Unlock();
}

// creates a filename with serial numbers which are for temporarily use only
const char *Application1::GenerateInfFilename(int32 index) {

	BString	basename;
	BString	pathtoinf;
	BString	final="";
	
	pathtoinf.SetTo(base_folder.String());

	basename.SetTo(((AudioRow *)(window1->view1->listview1->RowAt(index)))->GetFilename());
	basename.RemoveLast(".mp3");
	basename.RemoveLast(".wav");
	basename.RemoveLast(".MP3");
	basename.RemoveLast(".WAV");
	basename.RemoveLast(".AIFF");
	basename.RemoveLast(".AIF");
	basename.RemoveLast(".aiff");
	basename.RemoveLast(".aif");
	basename.RemoveLast(".ogg");
	basename.RemoveLast(".OGG");
	basename.RemoveLast(".mpeg");
	basename.RemoveLast(".MPEG");
	basename.RemoveLast(".mpeg3");
	basename.RemoveLast(".MPEG3");
	basename.RemoveLast(".mpeg1");
	basename.RemoveLast(".MPEG1");
	//if (pathtoinf[pathtoinf.Length()-1]!='/') pathtoinf.Append("/");
	AppendTrailingSlash(&pathtoinf);
	basename << ".inf";
		
	final.SetTo(pathtoinf.String());
	final.Append(basename.String());
	printf("HELIOS: >%s<\n",final.String());
	return strdup(final.String());
}

// writes info files for all tracks kept by the current project
void Application1::SaveCDTEXTAlbum() {
	
	window1->Lock();
	for (int32 index=0; index<window1->view1->listview1->iCountItems();index++) {
		// call export routine
		ExportAsInf(GenerateInfFilename(index), index);
	}	
	window1->Unlock();	
}


// remove the temporary files (info files for audio-cd burning)
// located in the temp-path of helios
void
Application1::RemoveInfFiles()
{
	BEntry	*entry = new BEntry();
	
	window1->Lock();
	for (int32 index=0; index<window1->view1->listview1->iCountItems();index++) {
		entry->SetTo(GenerateInfFilename(index),true);
		if (entry->Exists()) {
			entry->Remove();
		}
		entry->Unset();
	}	
	delete entry;
	window1->Unlock();	
}


BString
SubString(BString *string, int32 first, int32 last)
{
	BString temp;
	
	temp.SetTo(string->String());
	temp.Remove(last,temp.Length()-last);
	temp.Remove(0, first);
	return temp;
}


void
Application1::PopulateFoldersMenu()
{
	BString current=current_folder;
	current.RemoveLast("/");
	BitmapMenuItem	*mi;
	BMessage	*msg;
	BEntry		entry;
	entry_ref	ref;
	
	window1->Lock();
//	window1->view1->listview1->fStatusView->RemoveMenuItems();
	if (current.Length()>base_folder.Length()) 
		current.Remove(current.FindLast("/"), current.Length()-current.FindLast("/"));

	while (current.Length()>base_folder.Length()) {
		msg=new BMessage(FOLDERMENU_MSG);
		msg->AddString("folder", current);
		if (entry.SetTo(current.String(), true)==B_OK) {
			entry.GetRef(&ref);
			entry.Unset();
		}
		mi=new BitmapMenuItem(SubString(&current, current.FindLast("/")+1, current.Length()).String(), GetIcon(&ref), msg);
//		window1->view1->listview1->fStatusView->AddMenuItem(mi);
		current.Remove(current.FindLast("/"), current.Length()-current.FindLast("/"));
	}
	current=current_folder;
	if (current.Length()>base_folder.Length()) {
		BString t="";
		if (window1->view1->CDType()==DATACD_INDEX) t=window1->view1->GetVolumeName();
		if (window1->view1->CDType()==BOOTABLECD_INDEX) t=window1->view1->GetVolumeNameBootCD();
		msg=new BMessage(FOLDERMENU_MSG);
		msg->AddString("folder", base_folder);
		entry.SetTo(base_folder.String(), true);
		entry.GetRef(&ref);
		entry.Unset();
		mi=new BitmapMenuItem(t.String(), GetIcon(&ref),  msg);
//		window1->view1->listview1->fStatusView->AddMenuItem(mi);
	}
	window1->Unlock();
}


// "path" is a devfs-path
size_t
Application1::GetDeviceSize(const char *path)
{
	int 	fd=open(path, O_RDONLY);
	size_t	data;
	
	ioctl(fd, B_GET_DEVICE_SIZE, &data);
	close(fd);
	return data;
}

/*

If you like to append a filesystem to a audio CD, first extract the 
CD by calling:

	cdda2wav -B -v255

This first audio session may be written in TAO mode with the command

	cdrecord -multi -audio file1 ....

or in DAO mode with the command

	cdrecord -dao -multi -useinfo -audio file1 ....

To add the second session that contains the data track, you need
to create an ISO-9660 file system that starts not at sector 0.

With the current mkisofs (1.12), you must use the following method:


-	First call cdrecord -msinfo for your prepared multi-session audio CD.

	you will get something like 0,12345

-	Now call mkisofs:

	mkisofs -o fs.raw -C 0,12345 root_directory_for_new_cd

	replace 0,12345 with your actual cdrecord -msinfo output.

the image in fs.raw may now be written with cdrecord as second session.

See my README.multi for more info on how to create multi session CD's


The procedure again in short form:

	cdda2wav -v255 -B

	cdrecord -multi -audio audio_tracks ....
or
	cdrecord -dao -useinfo -multi -audio audio_tracks ....

	cdrecord -msinfo ....
	(output is e.g. 0,12345)

	mkisofs -R -o cd_plus.raw -C 0,12345 root_dir_of_fs

	cdrecord -data cd_plus.raw
or
	cdrecord -multi cd_plus.raw


Note: If you want to create an HFS hybrid as the data track, then you must
use the '-part' option to mkisofs. Otherwise, the data track will be mounted
as an ISO9660/Joliet CD when used on a Mac.
*/


status_t
Application1::BurnCDExtra()
{
	if (BurnAudio(true)==B_OK) {
		//if (GetMultisessionSectors(start, end)==B_OK) {
			load_media(deviceCV->GetWriterDevicePath());

			BString	bf=base_folder;
			BString tp;
		
			base_folder=window1->view1->GetImageNameCDExtra();
			AppendTrailingSlash(&base_folder);

			if ((!saveCV->IsStreamed())
					||
				/* bfs? */ (window1->view1->GetFilesystem()==2))
				MakeImage_NOSTREAMING();
			else
				if (saveCV->IsStreamed())
					MakeImage();

			base_folder=bf;
		//}
	}
	return B_OK;
}

void
Application1::DeleteAndCount(BDirectory dir)
{
	const int32 c = dir.CountEntries();
	if (c>0) {
		for (int32 i=0; i<c; i++) {

			{
				BEntry entry;
				dir.GetNextEntry(&entry, false);
				if (entry.IsDirectory()) {
					DeleteAndCount(BDirectory(&entry));
					entry.Remove();
				} else {
					entry.Remove();
				}
				entry.Unset();
			}
			
			if (filecounter % 20==0) {
				if (window1->Lock()) {
					BString tmpstring;
					tmpstring << _T("Removing files") << B_UTF8_ELLIPSIS << " (" << filecounter << ")"; // "L:Removing files..."
	//				sprintf(text, "%s (%ld)", _T("L:Removing files..."), filecounter);
					window1->view1->statusBAR->SetText( tmpstring.String() );
					window1->Unlock();
				}
			}
			filecounter++;
		}
	}
}


void
Application1::PlayFinishedBurningSound() {

	if (soundCV->HasFinishedWritingSound()) {
		if (system_beep(WRITTEN_BEEP_EVENT)!=B_OK) {
			if (add_system_beep_event(WRITTEN_BEEP_EVENT)==B_OK) {
				system_beep(WRITTEN_BEEP_EVENT);
			} else {
				ErrorBox *eb=new ErrorBox(E_RED_COLOR,	_T("Error"), // "TERROR"
														_T("When tried to add a new system event there occurred an error. To suppress this error message, turn of sound events in the 'Sounds' preferences Panel of Helios."), // "TCOULDNOTADDSOUNDEVENT"
														_T("Ok")); // "TOK"
				eb->Go();
			}
		}
	}
}


void
Application1::PlayFinishedImageSound()
{
	if (soundCV->HasFinishedImageSound()) {
		if (system_beep(IMAGE_BEEP_EVENT)!=B_OK) {
			if (add_system_beep_event(IMAGE_BEEP_EVENT)==B_OK) {
				system_beep(IMAGE_BEEP_EVENT);
			} else {
				ErrorBox *eb=new ErrorBox(E_RED_COLOR,	_T("Error"), // "TERROR"
														_T("When tried to add a new system event there occurred an error. To suppress this error message, turn of sound events in the 'Sounds' preferences Panel of Helios."), // "TCOULDNOTADDSOUNDEVENT"
														_T("Ok")); // "TOK"
				eb->Go();
			}
		}
	}
}


void
Application1::PlayErrorSound()
{
	if (soundCV->HasErrorSound()) {
		if (system_beep(ERROR_BEEP_EVENT)!=B_OK) {
			if (add_system_beep_event(ERROR_BEEP_EVENT)==B_OK) {
				system_beep(ERROR_BEEP_EVENT);
			} else {
				ErrorBox *eb=new ErrorBox(E_RED_COLOR,	_T("Error"), // "TERROR"
														_T("When tried to add a new system event there occurred an error. To suppress this error message, turn of sound events in the 'Sounds' preferences Panel of Helios."), // "TCOULDNOTADDSOUNDEVENT"
														_T("Ok")); // "TOK"
				eb->Go();
			}
		}
	}
}


// optimized for 1440k-floppy disks
status_t
Application1::ReadFloppyImage(const char *path)
{
	BFile	*src, *dst;
	ssize_t	numread;
	char	*buffer=new char[36*512];
	char	info[20];
	int	buffersize=18*512;
	const int sectorsize=512;
	float	progressed=0, sum=0;
	bool	retval=true;
	BString tmpstring;

	src=new BFile("/dev/disk/floppy/raw", B_READ_ONLY);
	dst=new BFile(path, B_READ_WRITE | B_CREATE_FILE);
	
	BStopWatch *watch=new BStopWatch("kBps",true);
	tmpstring << _T("Reading image file") << B_UTF8_ELLIPSIS; // "Status:Reading Image File"
	ShowStatus( tmpstring.String(), -1);
	SetStatusInfo("--- kB/s");

	if ((src->InitCheck()==B_OK) && (dst->InitCheck()==B_OK)) {
		dst->SetSize(0);
		
		numread=src->Read(buffer, buffersize);
		while ((dst->Write(buffer, numread)==numread) && (buffersize>=sectorsize)) {
			numread=src->Read(buffer, buffersize);
			if ((numread<=0) && (buffersize>sectorsize)) {
				buffersize/=2;
				printf("HELIOS: Reducing Buffer Size To %d Bytes.\n", buffersize);
			}
			if (buffersize<=sectorsize) buffersize=0;
			if (fStatusWindow->interrupted) {
				retval=false;
				break;
			}

			progressed+=numread;
			if (progressed>18*512) {
					sprintf(info,"%0.1f kB/s",((float)progressed)/(((float)watch->ElapsedTime())*0.001024F));
					SetStatusInfo(info);
					watch->Reset();
					sum+=progressed;
					SetPercentage(100*sum/(1440*1024));
					progressed=0.0F;
			}

		}
		dst->Sync();
	} else {
		retval=false;
	}

	src->Unset();
	dst->Unset();
	delete src;
	delete dst;

	HideStatus();

	delete [] buffer;
	delete watch;
	return B_OK;
}

