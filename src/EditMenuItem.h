#ifndef _EDITMENUITEM_H_
#define _EDITMENUITEM_H_

#include <InterfaceKit.h>

class EditMenuItem : public BMenuItem
{
public:
	EditMenuItem(const char* label, BMessage* message, char shortcut = 0, uint32 modifiers = 0);
	EditMenuItem(BMenu* submenu, BMessage* message = NULL);

	~EditMenuItem();

	void SetRemovable(bool);
	bool IsRemovable();

private:
	bool fIsRemovable;
};

#endif
