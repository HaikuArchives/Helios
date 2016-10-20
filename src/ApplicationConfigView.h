#ifndef _APPLICATIONCONFIGVIEW_H_
#define _APPLICATIONCONFIGVIEW_H_

#include "ConfigView.h"
#include <Slider.h>
#include <CheckBox.h>
#include <TextControl.h>
#include <String.h>
#include <StringView.h>
#include <MenuField.h>
#include <PopUpMenu.h>
#include <TextControl.h>

class ApplicationConfigView : public ConfigView
{
public:
	ApplicationConfigView(BRect frame);
	~ApplicationConfigView();

	bool IsAutoUpdating();
	void SetAutoUpdating(bool);

	int GetUpdateFrequency();
	void SetUpdateFrequency(int seconds);

	bool IsTimeout();
	void SetTimeout(bool);

	int GetTimeoutLength();
	void SetTimeoutLength(int seconds);

private:
	void AllAttached();
	void AllDetached();
	void MessageReceived(BMessage*);

	BCheckBox* updatefreqCB, *timeoutCB;
	BSlider* updatefreqSL, *timeoutSL;
	BStringView* updatefreqSV, *timeoutSV;
};

#endif
