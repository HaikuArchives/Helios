#ifndef _FILEFIELD_H_
#define _FILEFIELD_H_

#include "ColumnTypes.h"


class BFileField : public BStringField {
public:
				BFileField(const char* string, bool isFolder);
							
		bool		IsFolder();
		void		SetFolder(bool);
		bool		IsParentFolder();
		void		SetParentFolder(bool);
	
	private:
		bool	kIsFolder,
			kIsParentFolder;
};

#endif
