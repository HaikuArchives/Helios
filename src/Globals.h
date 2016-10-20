#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#if defined(_BEOS_R5_BUILD_) || defined(_BEOS_HAIKU_BUILD_)
#include "MSHLanguageMgr.h"
_EXPORT MSHLanguageMgr*		gMSHLangMgr;
	// Global language manager.
#endif
_EXPORT bool 	project_has_changed;

#endif
