#include <Looper.h>

#if defined(_BEOS_R5_BUILD_) || defined(_BEOS_HAIKU_BUILD_)
#include "MSHLanguageMgr.h"
extern MSHLanguageMgr* gMSHLangMgr;
#define _T(str)	gMSHLangMgr->_T(str).String()
#define _TPS(str)	gMSHLangMgr->_T(str)
#else
#include <locale/Locale.h>
#endif

#include "Application1.h"
#include "ApplicationConfigView.h"

#define UPDATEFREQSLIDER_CHANGED	'ACV0'
#define TIMEOUTSLIDER_CHANGED	'ACV1'
#define UPDATECHECKBOX_CLICKED	'ACV2'
#define TIMEOUTCHECKBOX_CLICKED	'ACV3'


ApplicationConfigView::ApplicationConfigView(BRect frame) : ConfigView(frame){
	
	// CHECKBOXES
	updatefreqCB=new BCheckBox(BRect(15,5,230,20), "acv_updatefreqCB",
													_T("Update Infobox automatically"), // "TENABLEAUTOUPDATING"
													new BMessage(UPDATECHECKBOX_CLICKED));
	updatefreqCB->ResizeToPreferred();
	AddChild(updatefreqCB);
	
	// SLIDERS
	updatefreqSL=new BSlider(BRect(15, 25, frame.Width()-15, 55), "acv_updatefreqSL", NULL, new BMessage(UPDATEFREQSLIDER_CHANGED), 1, 20);
	updatefreqSL->SetHashMarks(B_HASH_MARKS_BOTTOM);
	updatefreqSL->SetHashMarkCount(20);
	updatefreqSL->SetKeyIncrementValue(1);
	updatefreqSL->SetLimitLabels("15s", "300s");
	AddChild(updatefreqSL);

	// CHECKBOXES
	timeoutCB=new BCheckBox(BRect(15,60,230,75), "acv_timeoutCB",
													_T("Timeouts"), // "TENABLETIMEOUT"
													new BMessage(TIMEOUTCHECKBOX_CLICKED));
	timeoutCB->ResizeToPreferred();
	AddChild(timeoutCB);
	
	// SLIDERS
	timeoutSL=new BSlider(BRect(15, 80, frame.Width()-15, 105), "acv_timeoutSL", NULL, new BMessage(TIMEOUTSLIDER_CHANGED), 2, 90);
	timeoutSL->SetHashMarks(B_HASH_MARKS_BOTTOM);
	timeoutSL->SetHashMarkCount(22);
	timeoutSL->SetKeyIncrementValue(1);
	timeoutSL->SetLimitLabels("2s", "90s");
	AddChild(timeoutSL);
}

ApplicationConfigView::~ApplicationConfigView() {
}

bool	ApplicationConfigView::IsAutoUpdating() {
	return updatefreqCB->Value()==B_CONTROL_ON;
}

void	ApplicationConfigView::SetAutoUpdating(bool state) {
	updatefreqCB->SetValue(state ? B_CONTROL_ON : B_CONTROL_OFF);
	updatefreqSL->SetEnabled(updatefreqCB->Value()==B_CONTROL_ON);
	if (IsAutoUpdating()) {
		((Application1 *)be_app)->window1->view1->box2->SpawnUpdateThread();
		((Application1 *)be_app)->window1->view1->box2->StartUpdating();
		((Application1 *)be_app)->window1->view1->box2->SetUpdateFrequency(GetUpdateFrequency());
	} else {
		((Application1 *)be_app)->window1->view1->box2->ExitUpdateThread();
	}
}

int	ApplicationConfigView::GetUpdateFrequency() {
	return updatefreqSL->Value()*15;
}

void	ApplicationConfigView::SetUpdateFrequency(int seconds) {
	BString str=_T("Update Infobox automatically"); // "TENABLEAUTOUPDATING"
	str << ": (" << seconds << "s)";
	updatefreqSL->SetValue((int32)(seconds/(int)15));
	updatefreqCB->SetLabel(str.String());
	updatefreqCB->ResizeToPreferred();
	if (IsAutoUpdating())
		((Application1 *)be_app)->window1->view1->box2->SetUpdateFrequency(seconds);
}

bool	ApplicationConfigView::IsTimeout() {
	return timeoutCB->Value()==B_CONTROL_ON;
}

void	ApplicationConfigView::SetTimeout(bool state) {
	timeoutCB->SetValue(state ? B_CONTROL_ON : B_CONTROL_OFF);
	timeoutSL->SetEnabled(timeoutCB->Value()==B_CONTROL_ON);
}

int	ApplicationConfigView::GetTimeoutLength() {
	return timeoutSL->Value();
}

void	ApplicationConfigView::SetTimeoutLength(int seconds) {
	BString str=_T("Timeouts"); // "TENABLETIMEOUT"
	str << ": (" << seconds << "s)";
	timeoutSL->SetValue(seconds);
	timeoutCB->SetLabel(str.String());
	timeoutCB->ResizeToPreferred();
}

// PRIVATE

void	ApplicationConfigView::AllAttached() {
	updatefreqSL->SetTarget(this);
	updatefreqSL->SetModificationMessage(new BMessage(UPDATEFREQSLIDER_CHANGED));
	timeoutSL->SetTarget(this);
	timeoutSL->SetModificationMessage(new BMessage(TIMEOUTSLIDER_CHANGED));
	timeoutCB->SetTarget(this);
	updatefreqCB->SetTarget(this);
	ConfigView::AllAttached();
}

void	ApplicationConfigView::AllDetached() {
}

void	ApplicationConfigView::MessageReceived(BMessage *msg) {
	switch(msg->what) {
		case UPDATEFREQSLIDER_CHANGED: {
			SetUpdateFrequency(GetUpdateFrequency());
			break;
		}
		
		case TIMEOUTSLIDER_CHANGED: {
			SetTimeoutLength(GetTimeoutLength());
			break;
		}
		
		case UPDATECHECKBOX_CLICKED: {
			updatefreqSL->SetEnabled(updatefreqCB->Value()==B_CONTROL_ON);
			SetAutoUpdating(IsAutoUpdating());
			break;
		}
		
		case TIMEOUTCHECKBOX_CLICKED: {
			timeoutSL->SetEnabled(timeoutCB->Value()==B_CONTROL_ON);
			break;
		}

		default: {
			BView::MessageReceived(msg);
			break;
		}
	}
}
