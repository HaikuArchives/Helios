#ifndef _MSTRING_H_
#define _MSTRING_H_

#include <String.h>

class MString : public BString
{
public:
	MString() : BString(){};
	MString(const char* str) : BString(str){};
	MString(const MString& str) : BString(str){};
	MString(const char* str, int32 maxLength) : BString(str, maxLength){};

	~MString(){};

	void TrimLeft();
	void TrimRight();
	void Trim();

	void Format(const char* format, ...);

protected:
private:
};

// Globals
BString* RemoveTrailingSlash(BString& path);
void AppendTrailingSlash(BString* path);

#endif
