#ifndef _ERRORBOX_H_
#define _ERRORBOX_H_

#include <Window.h>
#include <View.h>
#include <Bitmap.h>
#include <Rect.h>
#include <StringView.h>
#include <TextView.h>
#include <Button.h>

typedef enum {
	E_RED_COLOR=1,
	E_ORANGE_COLOR=2,
	E_YELLOW_COLOR=3,
	E_NORMAL_COLOR=4,
	E_BLUE_COLOR=5,
	E_GREEN_COLOR=6
} signal_color;


class ErrorView : public BView {
public:
	ErrorView(BRect frame, const char *name, signal_color color);
	~ErrorView();
	
	virtual void 	Draw(BRect updateRect);
private:
	rgb_color	viewColor;
};

class ErrorBox : public BWindow {
public:
	ErrorBox(signal_color color, const char *title, const char *errortext, const char *rightbutton, const char *middlebutton="", const char *leftbutton="");
	~ErrorBox();
	
	int 	Go();
	void 	SetText(const char *);
	void	SetShortcut(int32 index, uint32 shortcut);
	
	
private:

	virtual bool 	QuitRequested();
	virtual void	MessageReceived(BMessage *msg);

	ErrorView	*vError;
	BStringView	*errorSV,
			*versionNumberSV,
			*copyrightStringSV;
	BTextView	*textTV;
	BButton	*leftB,
			*middleB,
			*rightB;
	sem_id		sem;
	BRect		frame;
	int		retval;
	int8		lb, mb, rb;
};


#endif
