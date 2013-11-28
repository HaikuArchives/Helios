#include "FileField.h"

BFileField::BFileField(const char *string, bool isFolder = false) 
		: BStringField(string){
		
	kIsFolder=isFolder;
	kIsParentFolder=false;
	
	if (strcmp(string, "..")==0) SetEditable(false); else SetEditable(true);
	if (strcmp(string, "..")==0) SetParentFolder(true); else SetParentFolder(false);
}

bool	BFileField::IsFolder() {
	return kIsFolder;
}

void	BFileField::SetFolder(bool folder) {
	if (!folder) SetParentFolder(false);
	kIsFolder=folder;
}

bool	BFileField::IsParentFolder() {
	return kIsParentFolder;
}

void	BFileField::SetParentFolder(bool folder) {
	if (folder) SetFolder(true);
	kIsParentFolder=folder;
}
