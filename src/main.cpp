#include <Application.h>

#if !defined(_BEOS_R5_BUILD_) && !defined(_BEOS_HAIKU_BUILD_)
#include <locale/Locale.h>
#endif

#include "Application1.h"

int
main() {

	Application1 app;
	app.Run();

	return 0;
};
