#ifndef _WINDOW1_H_
#define _WINDOW1_H_

#include <Window.h>
#include <View.h>
#include <Bitmap.h>
#include "View1.h"
#include <Alert.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <String.h>
#include <File.h>
#include <ListItem.h>



class Window1 : public BWindow {
public:
	Window1 ();
	~Window1 (void);

	virtual bool QuitRequested(void);
	virtual void MessageReceived(BMessage *message);
	virtual void WorkspaceActivated(int32 workspace, bool active);
	View1 *view1;
	bool Alert(char *text);
//	void Update_sizes();

	int number;
};

#endif
