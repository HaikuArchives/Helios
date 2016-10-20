#ifndef _MKISOFSCMDLINE_H_
#define _MKISOFSCMDLINE_H_

#include <AppKit.h>

status_t MKISOFS_CommandLine(const char** argv, int* argc, bool printsize = false,
							 const char* outputFile = NULL);

#endif
