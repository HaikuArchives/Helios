#ifndef _CDTYPE_MENU_H_
#define _CDTYPE_MENU_H_

#include <Menu.h>
#include <MenuItem.h>
#include <PopUpMenu.h>
#include <Message.h>
#include <MenuField.h>
#include "Defines.h"

class CDTypeMenu : public BMenuField
{
public:
	CDTypeMenu(BRect frame, BMessage* msg);
	~CDTypeMenu();

	BPopUpMenu* GetMenu();

private:
	BPopUpMenu* CDTypeM;
	BMenuItem* audioCDTypeMI, *dataCDTypeMI, *extraCDTypeMI, *bootableCDTypeMI;

#ifdef INCLUDE_DVD
	BMenuItem* dvdvideoCDTypeMI;
#endif

	BMessage* message;
};

#endif
