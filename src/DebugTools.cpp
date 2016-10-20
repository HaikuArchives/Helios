// Debug tools implementation.

#include "DebugTools.h"

#include <stdio.h>
#include <String.h>

const char* printArguments(char* title, const char** args)
{
	BString str = "";
	int i = 0;
	printf("HELIOS: %s\n", title);
	while (args[i] != NULL) {
		printf("\targument(%d)=%s\n", i + 1, args[i]);
		i++;
	}
	printf("\n");

	i = 0;
	while (args[i] != NULL) {
		str << args[i] << " ";
		i++;
	}

	return strdup(str.String());
}
