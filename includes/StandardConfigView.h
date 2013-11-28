#ifndef _STANDARDCONFIGVIEW_H_
#define _STANDARDCONFIGVIEW_H_

#include "ConfigView.h"
#include <Slider.h>
#include <CheckBox.h>
#include <TextControl.h>
#include <String.h>
#include <StringView.h>
#include <MenuField.h>
#include <PopUpMenu.h>
#include <TextControl.h>
#include <MenuItem.h>
#include "CDTypeMenu.h"
#include "BrowseField.h"

class StandardConfigView : public ConfigView {
public:
	StandardConfigView(BRect frame);
	~StandardConfigView();
	
	int32		GetFilesystem();
	void		SetFilesystem(int32);

	const char	*GetVolumeName();
	void		SetVolumeName(const char *);
	
	int8		GetCDType();
	void		SetCDType(int8);
	
	const char 	*GetPublisher();
	void		SetPublisher(const char *);

	const char 	*GetPreparer();
	void		SetPreparer(const char *);

	const char 	*GetApplication();
	void		SetApplication(const char *);

//	const char 	*GetTemporaryPath();
//	void		SetTemporaryPath(const char *);
//	
//	const char	*GetProjectPath();
//	void		SetProjectPath(const char *);
	
private:
	
	void	MessageReceived(BMessage*);

	CDTypeMenu	*CDTypeM;
	BPopUpMenu	*filesystemPUM;
	BMenuField	*filesystemMF;
	BMenuItem	*iso9660MI,
			*iso9660longMI,
			*bfsMI,
			*hfsMI,
			*rockridgeMI,
			*jolietMI;
	BTextControl	*publisherTC,
			*preparerTC,
			*applicationTC,
			*volumenameTC;
	BrowseField	*temppathTC,
			*projectpathTC;
};

#endif
