#ifndef _PATHCONFIGVIEW_H_
#define _PATHCONFIGVIEW_H_

#include "ConfigView.h"
#include <Slider.h>
#include <CheckBox.h>
#include <TextControl.h>
#include <String.h>
#include <StringView.h>
#include <MenuField.h>
#include <PopUpMenu.h>
#include <TextControl.h>
#include "BrowseField.h"

class PathConfigView : public ConfigView
{
public:
	PathConfigView(BRect frame);
	~PathConfigView();

	const char* GetTemporaryPath();
	void SetTemporaryPath(const char*);

	const char* GetProjectPath();
	void SetProjectPath(const char*);

	const char* GetDefaultImageName();
	void SetDefaultImageName(const char*);

	const char* GetDefaultMountPoint();
	void SetDefaultMountPoint(const char*);

private:
	void AllAttached();
	void MessageReceived(BMessage*);

	BrowseField* temppathTC, *projectpathTC, *imagenameTC, *mountpointTC;
};

#endif
