#ifndef _APPLICATION1_H_
#define _APPLICATION1_H_

#include <Alert.h>
#include <Application.h>
#include <Bitmap.h>
#include <CAM.h>
#include <FilePanel.h>
#include <List.h>
#include <OS.h>
#include <scsi.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <StopWatch.h>
#include <storage/Directory.h>
#include <storage/Entry.h>
#include <storage/File.h>
#include <storage/Node.h>
#include <storage/NodeInfo.h>
#include <storage/Path.h>
#include <SymLink.h>
#include <unistd.h>
#include <Window.h>

#include "ApplicationConfigView.h"
#include "AudioWindow.h"
#include "ColorConfigView.h"
#include "ConfigManager.h"
#include "ConfigWindow.h"
#include "DeviceConfigView.h"
#include "FSPanel.h"
#include "HeliosConfigView.h"
#include "ImageConfigView.h"
#include "LanguageConfigView.h"
#include "M3UList.h"
#include "MediaInfo.h"
#include "PathConfigView.h"
#include "ProtocolWindow.h"
#include "SaveConfigView.h"
#include "SoundConfigView.h"
#include "StatusWindow.h"
#include "StandardConfigView.h"
#include "VersionConfigView.h"
#include "Window1.h"
#include "Window2.h"


class Application1 : public BApplication {
public:
	Application1();
	~Application1();

	virtual void ReadyToRun();
	virtual bool QuitRequested(void);
	virtual void MessageReceived(BMessage *message);
	virtual void RefsReceived(BMessage *message);
	virtual void AboutRequested(void);

	// Member variables (public currently, wil change in future)
	StatusWindow	*fStatusWindow;

	Window1				*window1;
	Window2				*window2;
	ConfigWindow	*configW;
	AudioWindow		*audioW;

	// this string contains a description of the last error occurred
	BString				errtype, errstring;

	bool					IsProject(const bool show_alert);

status_t GetMultisessionSectors(int32 &start, int32 &end);

void ShowFolder(bool audio=false);
status_t MakeImage();
status_t BurnAudio(bool multisession);
status_t BurnCDExtra();
status_t BlankRW(char *mode);
bool GetImageSize();
void ChangePauses();
void Update_DATA_AUDIO();
status_t MakeImage_NOSTREAMING();
void ReadOutputOfCdrecord(int outstream, thread_id thread, int tracks=1);
void MoveUp();
void MoveDown();

status_t ReadImage(BString *filename, bool alloweject=true);

// Status window functions - need to move into separate class.
void HideStatus();
void ShowStatus(const char *text, thread_id Thread);
void SetStatus(const char *text, thread_id Thread);
void SetStatusInfo(const char *text, const char *leading="");

status_t MakeImageOnly(const char *filename);
void eject_media(const char * dev);
void load_media(const char * dev);
status_t CopyOnTheFly();
status_t CopyOnTheFlyAudio();
status_t CopyAudioCD();
void DoAudioMenu();
void DoDataMenu();
void Calculate_time();
void GetFolder(BDirectory dir);
void UpdateTrackNumbers();
float ItemLength(BStringItem *item);
void SetPercentage(float percentage);
int DeviceStatus(const char *path);
status_t CopyDataCD();
status_t WaitForDisc(const char *device);

void SaveSettings();
void ReadSettings();

void SetHelpBubbles();

const char *SuggestProjectName();
//void ShowOpenPanel();
//void ShowSavePanel();
//void ShowFolderPanel();
//void ShowExportPanel(BMessage *msg);

void LoadProject(BEntry *entry);
void SaveProject(BEntry *entry, bool export_project=false);
void NewProject();
void SetAsStandardProject();
void ExportAsM3U(BEntry *entry);
void ExportAsAPML(BEntry *entry);
void ExportAsFileList(BEntry *entry);
void GetEntryNamesIn(BDirectory dir, M3UList *list);
void AddToRecent(const char *filename);
void ShowRecentProjects();

void RemoveInfFiles();
const char *GenerateInfFilename(int32 index);
void ExportAsInf(const char *filename, int32 index);
void SaveCDTEXTAlbum();
//int MakeMKISOFSCommandline(const char **args, const char *filename, bool printsize=false);

// mkisofs-file system
status_t MakeMKISOImage(const char *filename);

	bool CheckCDSize();

int ReadAudioCD();
size_t GetDeviceSize(const char *path);

void PopulateFoldersMenu();
void DeleteAndCount(BDirectory dir);

void PlayFinishedBurningSound();
void PlayFinishedImageSound();
void PlayErrorSound();

status_t	ReadFloppyImage(const char *path);
void SetWorking(bool state=true);

//void AddFileTrackNumbers();
//void RemoveFileTrackNumbers();

sem_id streambuffer_sem;
sem_id		fileadder_sem, fileremover_sem;



//settings_struct settings;


//struct projectStruct {
//	char	_currentpath[B_PATH_NAME_LENGTH];
//	char	_basepath[B_PATH_NAME_LENGTH];
//	int	_filesystem;		// menuitem index
//	char 	_volumename[64];
//	char 	_cdtype;		// 0 for data, 1 for audio
//	char	_publisher[128];
//	char	_preparer[128];
//	char	_application[128];
//	char	_itemBlocksFollowing;
//};
//
//struct audioBlock {
//	char	_cdtext_title[161];
//	float	_pause;
//	float	_indexList[99];
//	char	_filename[B_PATH_NAME_LENGTH];
//	size_t	_length;
//};

BMessage	listview_content,
		application_settings;

char donow;
//BFilePanel	*filepanel;
//BStringView	*filepanelSV;
BString		currentproject;
BString base_folder;
BString current_folder;
BString cdir;
BString pathdisplay;
long imagesize;
BStopWatch *watch;
bool addershows;
int number;

//ToolTip *tooltip1, *tooltip2;
//BBitmap *bmp;

int32 filename_col;
bool save_and_quit;
bool is_all_showing;
bool just_quit;
size_t	cdextra_datasize;

int32 filecounter;

typedef struct track_info {
	char	type[32];
	int		size;
} track_info;

ProtocolWindow	*cdrecord_output,
			*mkisofs_output;

ConfigManager	*CM;
HeliosConfigView	*heliosCV;
DeviceConfigView	*deviceCV;
PathConfigView	*pathCV;
ImageConfigView	*imageCV;
StandardConfigView	*standardCV;
ApplicationConfigView *applicationCV;
SaveConfigView	*saveCV;
SoundConfigView	*soundCV;
ColorConfigView	*colorCV;
#if defined(_BEOS_R5_BUILD_) || defined(_BEOS_HAIKU_BUILD_)
LanguageConfigView *langCV;
#endif
VersionConfigView	*versionCV;
FSPanel		*fspanelCV;
};

#endif
