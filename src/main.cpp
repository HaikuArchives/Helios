#include <Application.h>

#if !defined(_BEOS_R5_BUILD_) && !defined(_BEOS_HAIKU_BUILD_)
#include <locale/Locale.h>
#endif

#include "Application1.h"

int
main() {
// Zeta translation manager.
#if !defined(_BEOS_R5_BUILD_) && !defined(_BEOS_HAIKU_BUILD_)
	be_locale.LoadLanguageFile("Language/Dictionaries/Helios");
#endif
// NOTE: R5 version uses MSHLanguageMgr, started in the app itself.
// The same files as Zeta will be used.

	Application1 app;
	app.Run();

	return 0;
};
