#ifndef _WINDOW5_H_
#define _WINDOW5_H_

#include <Window.h>
#include <View.h>
#include <Bitmap.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "StatusView.h"

class StatusWindow : public BWindow
{
public:
	StatusWindow();
	~StatusWindow();

	virtual bool QuitRequested();
	virtual void MessageReceived(BMessage* message);
	virtual void Show();
	void SetBarColor(rgb_color color);

	void DisableControls();
	void EnableControls();

	StatusView* fStatusView;
	thread_id Thread;
	bool interrupted;
};

#endif
