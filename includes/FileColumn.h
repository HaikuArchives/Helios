#ifndef _FILESCOLUMN_H_
#define _FILESCOLUMN_H_

#include "ColumnTypes.h"
#include "ColumnListView.h"
#include "FileField.h"
#include <PopUpMenu.h>
#include <MenuItem.h>

class BFileColumn : public BStringColumn
{
	public:
						BFileColumn(const char *title,
								float width,
								float maxWidth,
								float minWidth,
								uint32 truncate,
								alignment align = B_ALIGN_LEFT,
								bool contextmenu=false);
		void				SortFoldersFirst(bool);
		virtual int			CompareFields(BField* field1,
									BField* field2);
		//virtual void 		MouseDown(BColumnListView *parent, BRow *row, BField *field,
		//			              BRect field_rect, BPoint point, uint32 buttons);


	private:
		bool	kFoldersFirst;
		BPopUpMenu	*cmenu;
		BMenuItem	*deleteItem,
				*newfolderItem,
				*renamefileItem,
				*moveupItem,
				*movedownItem,
				*indexItem,
				*splitItem,
				*CDTEXTTitleItem;
		bool	contextM;
};

#endif
