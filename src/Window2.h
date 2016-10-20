#ifndef _WINDOW2_H_
#define _WINDOW2_H_

#include <Window.h>
#include <View.h>
#include <Bitmap.h>
#include "View2.h"

class Window2 : public BWindow
{
public:
	Window2();
	~Window2(void);

	virtual bool QuitRequested(void);
	virtual void MessageReceived(BMessage* message);
	virtual void Show(void);
	View2* view2;
	char todo;
	bool shown;
};

#endif
