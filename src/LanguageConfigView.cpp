//=============================================================================
// Language Config View - Currently only R5, but may be used in Zeta later
//												for completeness (depends if it turns out to be
//												useful rather than confusing to have a language
//												override.
//=============================================================================

const char kTagForOverrideZetaLocale[]	= "Override Zeta Localization";
const char kTagForSelectLanguage[]			= "Select your language";
const char kTagForRestartHelios[]				= "Note: You must restart Helios for changes to take effect.";

#if defined(_BEOS_R5_BUILD_) || defined(_BEOS_HAIKU_BUILD_)
#include "MSHLanguageMgr.h"
extern MSHLanguageMgr* gMSHLangMgr;
#define _T(str)	gMSHLangMgr->_T(str).String()
#define _TPS(str)	gMSHLangMgr->_T(str)
//MSH: Next two lines commented out for possible future usage.
//#else
//#include <locale/Locale.h>

#include "LanguageConfigView.h"

#include <Invoker.h>
#include <Looper.h>
#include <Menu.h>
#include <MenuItem.h>
#include <Messenger.h>

#define OVERRIDEZETACHECKBOX_CLICKED	'LCV0'
#define LANGUAGEMENU_CLICKED					'LCV1'

LanguageConfigView::LanguageConfigView(BRect& frame,
		const BString& savedLanguageTag) :
	ConfigView(frame), 
	fLanguageMenu(NULL),
	fLanguageListMF(NULL)
{
	float currControlTopPos = 5;

	// Zeta localisation overriding checkbox.
#if !defined(_BEOS_R5_BUILD_) && !defined(_BEOS_HAIKU_BUILD_)
	fOverrideZetaLocaleCB = new BCheckBox(15, 5, frame.Width() - 15, 20),
			_T(kTagForOverrideZetaLocale),
			new BMessage(OVERRIDEZETACHECKBOX_CLICKED));
	currControlTopPos = fOverrideZetaLocaleCB->Frame().bottom + 5;
#endif

	// Language selection text.
	BString tempstring;
	tempstring << _T(kTagForSelectLanguage) << B_UTF8_ELLIPSIS;
	fSelectLanguageSV = new BStringView(BRect(15, currControlTopPos, 280, 80), "lcv_selectlanguageSV", tempstring.String());
	fSelectLanguageSV->ResizeToPreferred();
	AddChild(fSelectLanguageSV);
	currControlTopPos = fSelectLanguageSV->Frame().bottom + 5;

	// Language menu.
	SetupLanguageMenu(savedLanguageTag);
	fLanguageListMF = new BMenuField(BRect(15, currControlTopPos, frame.Width() - 15, currControlTopPos + 15),
			NULL,
			NULL,
			fLanguageMenu);
	fLanguageListMF->ResizeToPreferred();
	AddChild(fLanguageListMF);
	currControlTopPos = fLanguageListMF->Frame().bottom + 5;

	tempstring = _T(kTagForRestartHelios);
	fRestartMessageSV = new BStringView(BRect(15, currControlTopPos, frame.Width() - 15, currControlTopPos + 15), "lcv_restartmessageSV", tempstring.String());
	fRestartMessageSV->ResizeToPreferred();
	AddChild(fRestartMessageSV);
	currControlTopPos = fRestartMessageSV->Frame().bottom + 5;
}

LanguageConfigView::~LanguageConfigView()
{
}

BString
LanguageConfigView::GetLanguageTag()
{
	BString languageTag("enUS");

	BMenuItem* firstMarkedItem = fLanguageMenu->FindMarked();
	if (NULL != firstMarkedItem) {
		languageTag = BString(firstMarkedItem->Label());
	}
	
	return languageTag;
}

void
LanguageConfigView::AllAtached()
{
}

void
LanguageConfigView::AllDetached()
{
}

void
LanguageConfigView::MessageReceived(BMessage* msg)
{
	switch(msg->what) {
#if !defined(_BEOS_R5_BUILD_) && !defined(_BEOS_HAIKU_BUILD_)
		case OVERRIDEZETACHECKBOX_CLICKED : {
			// TODO: Future implementation.
			break;
		}
#endif

		case LANGUAGEMENU_CLICKED: {
			// Update the current language selection.
			gMSHLangMgr->SelectLanguageByName(GetLanguageTag());
			UpdateViewAfterLanguageChange();
			break;
		}
		
		default: {
			ConfigView::MessageReceived(msg);
			break;
		}
	}
}

void
LanguageConfigView::UpdateViewAfterLanguageChange()
{
#if !defined(_BEOS_R5_BUILD_) && !defined(_BEOS_HAIKU_BUILD_)
	fOverrideZetaLocaleCB->SetText(_T(kTagForRestartHelios));
#endif
	fSelectLanguageSV->SetText(_T(kTagForSelectLanguage));
	fRestartMessageSV->SetText(_T(kTagForRestartHelios));
}

void
LanguageConfigView::SetupLanguageMenu(const BString& savedLanguageTag)
{
	if (NULL == fLanguageMenu) {
		fLanguageMenu = new BPopUpMenu("lcv_languagemenu");
	}

	const int32 numMenuItems = fLanguageMenu->CountItems();
	for (int32 itemIndex = 0; itemIndex < numMenuItems; itemIndex++) {
		fLanguageMenu->RemoveItem(static_cast<int32>( 0 ));
			// Always remove the first item since they will change as
			// the list shrinks.
	}

	// Read the language names and add them to the menu list.
	const int32 numLanguages = gMSHLangMgr->CountLanguages();
	for (int32 currLang = 0; currLang < numLanguages; currLang++) {
		BString languageTag = gMSHLangMgr->GetLanguageName(currLang);

		//MSH:	Note: the following code does not work entirely yet.
		//			I am attempting to make the menu item's send messages
		//			when they are clicked upon (i.e. the language changes)
		//			but the MessageReceived() function never received the
		//			LANGUAGEMENU_CLICKED message. Any ideas? (I'm sure it's
		//			an obvious mistake...)
		BMessage* itemMessage = new BMessage(LANGUAGEMENU_CLICKED);
		BMenuItem *menuItem = new BMenuItem(languageTag.String(), itemMessage);
		menuItem->SetTarget(this);
		fLanguageMenu->AddItem(menuItem);

		// Set to currently selected language, if found...
		if (languageTag == savedLanguageTag) {
			fLanguageMenu->ItemAt(currLang)->SetMarked(true);
			gMSHLangMgr->SelectLanguage(currLang);
		}
	}
}

#endif
// _BEOS_R5_BUILD_ 
