#ifndef _VIDEODVDPANEL_H_
#define _VIDEODVDPANEL_H_
#include <View.h>
#include <CheckBox.h>
#include <PopUpMenu.h>
#include <MenuField.h>
#include <MenuItem.h>
#include <TextControl.h>

class VideoDVDPanel : public BView
{
public:
	VideoDVDPanel(BRect frame);
	~VideoDVDPanel();

	void AllAttached(void);
	//	const char	*GetFileSystem();
	const char* GetVolumeName(void);
	void SetVolumeName(const char* name);
	BList* GetFilesystem();

	BPopUpMenu* filesystemPUM;

protected:
	virtual void MessageReceived(BMessage*);

private:
	BMenuField* filesystemMF;
	BMenuItem* mi;
	BMenuItem* DVDVideoMI, *DVDDataMI;
	BTextControl* volumenameTC;
};

#endif
