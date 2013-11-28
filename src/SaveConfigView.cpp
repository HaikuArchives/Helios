#include <stdio.h>

#include <Beep.h>
#include <Application.h>
#include <Window.h>
#include <FilePanel.h>
#include <Roster.h>
#include <Slider.h>

#if defined(_BEOS_R5_BUILD_) || defined(_BEOS_HAIKU_BUILD_)
#include "MSHLanguageMgr.h"
extern MSHLanguageMgr* gMSHLangMgr;
#define _T(str)	gMSHLangMgr->_T(str).String()
#define _TPS(str)	gMSHLangMgr->_T(str)
#else
#include <locale/Locale.h>
#endif

#include "Defines.h"
#include "ErrorBox.h"
#include "SaveConfigView.h"

#define INFOBAR_MSG	'SCV0'
#define SCV_LANGUAGE_MSG	'SCV1'
#define SCV_ADD_LANGUAGE_MSG	'SCV2'
#define BALLOONHELP_MSG	'SCV3'
#define BURNPRIORITYSLIDER_CHANGED	'SCV4'

SaveConfigView::SaveConfigView(BRect frame) :
	ConfigView(frame)
{
	// CHECKBOXES
	streamedCB=new BCheckBox(BRect(15,5,230,20), "scv_streamedCB", _T("Data streaming"), NULL); // "TSTREAMING"
	streamedCB->ResizeToPreferred();
	AddChild(streamedCB);
	savewhenquitCB=new BCheckBox(BRect(15,streamedCB->Frame().bottom + 5,230,40), "scv_savewhenquitCB", _T("Remember last project"), NULL); // "TSAVEWHENQUIT"
	savewhenquitCB->ResizeToPreferred();
	AddChild(savewhenquitCB);
	autoupdateCB=new BCheckBox(BRect(15,savewhenquitCB->Frame().bottom + 5,230,60), "scv_autoupdateCB", _T("Info box automatic refresh"), NULL); // "TAUTOUPDATEINFOBOX"
	autoupdateCB->ResizeToPreferred();
	AddChild(autoupdateCB);
	
	// STRINGVIEWS
	BString tempstring;
	tempstring << _T("On exit do") << B_UTF8_ELLIPSIS;  // "TONEXITDO_SV"
	onexitdoSV=new BStringView(BRect(15,autoupdateCB->Frame().bottom + 5, 280, 80), "scv_onexitdoSV", tempstring.String());
	onexitdoSV->ResizeToPreferred();
	AddChild(onexitdoSV);
	
	// RADIOBUTTONS
	autosaveRB=new BRadioButton(BRect(30,onexitdoSV->Frame().bottom + 5,300,100), "scv_autosaveRB", _T("Save project without asking."), NULL); // "TAUTOSAVE_RB"
	autosaveRB->ResizeToPreferred();
	AddChild(autosaveRB);
	askforsaveRB=new BRadioButton(BRect(30,autosaveRB->Frame().bottom + 5,300,120), "scv_askforsaveRB", _T("Ask whether project should be saved or not."), NULL); // "TASKFORSAVE_RB"
	askforsaveRB->ResizeToPreferred();
	AddChild(askforsaveRB);
	donothingRB=new BRadioButton(BRect(30,askforsaveRB->Frame().bottom + 5,300,140), "scv_donothingRB", _T("Nothing. Just quit."), NULL); // "TDONOTHING_RB"
	donothingRB->ResizeToPreferred();
	AddChild(donothingRB);

	// balloon help cb
//	balloonhelpCB=new BCheckBox(BRect(15,150,230,165), "scv_balloonhelpCB", _T("Bubble help"), // "CB:Bubble Help"
//			new BMessage(BALLOONHELP_MSG), B_FOLLOW_LEFT|B_FOLLOW_TOP);
//	AddChild(balloonhelpCB);
	 
	burnPrioritySV = new BStringView(BRect(15, donothingRB->Frame().bottom + 15, frame.Width()-15, donothingRB->Frame().bottom+30), "scv_burningPrioritySV", _T("Burning Thread Priority"));
	AddChild(burnPrioritySV);

	burnPriority = new BSlider(BRect(15, burnPrioritySV->Frame().bottom + 5, frame.Width()-15, burnPrioritySV->Frame().bottom + 15), "scv_burnPrioritySL", NULL, new BMessage(BURNPRIORITYSLIDER_CHANGED), 1, 2);
	burnPriority->SetHashMarks(B_HASH_MARKS_BOTTOM);
	burnPriority->SetHashMarkCount(2);
	burnPriority->SetKeyIncrementValue(1);
	burnPriority->SetLimitLabels(_T("Low"), _T("High"));
	AddChild(burnPriority);

	//infobarIBTI=new IBToggleItem();
}

