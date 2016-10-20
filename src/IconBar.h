#ifndef _ICONBAR_H_
#define _ICONBAR_H_

#include <InterfaceKit.h>

class IconBarSeparator : public BView
{
public:
	IconBarSeparator(BRect frame, const char* name,
					 uint32 resizingMode = B_FOLLOW_LEFT | B_FOLLOW_TOP,
					 uint32 flags = B_WILL_DRAW);

protected:
	virtual void Draw(BRect);

private:
};

class IconButton : public BButton
{
public:
	IconButton(BRect frame, const char* name, BBitmap* icon, BMessage* message,
			   uint32 resizingMode = B_FOLLOW_LEFT | B_FOLLOW_TOP,
			   uint32 flags = B_WILL_DRAW | B_NAVIGABLE);
	~IconButton();

protected:
	virtual void Draw(BRect);
	virtual void DrawFrame(bool pressed);

private:
	BBitmap* fIcon;
	BRect fBounds;
};

class IconBar : public BView
{
public:
	IconBar(BRect frame, const char* name, uint32 resizingMode = B_FOLLOW_LEFT | B_FOLLOW_TOP,
			uint32 flags = B_WILL_DRAW | B_FRAME_EVENTS);
	~IconBar();

	void AddChildView(BView* button);
	int32 AddButton(const char* iconname, const char* name, BMessage* msg,
					uint32 resizingMode = B_FOLLOW_LEFT | B_FOLLOW_TOP, bool miniicon = true);
	int32 AddButton(int32 ID, const char* name, BMessage* msg,
					uint32 resizingMode = B_FOLLOW_LEFT | B_FOLLOW_TOP, bool miniicon = true);
	int32 AddButtonMSGG(const char* iconname, const char* name, BMessage* msg,
						uint32 resizingMode = B_FOLLOW_LEFT | B_FOLLOW_TOP);
	BView* RemoveChildView(const char* name);
	BView* ChildViewAt(int32);
	BView* ChildViewAt(const char* name);

protected:
	virtual void Draw(BRect);
	virtual void FrameResized(float width, float height);

private:
	BList* fButtonL;
};

#endif
