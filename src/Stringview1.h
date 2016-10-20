#ifndef _STRINGVIEW1_H_
#define _STRINGVIEW1_H_

#include <View.h>
#include <Bitmap.h>
#include <Node.h>
#include <String.h>

class Stringview1 : public BView {
public:
	Stringview1(BRect frame,const char *name,uint32 resizeMask,uint32 flags);
	~Stringview1 (void) {};

	virtual void SetText(const char *utext);
	virtual void SetTypeText(const char *utypetext);
	virtual void SetFileTypeText(BNode *unode);
	virtual void SetIcon(BNode unode);
	virtual void Clear();

	virtual void Draw(BRect updateRect);
private:
	BBitmap *bmp;
	BString *text, *typetext;
	bool hasIcon;
};

#endif