void	SaveConfigView::Invalidate() {
	streamedCB->SetLabel(_T("Data streaming")); // "TSTREAMING"
	savewhenquitCB->SetLabel(_T("Remember last project")); // "TSAVEWHENQUIT"
}

SaveConfigView::~SaveConfigView() {
	//delete infobarIBTI;
}
	
bool
SaveConfigView::IsStreamed() {
	return streamedCB->Value()==B_CONTROL_ON;
}

void
SaveConfigView::SetStreamed(bool state) {
	streamedCB->SetValue(state ? B_CONTROL_ON : B_CONTROL_OFF);
}

bool
SaveConfigView::IsSavedWhenQuit() {
	return savewhenquitCB->Value()==B_CONTROL_ON;
}

void
SaveConfigView::SetSaveWhenQuit(bool state) {
	savewhenquitCB->SetValue(state ? B_CONTROL_ON : B_CONTROL_OFF);
}

int32
SaveConfigView::GetBurnPriority()
{
	// Convert slider value to real priority for thread.
	int32 priority = 21;
	if ( burnPriority->Value() != 1 ) {
		priority = 110;
			// Very high - for unreliable drives (or old ones without burn proof).
	}
	return priority;
}

void
SaveConfigView::SetBurnPriority(int32 priority)
{
	// Convert real priority to slider value.
	burnPriority->SetValue((priority == 21 ) ? 1 : 2);
}

exit_modes
SaveConfigView::GetExitMode()
{
	if (autosaveRB->Value()==B_CONTROL_ON) return EM_SAVE_PROJECT;
	if (askforsaveRB->Value()==B_CONTROL_ON) return EM_ASK_USER;
	if (donothingRB->Value()==B_CONTROL_ON) return EM_DO_NOTHING;
	return EM_ASK_USER;
}

void
SaveConfigView::SetExitMode(exit_modes mode) {
	switch(mode) {
		case EM_SAVE_PROJECT: {
			autosaveRB->SetValue(B_CONTROL_ON);
			break;
		}
		
		case EM_ASK_USER: {
			askforsaveRB->SetValue(B_CONTROL_ON);
			break;
		}
		
		case EM_DO_NOTHING: {
			donothingRB->SetValue(B_CONTROL_ON);
			break;
		}
		
		default: {
			break;
		}
	}
}

bool
SaveConfigView::AutoUpdate() {
	return autoupdateCB->Value()==B_CONTROL_ON;
}

void
SaveConfigView::SetAutoUpdate(bool state) {
	autoupdateCB->SetValue(state ? B_CONTROL_ON : B_CONTROL_OFF);
}


void
SaveConfigView::SetBalloonHelp(bool state) {
//	balloonhelpCB->SetValue(state?B_CONTROL_ON:B_CONTROL_OFF);
//	HLP->EnableHelp(state);
}


// PRIVATE


bool	MoveResource(BResources *res, int32 fromID, int32 toID) {
	//BResources	*temp=new BResources();
	size_t		outsize;
	const void	*data;
	const char	*name;
	
	type_code	tc;
	int32		id;
	size_t		length;
	
	if ((data=res->LoadResource(B_MESSAGE_TYPE, fromID, &outsize))!=NULL) {
		res->GetResourceInfo(data, &tc, &id, &length, &name);
		res->AddResource(B_MESSAGE_TYPE, toID, data, outsize, name);
		res->RemoveResource(res->LoadResource(B_MESSAGE_TYPE, fromID, &outsize));
		return true;
	} else
		return false;
}


void
SaveConfigView::MessageReceived(BMessage *msg) {
	switch(msg->what) {
		case BALLOONHELP_MSG: {
//			HLP->EnableHelp(BalloonHelp());
			break;
		}
		case HELIOS_LANGUAGE_UPDATE_MSG: {
			streamedCB->SetLabel(_T("Data streaming")); // "TSTREAMING"
			savewhenquitCB->SetLabel(_T("Remember last project")); // "TSAVEWHENQUIT"
			break;
		}
		
		case BURNPRIORITYSLIDER_CHANGED: {
			// Nothing to do...
			break;
		}
		
		case B_REFS_RECEIVED: {
			break;
		}
		
		case B_CANCEL: {
			// No break apparently - might need to add one...
		}
		
		default: {
			ConfigView::MessageReceived(msg);
			break;
		}
	}
}

void
SaveConfigView::AllAttached()
{
}
