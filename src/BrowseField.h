#ifndef _BROWSEFIELD_H_
#define _BROWSEFIELD_H_

#include <View.h>
#include <Button.h>
#include <TextControl.h>
#include <String.h>
#include <Path.h>
#include <Entry.h>
#include "FileDialog.h"

class BrowseField : public BView
{
public:
	BrowseField(BRect frame, const char* name, uint32 resizingMode = B_FOLLOW_LEFT | B_FOLLOW_TOP,
				uint32 flags = B_WILL_DRAW);
	~BrowseField();

	void Init(const char* title, const char* path = NULL, uint32 node_flavors = B_FILE_NODE);

	BButton* GetButton();
	BTextControl* GetTextControl();

protected:
	virtual void MessageReceived(BMessage*);
	virtual void AttachedToWindow();

private:
	BTextControl* fileTC;
	BButton* browseB;

	BString fTitle;
	BString fPath;
	uint32 fNodeFlavors;
};

#endif
