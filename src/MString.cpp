#include "MString.h"
#include <stdarg.h>
#include <stdio.h>

void MString::TrimLeft()
{
	if (Length() > 0)
		while (this->ByteAt(0) == ' ') {
			Remove(0, 1);
			if (Length() == 0) break;
		}
}

void MString::TrimRight()
{
	if (Length() > 0)
		while (this->ByteAt(Length() - 1) == ' ') {
			Remove(Length() - 1, 1);
			if (Length() == 0) break;
		}
}

void MString::Trim()
{
	TrimLeft();
	TrimRight();
}

void MString::Format(const char* format, ...)
{
	va_list args;
	char* buffer = LockBuffer(16384);
	va_start(args, format);
	vsprintf(buffer, format, args);
	va_end(args);
	UnlockBuffer();
}

// Globals
BString* RemoveTrailingSlash(BString& path)
{
	if (path.ByteAt(path.Length() - 1) == '/') path.RemoveLast("/");
	return &path;
}

void AppendTrailingSlash(BString* path)
{
	if (path->ByteAt(path->Length() - 1) != '/') path->Append("/");
}
