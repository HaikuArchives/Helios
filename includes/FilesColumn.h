#ifndef _FILESCOLUMN_H_
#define _FILESCOLUMN_H_

#include "ColumnTypes.h"
#include "ColumnListView.h"

class BFileColumn : public BStringColumn {
public:
					BFileColumn(const char *title,
							float width,
							float maxWidth,
							float minWidth,
							uint32 truncate,
							alignment align = B_ALIGN_LEFT);
		void				SortFoldersFirst(bool);
		virtual int			CompareFields(BField* field1,
									BField* field2);

private:
		bool	kFoldersFirst;
};

#endif
