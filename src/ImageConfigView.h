#ifndef _IMAGECONFIGVIEW_H_
#define _IMAGECONFIGVIEW_H_

#include "ConfigView.h"
#include <Slider.h>
#include <CheckBox.h>
#include <TextControl.h>
#include <String.h>
#include <StringView.h>
#include <MenuField.h>
#include <PopUpMenu.h>
#include <TextControl.h>

class ImageConfigView : public ConfigView
{
public:
	ImageConfigView(BRect frame);
	~ImageConfigView();

	const char* GetPublisher();
	void SetPublisher(const char*);

	const char* GetPreparer();
	void SetPreparer(const char*);

	const char* GetApplication();
	void SetApplication(const char*);

private:
	void MessageReceived(BMessage*);

	BTextControl* publisherTC, *preparerTC, *applicationTC;
};

#endif
