#include <Application.h>

#if defined(_BEOS_R5_BUILD_) || defined(_BEOS_HAIKU_BUILD_)
#include "MSHLanguageMgr.h"
extern MSHLanguageMgr* gMSHLangMgr;
#define _T(str)	gMSHLangMgr->_T(str).String()
#define _TPS(str)	gMSHLangMgr->_T(str)
#else
#include <locale/Locale.h>
#endif

#include "Defines.h"
#include "View2.h"

View2::View2()
 : BView(BRect(0, 0, 262, 64), "View2", B_FOLLOW_TOP_BOTTOM | B_FOLLOW_LEFT_RIGHT, B_WILL_DRAW)
{

	textcontrol3 = new BTextControl(BRect(4, 5, 259, 24), "tc", _T("New name:"), NULL, NULL, B_FOLLOW_LEFT | B_FOLLOW_TOP, B_WILL_DRAW | B_NAVIGABLE);
	textcontrol3->SetAlignment(B_ALIGN_RIGHT, B_ALIGN_LEFT);
	textcontrol3->SetDivider(75);
	textcontrol3->TextView()->SetMaxBytes(700);
	AddChild(textcontrol3);

	button7 = new BButton(BRect(195, 32, 256, 57), "Button", _T("Ok"), new BMessage(OK_MSG), B_FOLLOW_BOTTOM | B_FOLLOW_RIGHT, B_WILL_DRAW);
	button7->MakeDefault(true);
	AddChild(button7);

	button8 = new BButton(BRect(110, 32, 179, 57), "Button", _T("Cancel"), new BMessage(CANCEL_MSG), B_FOLLOW_BOTTOM | B_FOLLOW_RIGHT, B_WILL_DRAW);
	AddChild(button8);
	SetViewColor((rgb_color) {216,216,216,255});
}

View2::~View2(void){
}

void View2::AttachedToWindow(void) {

	textcontrol3->MakeFocus(true);
	//button7->MakeDefault(true);
}

