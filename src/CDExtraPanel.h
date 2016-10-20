#ifndef _CDEXTRAPANEL_H_
#define _CDEXTRAPANEL_H_

#include <View.h>
#include <CheckBox.h>
#include <TextControl.h>
#include <PopUpMenu.h>
#include <MenuField.h>

class CDExtraPanel : public BView
{
public:
	CDExtraPanel(BRect frame);
	~CDExtraPanel();

	void AllAttached(void);
	void MessageReceived(BMessage* msg);

	bool writeCDText();
	bool copyProtection();
	void SetCDText(bool flag);
	void SetProtection(bool flag);
	void SetCDAlbum(const char* name);
	const char* GetCDAlbum();
	void SetImageName(const char* filename);
	const char* GetImageName();
	//	const char	*GetFileSystem();
	const char* GetVolumeName(void);
	void SetVolumeName(const char* name);
	BList* GetFilesystem();

	BPopUpMenu* filesystemPUM;

private:
	BCheckBox* cdtextCB, *copyprotectionCB;
	BTextControl* CDTextAlbum;
	BTextControl* imagenameTC;
	BMenuField* filesystemMF;
	BMenuItem* mi;
	BTextControl* volumenameTC;
};

#endif
