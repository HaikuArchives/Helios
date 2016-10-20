#define HELIOSPROJECTMIMETYPE "application/x-vnd.Helios-project"

#define HELIOS_LANGUAGE_UPDATE_MSG 'H$$L'

#define BURN_MSG 'HB01'
#define UPDATEINFO_MSG 'HB03'
#define IMAGE_MSG 'HB04'
#define READIMAGE_MSG 'HB06'
#define BURN_IMAGE_MSG 'HB08'
#define CREATEIMAGE_MSG 'HB09'
#define COPY_CD_MSG 'HB10'
#define DELETE_FILE_MSG 'HCM1'
#define RENAME_FILE_MSG 'HCM3'
#define NEW_FOLDER_MSG 'HCM2'
#define MOVE_UP_MSG 'HCM4'
#define MOVE_DOWN_MSG 'HCM5'
#define BURNSPEED_MSG 'HSL1'
#define FIXATE_CHKBOX_MSG 'HCB1'
#define ABOUT_OK 'HB11'
#define SET_INDICES_MSG 'HCM6'
#define SPLIT_TRACK_MSG 'HCM7'
#define SETCDTEXTTITLE_MSG 'HCM8'
// main-menu-messaged
#define NEWPROJECT_MIMSG 'MI01'
#define OPENPROJECT_MIMSG 'MI02'
#define SAVEPROJECT_MIMSG 'MI03'
#define SAVEPROJECTAS_MIMSG 'MI04'
#define EXIT_MIMSG 'MI05'
#define READCDIMAGE_MIMSG 'MI06'
#define CREATEFILEIMAGE_MIMSG 'MI07'
#define BURNIMAGEFILE_MIMSG 'MI08'
#define COPYAUDIOCD_MIMSG 'MI09'
#define COPYDATACD_MIMSG 'MI10'
#define PREFERENCES_MIMSG 'MI11'
#define FASTBLANK_MIMSG 'MI12'
#define STANDARDPROJECT_MIMSG 'MI13'
#define COMPLETEBLANK_MIMSG 'MI14'
#define UNCLOSESESSION_MIMSG 'MI15'
#define BLANKSESSION_MIMSG 'MI16'
#define EXPORT_M3U_MIMSG 'MI17'
#define EXPORT_APML_MIMSG 'MI18'
#define EXPORT_FILELIST_MIMSG 'MI19'
#define REMOVEPROJECT_MIMSG 'MI20'
#define CDRECORDPROTOCOL_MIMSG 'MI21'
#define MKISOFSPROTOCOL_MIMSG 'MI22'
#define READFLOPPYIMAGE_MIMSG 'MI23'
#define EXPORT_TRACKERTEMPLATE_MIMSG 'MI24'
#define READAUDIOCD_MIMSG 'MI25'

#define LISTITEM_DROPPED 'He01'
#define FILESYSTEM_MSG 'He02'
#define PAUSECHANGE_MSG 'He03'
#define DATAUDIO_MSG 'He04'
#define OK_MSG 'He05'
#define CANCEL_MSG 'He06'
#define KILL_MSG 'He07'
#define FOLDERMENU_MSG 'He08' // user clicked folder in statusview's menu
#define SAVE_SETTINGS 'He09'  // configwindow has been closed
#define GOTOPARENT_MSG 'He10'

#define OPEN_FPMSG 'FPOP'
#define SAVE_FPMSG 'FPSV'
#define FOLDER_FPMSG 'FPFL'
#define EXPORT_M3U_FPMSG 'FP01'
#define EXPORT_APML_FPMSG 'FP02'
#define EXPORT_FILELIST_FPMSG 'FP03'

#define SETTINGS_FILE "/boot/home/config/settings/Helios.settings"
#define STANDARDPROJECTPATH "Standard-Project/"

// logical field indexes of the BColumnListView used
// DATA CD
#define ICON_FIELD 0
#define FILENAME_FIELD 1
#define SIZE_FIELD 2
// AUDIO CD
#define TRACKNUMBER_FIELD 0
#define PREGAP_FIELD 1
#define TRACKNAME_FIELD 2
#define LENGTH_FIELD 3
#define BYTES_FIELD 4
#define INDEX_FIELD 5
#define CDTEXT_FIELD 6

// THREAD NAMES          	 xxxxxxxxxyxxxxxxxxxyxxxxxxxxxyx
#define MKISOFSTHREADNAME "Helios-controlled - mkisofs"
#define CDRECORDTHREADNAME "Helios-controlled - cdrecord"
#define READCDTHREADNAME "Helios-controlled - readcd"
#define TORAWTHREADNAME "Helios-controlled - toRAW"
#define CPTHREADNAME "Helios-controlled - cp"
#define CDDA2WAVTHREADNAME "Helios-controlled - cdda2wav"
#define BUFFERTHREADNAME "Buffer Thread"
#define UPDATETHREADNAME "Update Thread"

// DEVICE ERRORS
#define E_NO_ERROR 0
#define E_NO_DISC 1
#define E_NOT_READY 2
#define E_MEDIA_CHANGED 3
#define E_DOOR_OPEN 4
#define E_ERROR 5

// THREAD PRIORITIES
#define CDRECORD_PRIORITY 21
#define MKISOFS_PRIORITY 21
#define BUFFER_PRIORITY 21
#define READCD_PRIORITY 16
#define CDDA2WAV_PRIORITY 15
#define UPDATETHREAD_PRIORITY 2

// CD-TYPE MENU-INDICES
#define DATACD_INDEX 0
#define AUDIOCD_INDEX 1
#define BOOTABLECD_INDEX 2
#define CDEXTRA_INDEX 3
// BSeparatorItem here...	4
#define DVDVIDEO_INDEX 5

// system beep event names
#define WRITTEN_BEEP_EVENT "Helios: CD finished"
#define IMAGE_BEEP_EVENT "Helios: Image written"
#define ERROR_BEEP_EVENT "Helios: Error occurred"

#define INCLUDE_DVD
