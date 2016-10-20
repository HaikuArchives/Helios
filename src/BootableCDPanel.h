#ifndef _BOOTABLECDPANEL_H_
#define _BOOTABLECDPANEL_H_
#include <View.h>
#include <CheckBox.h>
#include <PopUpMenu.h>
#include <MenuField.h>
#include <MenuItem.h>
#include <TextControl.h>

class BootableCDPanel : public BView
{
public:
	BootableCDPanel(BRect frame);
	~BootableCDPanel();

	void AllAttached(void);
	//	const char	*GetFileSystem();
	const char* GetVolumeName(void);
	void SetVolumeName(const char* name);
	const char* GetImageName(void);
	void SetImageName(const char* name);
	void MessageReceived(BMessage* msg);
	BList* GetFilesystem();

	BPopUpMenu* filesystemPUM;

private:
	BMenuField* filesystemMF;
	BMenuItem* mi;
	BTextControl* volumenameTC, *eltoritoimageTC;
};

#endif
