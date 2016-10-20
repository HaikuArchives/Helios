#ifndef _AUDIOCDPANEL_H_
#define _AUDIOCDPANEL_H_

#include <View.h>
#include <CheckBox.h>
#include <TextControl.h>

class AudioCDPanel : public BView
{
public:
	AudioCDPanel(BRect frame);
	~AudioCDPanel();

	void AllAttached(void);
	void MessageReceived(BMessage* msg);

	bool writeCDText();
	bool copyProtection();
	void SetCDText(bool flag);
	void SetProtection(bool flag);
	void SetCDAlbum(const char* name);
	const char* GetCDAlbum();

private:
	BCheckBox* cdtextCB, *copyprotectionCB;
	BTextControl* CDTextAlbum;
};

#endif
