//=============================================================================
// Language Config View
//=============================================================================

#ifndef _LANGUAGECONFIGVIEW_H_
#define _LANGUAGECONFIGVIEW_H_

#include <CheckBox.h>
#include <MenuField.h>
#include <PopUpMenu.h>
#include <Rect.h>
#include <String.h>
#include <StringView.h>

#include "ConfigView.h"

class LanguageConfigView : public ConfigView {
public:
													LanguageConfigView(BRect& frame,
															const BString& savedLanguageTag);
													~LanguageConfigView();
														// Constructor/destructor.

	BString									GetLanguageTag();
														// Returns the language tag for the currently
														// selected language e.g. enUS for US English.
private:
	void										AllAtached();
														// Sets the targets for messenging, etc. once
														// all children are attached.

	void										AllDetached();
														// Saves any required data after everything has been
														// disconnected from the view.

	void										MessageReceived(BMessage*);
														// Processes events for the language config view.

	void										UpdateViewAfterLanguageChange();
														// Updates all text after a language change.
														// Note: only applies to this view - a restart
														// of Helios will be needed to force all text
														// to change (no different than in Zeta).

	void										SetupLanguageMenu(const BString& savedLanguageTag);
														// Populates the language menu items, removing old
														// ones first if needed.

	BStringView							*fSelectLanguageSV;
														// Label to instruct the user to select a language.

	BPopUpMenu							*fLanguageMenu;
														// Language menu - to be attached to a menu field.

	BMenuField							*fLanguageListMF;
														// Lists all languages available, and highlights
														// the currently active one as the default item.

	BStringView							*fRestartMessageSV;
														// Message to tell users to restart Helios after changes.
};

#endif
// _LANGUAGECONFIGVIEW_H_
