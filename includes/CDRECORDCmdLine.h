#ifndef _CDRECORDCMDLINE_H_
#define _CDRECORDCMDLINE_H_

#include <AppKit.h>
#include <List.h>
#include <String.h>

status_t CDRECORD_CommandLine(const char **argv, int *argc, 
		bool multisession=false, bool isStreaming=true);

void CDRECORD_ParseUserFlags(BString userFlags, BList &argList);
	// Processes the passed in 'userFlags' string,
	// and separates out the individual arguments,
	// removing whitespace as it goes.
	// NOTE: The caller takes ownership of the allocated
	// strings in the argList AND MUST DELETE THEM.

#endif

