#ifndef _EDITVIEW_H_
#define _EDITVIEW_H_

#include <InterfaceKit.h>
#include <String.h>

class TextEditView;

class EditView : public BView
{
public:
	EditView(BRect frame, const char* name, const char* text, BMessage* invokeMsg,
			 uint32 resizingMode);
	~EditView();

	void SetText(const char* text);
	const char* Text();
	void SetInvocationMessage(BMessage* msg);
	void SetAlignment(alignment align);
	alignment Alignment();

private:
	virtual void MouseDown(BPoint point);
	virtual void Draw(BRect);

	BMessage* fInvocationMsg;
	BString fText;
	TextEditView* editTEV;
	alignment fAlignment;
};

class TextEditView : public BTextView
{
public:
	TextEditView(EditView* parent);
	~TextEditView();

	virtual void KeyDown(const char* bytes, int32 numBytes);

private:
	EditView* fParentView;
};

#endif
