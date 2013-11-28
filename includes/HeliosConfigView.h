#ifndef _HELIOSCONFIGVIEW_H_
#define _HELIOSCONFIGVIEW_H_

#include "ConfigView.h"
#include <Slider.h>
#include <CheckBox.h>
#include <TextControl.h>
#include <String.h>
#include <StringView.h>

class HeliosConfigView : public ConfigView {
public:
	HeliosConfigView(BRect frame);
	~HeliosConfigView();
	
	bool	IsSimulation();
	void	SetSimulation(bool);
	
	bool	IsFixating();
	void	SetFixating(bool);
	
	bool	IsBurnproof();
	void	SetBurnproof(bool);
	
	bool	IsDAO();
	void	SetDAO(bool);
	
	bool	IsOverburning();
	void	SetOverburning(bool);

	int32	GetWriterSpeed();
	void	SetWriterSpeed(int32);
	
	int8	GetFIFOSize();
	void	SetFIFOSize(int8);

	bool	IsForcedErasing();
	void	SetForcedErasing(bool);

	BString	GetGeneralFlags();
	void		SetGeneralFlags(BString general);

	virtual void	GetSettings(BMessage *);
	virtual void	SetSettings(BMessage *);
private:
	
	void	AllAttached();
	void	MessageReceived(BMessage*);
	
	BCheckBox	*forcedCB,
			*simulationCB,
			*fixatingCB,
			*burnproofCB,
			*writerejectedCB,
			*readerejectedCB,
			*DAOCB,
			*overburningCB;
	BSlider		*speedSL,
			*fifoSL;
	BStringView	*speedSV,
			*fifoSV;
	BTextControl *generalFlagsTC;
	BString		string;
};

#endif
