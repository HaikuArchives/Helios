#if defined(_BEOS_R5_BUILD_) || defined(_BEOS_HAIKU_BUILD_)
#include "MSHLanguageMgr.h"
extern MSHLanguageMgr* gMSHLangMgr;
#define _T(str)	gMSHLangMgr->_T(str).String()
#define _TPS(str)	gMSHLangMgr->_T(str)
#else
#include <locale/Locale.h>
#endif

#include "LogSettingsWindow.h"

_CLASSNAME::_CLASSNAME(BRect frame, const char *title)
		:BWindow(frame, title, B_TITLED_WINDOW, B_NOT_ZOOMABLE|B_NOT_RESIZABLE|B_NOT_MINIMIZABLE|B_ASYNCHRONOUS_CONTROLS,
				B_CURRENT_WORKSPACE) {
	mainV=new BView(Bounds(), "mainV", B_FOLLOW_ALL, B_WILL_DRAW);
	mainV->SetViewColor(216, 216, 216);
	AddChild(mainV);
	
	loggingCB=new BCheckBox(BRect(15,15, 285, 35), "loggingCB", _T("Enable logging"), // "CB:Enable Logging"
			NULL, B_FOLLOW_LEFT_RIGHT|B_FOLLOW_TOP);
	mainV->AddChild(loggingCB);
}


_CLASSNAME::~_CLASSNAME() {
}

bool _CLASSNAME::QuitRequested() {
	return true;
}

void _CLASSNAME::LoadSettings(BMessage *msg) {
	BRect	frame;
	
	msg->FindRect("Frame", &frame);
}

void _CLASSNAME::SaveSettings(BMessage *msg) {
	msg->AddRect("Frame", Frame());
	
}

void _CLASSNAME::MessageReceived(BMessage *msg) {
	switch(msg->what) {
		default: _INHERITED::MessageReceived(msg);
	}
}
