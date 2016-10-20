#ifndef _VERSIONCONFIGVIEW_H_
#define _VERSIONCONFIGVIEW_H_

#include "ConfigView.h"
#include <Slider.h>
#include <CheckBox.h>
#include <TextControl.h>
#include <String.h>
#include <StringView.h>
#include <RadioButton.h>
#include "VersionControl.h"


class VersionConfigView : public ConfigView {
public:
	VersionConfigView(BRect frame);
	~VersionConfigView();
	
	void	SetHeliosVersion(const char *version, BBitmap *icon);
	void	SetCDRecordVersion(const char *version, BBitmap *icon);
	void	SetMkISOfsVersion(const char *version, BBitmap *icon);
	void	SetReadCDVersion(const char *version, BBitmap *icon);
	void	SetCDDA2WAVVersion(const char *version, BBitmap *icon);

private:
	void	MessageReceived(BMessage*);
	
	VersionControl		*heliosVC,
				*cdrecordVC,
				*mkisofsVC,
				*readcdVC,
				*cdda2wavVC;
};

#endif
