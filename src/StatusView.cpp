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
#include "StatusView.h"

StatusView::StatusView() :
		BView(BRect(0, 0, 276, 60), "StatusView", B_FOLLOW_TOP_BOTTOM | B_FOLLOW_LEFT_RIGHT, B_WILL_DRAW | B_NAVIGABLE)
{
	font14 = new BFont(be_bold_font);
	font14->SetSize(12);
	font14->SetSpacing(B_BITMAP_SPACING);

	stringview12 = new BStringView(BRect(4, 1, 272, 21), "v5_statusSV", "Label",
			B_FOLLOW_TOP | B_FOLLOW_LEFT_RIGHT, B_WILL_DRAW);
	stringview12->SetFont(font14);
	AddChild(stringview12);

	button17 = new BButton(BRect(205, 33, 274, 58), "v5_cancelB", _T("Cancel"), new BMessage(KILL_MSG),
			B_FOLLOW_TOP | B_FOLLOW_RIGHT, B_WILL_DRAW);
	AddChild(button17);

	progressSB=new BStatusBar(BRect(5,23,200,58),"v5_progressSB");
	progressSB->SetBarColor((rgb_color){60,225,60,255});
	AddChild(progressSB);

//	infoSV=new BStringView(BRect(130,17,202,32),"infoSV","");
//	infoSV->SetAlignment(B_ALIGN_RIGHT);
//	AddChild(infoSV);

	SetViewColor((rgb_color) {216,216,216,255});
}

StatusView::~StatusView(void)
{
}
