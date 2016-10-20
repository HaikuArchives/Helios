#if defined(_BEOS_R5_BUILD_) || defined(_BEOS_HAIKU_BUILD_)
#include "MSHLanguageMgr.h"
extern MSHLanguageMgr* gMSHLangMgr;
#define _T(str) gMSHLangMgr->_T(str).String()
#define _TPS(str) gMSHLangMgr->_T(str)
#else
#include <locale/Locale.h>
#endif

#include "CDTypeMenu.h"

CDTypeMenu::CDTypeMenu(BRect frame, BMessage* msg)
	: BMenuField(frame, NULL, NULL, (CDTypeM = new BPopUpMenu("")))
{

	CDTypeM->SetRadioMode(true);
	CDTypeM->SetLabelFromMarked(true);
	// MenuItems for the cd type menu...
	dataCDTypeMI = new BMenuItem(_T("Data CD"), // "TDATACD"
								 (msg != NULL) ? new BMessage(*msg) : NULL);
	CDTypeM->AddItem(dataCDTypeMI);
	audioCDTypeMI = new BMenuItem(_T("Audio CD"), // "TAUDIOCD"
								  (msg != NULL) ? new BMessage(*msg) : NULL);
	CDTypeM->AddItem(audioCDTypeMI);
	bootableCDTypeMI = new BMenuItem(_T("Bootable CD"), // "TBOOTABLECD"
									 (msg != NULL) ? new BMessage(*msg) : NULL);
	CDTypeM->AddItem(bootableCDTypeMI);
	extraCDTypeMI = new BMenuItem(_T("CD Extra"), // "TEXTRACD"
								  (msg != NULL) ? new BMessage(*msg) : NULL);
	CDTypeM->AddItem(extraCDTypeMI);

#ifdef INCLUDE_DVD
	CDTypeM->AddSeparatorItem();
	dvdvideoCDTypeMI = new BMenuItem(_T("Data DVD"), // "TDVDVIDEO"
									 (msg != NULL) ? new BMessage(*msg) : NULL);
	CDTypeM->AddItem(dvdvideoCDTypeMI);
#endif

	// extraCDTypeMI->SetEnabled(false);
	// BMenuField which will be in the label view of CDTypeBOX
	// for selecting a specific cd-type
	delete msg;
}

CDTypeMenu::~CDTypeMenu()
{
}

BPopUpMenu* CDTypeMenu::GetMenu()
{
	return CDTypeM;
}
