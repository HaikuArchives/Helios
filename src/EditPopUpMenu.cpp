#ifndef _ZETA_USING_DEPRECATED_API_
#define _ZETA_USING_DEPRECATED_API_
#endif
	// To allow PostMessage() to work. Grrr...

#include "EditPopUpMenu.h"
#include "EditMenuItem.h"
#include <support/ClassInfo.h>

EditPopUpMenu::EditPopUpMenu(const char *name,
			bool radioMode,
			bool labelFromMarked,
			menu_layout layout)
		: BPopUpMenu(name, radioMode, labelFromMarked, layout) {

}


EditPopUpMenu::~EditPopUpMenu() {
	if (fRemoveMsg)
		delete fRemoveMsg;
}

void EditPopUpMenu::KeyDown(const char *bytes, int32 numBytes) {
	if (bytes[0]==B_DELETE) {
		BPoint	location;
		uint32	buttons;
		EditMenuItem	*item;
		
		GetMouse(&location, &buttons);
		
		for (int32 i=CountItems()-1; i>=0; i--) {
			item=(EditMenuItem *)ItemAt(i);
			if (is_instance_of(item, EditMenuItem)) {
				if (!item->IsRemovable())
					continue;
				if (item->Frame().Contains(location)) {
					BMessage	*msg=NULL;
					if ((fRemoveMsg) && (fRemoveMsgTarget)) {
						msg=new BMessage(*fRemoveMsg);
						msg->AddInt32("from", IndexOf(item));
					}
					if (RemoveItem(item)) {
						delete item;
					}
					if (msg!=NULL)	
						fRemoveMsgTarget->Looper()->PostMessage(msg, fRemoveMsgTarget);
					break;
				}
			}
		}
	} else
		BPopUpMenu::KeyDown(bytes, numBytes);
}


void EditPopUpMenu::SetRemoveMessage(BMessage *msg) {
	if (fRemoveMsg)
		delete fRemoveMsg;
	fRemoveMsg=msg;
}

void EditPopUpMenu::SetRemoveMessageTarget(BHandler *handler) {
	fRemoveMsgTarget=handler;
}
