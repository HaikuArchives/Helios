#ifndef _SAVECONFIGVIEW_H_
#define _SAVECONFIGVIEW_H_

#include "ConfigView.h"
#include <Slider.h>
#include <CheckBox.h>
#include <TextControl.h>
#include <String.h>
#include <StringView.h>
#include <RadioButton.h>
#include <PopUpMenu.h>
#include <MenuField.h>
#include <MenuItem.h>
#include <Resources.h>
#include "EditPopUpMenu.h"
#include "EditMenuItem.h"

typedef enum {
	EM_SAVE_PROJECT=0,
	EM_ASK_USER,
	EM_DO_NOTHING
} exit_modes;

class SaveConfigView : public ConfigView {
public:
	SaveConfigView(BRect frame);
	~SaveConfigView();
	
	bool	IsStreamed();
	void	SetStreamed(bool);
	
	bool	IsSavedWhenQuit();
	void	SetSaveWhenQuit(bool);

	int32	GetBurnPriority();
	void	SetBurnPriority(int32 priority);
	
	exit_modes	GetExitMode();
	void	SetExitMode(exit_modes);

	bool	AutoUpdate();
	void	SetAutoUpdate(bool);

	bool	BalloonHelp() {return balloonhelpCB->Value()==B_CONTROL_ON;}
	void	SetBalloonHelp(bool);
	
	void	Invalidate();
	
	virtual void	MessageReceived(BMessage*);

private:
	
	void	AllAttached();
	void	ReorderLanguages(BResources *);
	
	BCheckBox	*streamedCB,
			*savewhenquitCB,
			*autoupdateCB,
			*balloonhelpCB;
	BRadioButton	*autosaveRB,
			*askforsaveRB,
			*donothingRB;
	BStringView	*onexitdoSV;
	BString		string;
	
	BStringView	*burnPrioritySV;
	BSlider		*burnPriority;
};

#endif
