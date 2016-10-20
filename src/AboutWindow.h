#ifndef _ABOUTWINDOW_H_
#define _ABOUTWINDOW_H_

#include <Window.h>
#include <View.h>
#include <Bitmap.h>
#include <Rect.h>
#include <StringView.h>
#include <TextView.h>
#include <ScrollView.h>

class AboutView : public BView
{
public:
	AboutView(BRect frame, const char* name, BBitmap* icon);
	~AboutView();

	virtual void Draw(BRect updateRect);
	void SetIcon(BBitmap* icon);

private:
	BBitmap* bIcon;
};

class AboutWindow : public BWindow
{
public:
	AboutWindow(BRect frame, const char* title);
	~AboutWindow();

	void SetApplicationName(const char*);
	void SetVersionNumber(const char*);
	void SetIcon(BBitmap*);
	void SetCopyrightString(const char*);
	void SetText(const char*);

	virtual bool QuitRequested();

private:
	AboutView* vAbout;
	BStringView* applicationNameSV, *versionNumberSV, *copyrightStringSV;
	BTextView* textTV;
	BScrollView* textSV;
};

#endif
