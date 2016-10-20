#ifndef _VIEW1_H_
#define _VIEW1_H_

#include <View.h>
#include <Bitmap.h>
#include <StringView.h>
#include <Font.h>
#include <Box.h>
#include <MenuField.h>
#include <PopUpMenu.h>
#include <MenuItem.h>
#include <Application.h>
#include <TextControl.h>
#include <TextView.h>
#include <ScrollView.h>
#include <ListView.h>
#include <RadioButton.h>
#include "InfoBox.h"
#include "ListView1.h"
#include "Defines.h"
#include <Bitmap.h>
#include "Stringview1.h"
#include <MenuBar.h>
#include "AudioCDPanel.h"
#include "DataCDPanel.h"
#include "BootableCDPanel.h"
#include "CDExtraPanel.h"
#include "CDTypeMenu.h"
#include "IconBar.h"
#include "StatusBarView.h"

#ifdef INCLUDE_DVD
#include "VideoDVDPanel.h"
#endif

class View1 : public BView
{
public:
	View1(BRect rect);
	virtual ~View1(void);

	int CDType();
	void SetCDType(int32 index);

	// DATA CD
	const char* GetVolumeName();
	void SetVolumeName(const char* name);
	bool IsMultisession();
	void SetMultisession(bool flag);
	int32 GetFilesystem();
	void SetFilesystem(int32 index);

	// AUDIO CD
	bool WriteCDText();
	bool ProtectAudioCD();
	void SetCDText(bool flag);
	void SetProtection(bool flag);
	void SetCDAlbum(const char* name);
	const char* GetCDAlbum();

	// BOOTABLE CD
	const char* GetVolumeNameBootCD();
	void SetVolumeNameBootCD(const char* name);
	const char* GetImageNameBootCD();
	void SetImageNameBootCD(const char* name);
	int32 GetFilesystemBootCD();
	void SetFilesystemBootCD(int32 index);

	// CD EXTRA
	const char* GetImageNameCDExtra();
	void SetImageNameCDExtra(const char* name);
	bool WriteCDTextCDExtra();
	void SetCDTextCDExtra(bool flag);
	void SetCDAlbumCDExtra(const char* name);
	const char* GetCDAlbumCDExtra();
	const char* GetVolumeNameCDExtra();
	void SetVolumeNameCDExtra(const char* name);
	int32 GetFilesystemCDExtra();
	void SetFilesystemCDExtra(int32 index);

#ifdef INCLUDE_DVD
	// DVD VIDEO
	const char* GetVolumeNameDVDVideo();
	void SetVolumeNameDVDVideo(const char* name);
	int32 GetFilesystemDVDVideo();
	void SetFilesystemDVDVideo(int32 index);
#endif

	void SetCurrentPath(const char* text);

	void UpdatePanels();
	virtual void AllAttached(void);
	virtual void Draw(BRect updateRect);
	BFont* font1;
	InfoBox* box2;
	BMenuField* menufield2;
	ListView1* listview1;
	BTextControl* textcontrol2;
	BButton* button1;

	BMenuBar* mainMB;
	BMenu* fileM, *imageM, *copycdM, *rwM, *windowM;
	BMenu* exportSM, *openrecentSM, *protocolSM, *cdimgSM, *floppyimgSM; // sub-menu
	BMenuItem* newprojectMI, *openprojectMI, *saveprojectMI, *saveprojectasMI, *standardprojectMI,
		*removeprojectMI, *preferencesMI, *cdrecord_outputMI, *mkisofs_outputMI, *aboutMI, *exitMI,
		*readcdimageMI, *createfileimageMI, *burnimagefileMI, *readfloppyimagefileMI,
		*readaudiocdMI, *copyaudiocdMI, *copydatacdMI, *fastblankMI, *completeblankMI,
		*unclosesessionMI, *blanksessionMI, *export_M3U, *export_APML, *export_FileList,
		*export_TrackerTemplate;

	CDTypeMenu* CDTypeMF;

	BBox* CDTypeBOX;

	AudioCDPanel* audiopanel;
	DataCDPanel* datapanel;
	BootableCDPanel* bootablepanel;
	CDExtraPanel* cdextrapanel;

#ifdef INCLUDE_DVD
	VideoDVDPanel* dvdvideopanel;
#endif

	StatusBar* statusBAR;
	IconBar* mainBAR;
	IconBar* iconBAR;

	float list_left, menu_height;

private:
	BTextControl* IBpathDisplayTC;
};

#endif
