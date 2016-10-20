#ifndef _EDITPOPUPMENU_H_
#define _EDITPOPUPMENU_H_

#include <InterfaceKit.h>

class EditPopUpMenu : public BPopUpMenu
{
public:
	EditPopUpMenu(const char* name, bool radioMode = true, bool labelFromMarked = true,
				  menu_layout layout = B_ITEMS_IN_COLUMN);
	~EditPopUpMenu();

	virtual void KeyDown(const char* bytes, int32 numBytes);
	void SetRemoveMessage(BMessage*);
	void SetRemoveMessageTarget(BHandler*);

private:
	BMessage* fRemoveMsg;
	BHandler* fRemoveMsgTarget;
};

#endif
