#include "EditMenuItem.h"

EditMenuItem::EditMenuItem(const char* label, BMessage* message, char shortcut, uint32 modifiers)
	: BMenuItem(label, message, shortcut, modifiers)
{

	fIsRemovable = true;
}

EditMenuItem::EditMenuItem(BMenu* submenu, BMessage* message) : BMenuItem(submenu, message)
{

	fIsRemovable = true;
}

EditMenuItem::~EditMenuItem()
{
}

void EditMenuItem::SetRemovable(bool state)
{
	fIsRemovable = state;
}

bool EditMenuItem::IsRemovable()
{
	return fIsRemovable;
}
